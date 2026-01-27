#include "unipm/executor.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <array>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "unipm/safety.h"

namespace unipm {

ExecutionResult Executor::execute(const std::string& command, bool requiresRoot) {
    std::string finalCommand = command;

    // Add sudo if needed (Unix only)
#ifndef _WIN32
    if (requiresRoot && !isAdmin()) {
        if (hasSudo()) {
            finalCommand = prependSudo(command);
        }
    }
#endif

    // Log the operation
    Safety::logOperation(finalCommand, false);

    ExecutionResult result;
    result.command = finalCommand;

#ifdef _WIN32
    result = executeWindows(finalCommand);
#else
    result = executeUnix(finalCommand);
#endif

    // Log success/failure
    Safety::logOperation(finalCommand, result.success);

    return result;
}

void Executor::preview(const std::string& command, bool requiresRoot) {
    std::string finalCommand = command;

#ifndef _WIN32
    if (requiresRoot && !isAdmin() && hasSudo()) {
        finalCommand = prependSudo(command);
    }
#endif

    std::cout << "Would execute: " << finalCommand << std::endl;
}

bool Executor::hasSudo() {
#ifdef _WIN32
    return false;
#else
    return system("which sudo >/dev/null 2>&1") == 0;
#endif
}

bool Executor::isAdmin() {
#ifdef _WIN32
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin == TRUE;
#else
    return geteuid() == 0;
#endif
}

std::string Executor::prependSudo(const std::string& command) {
    return "sudo " + command;
}

ExecutionResult Executor::executeWindows(const std::string& command) {
#ifdef _WIN32
    ExecutionResult result;
    result.success = false;
    result.exitCode = -1;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hStdoutRead, hStdoutWrite;
    HANDLE hStderrRead, hStderrWrite;

    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0) ||
        !CreatePipe(&hStderrRead, &hStderrWrite, &sa, 0)) {
        result.stderr = "Failed to create pipes";
        return result;
    }

    SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStderrRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = hStderrWrite;
    si.hStdOutput = hStdoutWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    std::string cmdLine = "cmd.exe /C " + command;

    if (CreateProcessA(NULL, const_cast<char*>(cmdLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL,
                       &si, &pi)) {
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrWrite);

        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        result.exitCode = exitCode;
        result.success = (exitCode == 0);

        // Read stdout
        char buffer[4096];
        DWORD bytesRead;
        while (ReadFile(hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) &&
               bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result.stdout += buffer;
        }

        // Read stderr
        while (ReadFile(hStderrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) &&
               bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result.stderr += buffer;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        result.stderr = "Failed to create process";
    }

    CloseHandle(hStdoutRead);
    CloseHandle(hStderrRead);

    return result;
#else
    ExecutionResult result;
    result.success = false;
    result.stderr = "Windows execution not available on this platform";
    return result;
#endif
}

ExecutionResult Executor::executeUnix(const std::string& command) {
    ExecutionResult result;
    result.success = false;
    result.exitCode = -1;

    // Execute command and capture output
    std::string fullCommand = command + " 2>&1";
    FILE* pipe = popen(fullCommand.c_str(), "r");

    if (!pipe) {
        result.stderr = "Failed to execute command";
        return result;
    }

    result.stdout = captureOutput(pipe);
    result.exitCode = pclose(pipe);

    // pclose returns exit status in format that needs WEXITSTATUS macro
#ifndef _WIN32
    if (WIFEXITED(result.exitCode)) {
        result.exitCode = WEXITSTATUS(result.exitCode);
    }
#endif

    result.success = (result.exitCode == 0);

    return result;
}

std::string Executor::captureOutput(FILE* pipe) {
    std::array<char, 256> buffer;
    std::string result;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    return result;
}

}  // namespace unipm
