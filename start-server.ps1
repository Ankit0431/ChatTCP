Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   TCP Chat Server - Quick Start" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Build the project
Write-Host "`nStep 1: Building the project..." -ForegroundColor Green
.\build.ps1

if ($LASTEXITCODE -ne 0) {
    Write-Host "`nBuild failed. Please check the errors above." -ForegroundColor Red
    exit 1
}

# Run the server
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "   Starting Server on Port 4000" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "`nPress Ctrl+C to stop the server`n" -ForegroundColor Yellow

if (Test-Path "build\bin\Release\ChatServer.exe") {
    .\build\bin\Release\ChatServer.exe
}
elseif (Test-Path "build\bin\Debug\ChatServer.exe") {
    .\build\bin\Debug\ChatServer.exe
}
elseif (Test-Path "ChatServer.exe") {
    .\ChatServer.exe
}
else {
    Write-Host "Error: Could not find ChatServer.exe" -ForegroundColor Red
    exit 1
}
