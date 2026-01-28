# unipm installer for Windows
# Download from: iwr -useb https://raw.githubusercontent.com/ieee-cs-bmsit/unipm/main/scripts/install.ps1 | iex

$ErrorActionPreference = "Stop"

# Configuration
$Repo = "ieee-cs-bmsit/unipm"
$InstallDir = if ($env:UNIPM_INSTALL_DIR) { $env:UNIPM_INSTALL_DIR } else { "$env:LOCALAPPDATA\unipm" }
$Version = $env:UNIPM_VERSION
$BinaryName = "unipm.exe"

# Functions
function Write-Info {
    param([string]$Message)
    Write-Host "ℹ $Message" -ForegroundColor Blue
}

function Write-Success {
    param([string]$Message)
    Write-Host "✓ $Message" -ForegroundColor Green
}

function Write-Error-Message {
    param([string]$Message)
    Write-Host "✗ $Message" -ForegroundColor Red
}

function Write-Warning-Message {
    param([string]$Message)
    Write-Host "⚠ $Message" -ForegroundColor Yellow
}

function Detect-Architecture {
    Write-Info "Detecting architecture..."
    
    $Arch = if ([Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }
    
    if ($Arch -eq "x86") {
        Write-Error-Message "32-bit Windows is not supported"
        Write-Info "unipm requires 64-bit Windows"
        exit 1
    }
    
    Write-Info "Architecture: $Arch"
    return $Arch
}

function Get-LatestVersion {
    if (-not $Version) {
        Write-Info "Fetching latest version..."
        
        try {
            $LatestRelease = Invoke-RestMethod -Uri "https://api.github.com/repos/$Repo/releases/latest"
            $Script:Version = $LatestRelease.tag_name -replace '^v', ''
            Write-Success "Latest version: v$Version"
        }
        catch {
            Write-Error-Message "Failed to fetch latest version from GitHub"
            Write-Info "You can manually specify a version: `$env:UNIPM_VERSION='1.0.0'; iwr ... | iex"
            exit 1
        }
    }
}

function Download-And-Install {
    param([string]$Arch)
    
    Write-Info "Preparing installation to $InstallDir..."
    
    # Create installation directory
    New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
    
    # Construct download URL
    $BinaryFile = "unipm-windows-$Arch.exe"
    $DownloadUrl = "https://github.com/$Repo/releases/download/v$Version/$BinaryFile"
    $ChecksumUrl = "https://github.com/$Repo/releases/download/v$Version/$BinaryFile.sha256"
    $DestPath = Join-Path $InstallDir $BinaryName
    $TempPath = Join-Path $env:TEMP "unipm-download.exe"
    
    Write-Info "Downloading from $DownloadUrl..."
    
    try {
        Invoke-WebRequest -Uri $DownloadUrl -OutFile $TempPath -UseBasicParsing
        Write-Success "Downloaded successfully"
    }
    catch {
        Write-Error-Message "Failed to download unipm binary"
        Write-Info "Check if version v$Version exists at https://github.com/$Repo/releases"
        if (Test-Path $TempPath) { Remove-Item $TempPath -Force }
        exit 1
    }
    
    # Download and verify checksum if available
    try {
        $ChecksumTempPath = "$TempPath.sha256"
        Invoke-WebRequest -Uri $ChecksumUrl -OutFile $ChecksumTempPath -UseBasicParsing
        Write-Info "Verifying checksum..."
        
        $ExpectedChecksum = (Get-Content $ChecksumTempPath).Split()[0]
        $ActualChecksum = (Get-FileHash -Path $TempPath -Algorithm SHA256).Hash.ToLower()
        
        if ($ExpectedChecksum -eq $ActualChecksum) {
            Write-Success "Checksum verified"
        }
        else {
            Write-Error-Message "Checksum verification failed!"
            Write-Error-Message "Expected: $ExpectedChecksum"
            Write-Error-Message "Actual: $ActualChecksum"
            Remove-Item $TempPath -Force
            Remove-Item $ChecksumTempPath -Force
            exit 1
        }
        
        Remove-Item $ChecksumTempPath -Force
    }
    catch {
        Write-Warning-Message "Checksum not available, skipping verification"
    }
    
    # Move binary to installation directory
    Move-Item -Path $TempPath -Destination $DestPath -Force
    Write-Success "Installed to $DestPath"
}

function Update-Path {
    Write-Info "Configuring PATH..."
    
    # Get current user PATH
    $UserPath = [Environment]::GetEnvironmentVariable("Path", "User")
    
    # Check if already in PATH
    if ($UserPath -like "*$InstallDir*") {
        Write-Success "PATH already configured"
        return
    }
    
    # Add to PATH
    $NewPath = "$UserPath;$InstallDir"
    [Environment]::SetEnvironmentVariable("Path", $NewPath, "User")
    
    # Update current session PATH
    $env:Path = "$env:Path;$InstallDir"
    
    Write-Success "Updated user PATH"
    Write-Info "PATH will be available in new terminal sessions"
}

function Verify-Installation {
    Write-Info "Verifying installation..."
    
    $BinaryPath = Join-Path $InstallDir $BinaryName
    
    if (-not (Test-Path $BinaryPath)) {
        Write-Error-Message "Installation verification failed"
        Write-Info "Binary not found at $BinaryPath"
        exit 1
    }
    
    try {
        $VersionOutput = & $BinaryPath --version 2>&1 | Select-Object -First 1
        Write-Success "Installation verified: $VersionOutput"
    }
    catch {
        Write-Error-Message "Failed to run unipm"
        Write-Info "Binary exists but cannot execute. Check permissions."
        exit 1
    }
}

function Print-Success {
    Write-Host ""
    Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Green
    Write-Host "  ✓ unipm installed successfully!" -ForegroundColor Green
    Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Green
    Write-Host ""
    Write-Host "📦 Version: " -NoNewline
    Write-Host "v$Version" -ForegroundColor Blue
    Write-Host "📁 Location: " -NoNewline
    Write-Host "$InstallDir\$BinaryName" -ForegroundColor Blue
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Yellow
    Write-Host "  1. Restart your terminal (PowerShell, CMD, or Windows Terminal)"
    Write-Host ""
    Write-Host "  2. Verify installation:"
    Write-Host "     unipm --version"
    Write-Host ""
    Write-Host "  3. Get started:"
    Write-Host "     unipm install docker"
    Write-Host "     unipm help"
    Write-Host ""
    Write-Host "📚 Documentation: " -NoNewline
    Write-Host "https://github.com/$Repo" -ForegroundColor Blue
    Write-Host "💬 Issues: " -NoNewline
    Write-Host "https://github.com/$Repo/issues" -ForegroundColor Blue
    Write-Host ""
}

# Main installation flow
function Main {
    Write-Host ""
    Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    Write-Host "  unipm installer"
    Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    Write-Host ""
    
    try {
        $Arch = Detect-Architecture
        Get-LatestVersion
        Download-And-Install -Arch $Arch
        Update-Path
        Verify-Installation
        Print-Success
    }
    catch {
        Write-Host ""
        Write-Error-Message "Installation failed: $_"
        Write-Info "For help, visit: https://github.com/$Repo/issues"
        exit 1
    }
}

# Run main function
Main
