# Build script for Windows using MinGW (PowerShell)

Write-Host "Building unipm with MinGW..." -ForegroundColor Cyan

# Check if MinGW is installed
$mingwCheck = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $mingwCheck) {
    Write-Host "Error: MinGW (gcc) not found in PATH!" -ForegroundColor Red
    Write-Host "" 
    Write-Host "Please install MinGW-w64:" -ForegroundColor Yellow
    Write-Host "  Option 1: choco install mingw"
    Write-Host "  Option 2: scoop install mingw"
    Write-Host "  Option 3: Download from https://www.mingw-w64.org/"
    Write-Host ""
    exit 1
}

# Create build directory
New-Item -ItemType Directory -Force -Path build | Out-Null
Set-Location build

# Run CMake with MinGW Makefiles generator
Write-Host "Configuring with CMake..." -ForegroundColor Cyan
cmake -G "MinGW Makefiles" ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build
Write-Host "Building..." -ForegroundColor Cyan
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Build complete! Binary location: build\bin\unipm.exe" -ForegroundColor Green
Write-Host ""
Write-Host "To install system-wide, copy the executable to a directory in your PATH"
