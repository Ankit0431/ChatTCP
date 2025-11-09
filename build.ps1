# Build script for TCP Chat Server
# Usage: .\build.ps1

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Building TCP Chat Server" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Check if g++ is available
Write-Host "`nChecking for g++ compiler..." -ForegroundColor Yellow
$gppExists = $null -ne (Get-Command g++ -ErrorAction SilentlyContinue)

if ($gppExists) {
    Write-Host "✓ g++ found" -ForegroundColor Green
    Write-Host "`nBuilding with g++..." -ForegroundColor Green
    
    Write-Host "Compiling server..." -ForegroundColor Yellow
    g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -o ChatServer.exe main.cpp ChatServer.cpp Client.cpp ChatListener.cpp BroadcastClient.cpp DMClient.cpp -lws2_32
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Server build successful!" -ForegroundColor Green
        
        Write-Host "`nBuilding test client..." -ForegroundColor Yellow
        g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ -o ChatClient.exe ChatClient.cpp -lws2_32
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ Test client built successfully!" -ForegroundColor Green
        }
        else {
            Write-Host "✗ Test client build failed!" -ForegroundColor Red
        }
    }
    else {
        Write-Host "`n✗ Server build failed!" -ForegroundColor Red
        exit 1
    }
}
else {
    # Try Visual Studio compiler
    Write-Host "g++ not found, trying Visual Studio compiler (cl)..." -ForegroundColor Yellow
    $clExists = $null -ne (Get-Command cl -ErrorAction SilentlyContinue)
    
    if ($clExists) {
        Write-Host "✓ cl found" -ForegroundColor Green
        
        Write-Host "`nCompiling server..." -ForegroundColor Yellow
        cl /EHsc /std:c++17 /O2 /Fe:ChatServer.exe main.cpp MessageServer.cpp Connect.cpp ChatListener.cpp BroadcastMessage.cpp DM.cpp ws2_32.lib /nologo
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✓ Server build successful!" -ForegroundColor Green
            
            Write-Host "`nBuilding test client..." -ForegroundColor Yellow
            cl /EHsc /std:c++17 /O2 /Fe:ChatClient.exe ChatClient.cpp ws2_32.lib /nologo
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host "✓ Test client built successfully!" -ForegroundColor Green
            }
        }
        else {
            Write-Host "`n✗ Build failed!" -ForegroundColor Red
            exit 1
        }
    }
    else {
        Write-Host "`n✗ No C++ compiler found!" -ForegroundColor Red
        Write-Host "`nPlease install one of the following:" -ForegroundColor Yellow
        Write-Host "  1. MinGW (includes g++): https://sourceforge.net/projects/mingw/" -ForegroundColor White
        Write-Host "  2. Visual Studio Build Tools (includes cl)" -ForegroundColor White
        exit 1
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "   Build Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "`nTo run the server:" -ForegroundColor Yellow
Write-Host "  .\ChatServer.exe" -ForegroundColor White
Write-Host "  .\ChatServer.exe 5000        (custom port)" -ForegroundColor White
Write-Host "  .\ChatServer.exe 5000 120    (custom port & timeout)" -ForegroundColor White
Write-Host "`nTo run the test client:" -ForegroundColor Yellow
Write-Host "  .\ChatClient.exe" -ForegroundColor White
Write-Host "  .\ChatClient.exe localhost 4000" -ForegroundColor White
