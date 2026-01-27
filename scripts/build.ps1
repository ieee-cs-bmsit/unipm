# Build script for Windows (PowerShell)

Write-Host "Building unipm..." -ForegroundColor Cyan

# Create build directory
New-Item -ItemType Directory -Force -Path build | Out-Null
Set-Location build

# Run CMake
cmake ..

# Build
cmake --build . --config Release

Write-Host ""
Write-Host "Build complete! Binary location: build\bin\Release\unipm.exe" -ForegroundColor Green
Write-Host ""
Write-Host "To install system-wide, copy the executable to a directory in your PATH"
