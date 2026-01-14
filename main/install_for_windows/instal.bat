@echo off
chcp 65001 >nul
title QWEE Engine Setup - Raylib & Dependencies Installer
color 0A

echo ================================================
echo      QWEE Engine v0.3 Setup Utility
echo ================================================
echo.

net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [WARNING] Running without administrator rights.
    echo Some operations may require elevation.
    echo.
)

if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set ARCH=64
    set ARCH_NAME=x64
) else (
    set ARCH=32
    set ARCH_NAME=x86
)

echo [INFO] System architecture: %ARCH%-bit (%ARCH_NAME%)
echo.

set VCPKG_ROOT=%CD%\vcpkg
set LIB_DIR=%CD%\lib
set INCLUDE_DIR=%CD%\include
set TEMP_DIR=%CD%\temp_install

:wait
timeout /t 3 /nobreak >nul
goto :eof

:check_program
where %1 >nul 2>nul
if %errorlevel% equ 0 (
    echo [OK] %1 found
    exit /b 0
) else (
    echo [MISSING] %1 not found
    exit /b 1
)
goto :eof

echo [1/6] Checking prerequisites...
echo.

call :check_program git
if %errorlevel% neq 0 (
    echo [INSTALL] Installing Git...

    if not exist "%TEMP_DIR%" mkdir "%TEMP_DIR%"
    cd "%TEMP_DIR%"

    echo Downloading Git installer...
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/git-for-windows/git/releases/download/v2.45.2.windows.1/Git-2.45.2-64-bit.exe' -OutFile 'git_install.exe' -UserAgent 'Mozilla/5.0'"
    
    if exist "git_install.exe" (
        echo Installing Git (this may take a few minutes)...
        echo Please follow the Git installer prompts if they appear.
        echo.

        start /wait git_install.exe /VERYSILENT /NORESTART /NOCANCEL /SP- /CLOSEAPPLICATIONS /NORESTARTAPPLICATIONS

        setx PATH "%PATH%;C:\Program Files\Git\cmd" >nul
        set PATH=%PATH%;C:\Program Files\Git\cmd

        call :wait
        where git >nul 2>nul
        if %errorlevel% equ 0 (
            echo [OK] Git installed successfully
        ) else (
            echo [ERROR] Git installation failed!
            echo Please install Git manually from: https://git-scm.com/
            pause
            exit /b 1
        )
    ) else (
        echo [ERROR] Failed to download Git installer
        echo Please install Git manually from: https://git-scm.com/
        pause
        exit /b 1
    )
    
    cd ..
) else (
    echo [OK] Git is already installed
)

echo.
echo [2/6] Checking and installing vcpkg...
echo.

where vcpkg >nul 2>nul
if errorlevel 1 (
    echo [INSTALL] vcpkg not found, installing...

    if exist "%VCPKG_ROOT%" (
        echo Removing old vcpkg installation...
        rmdir /s /q "%VCPKG_ROOT%" 2>nul
    )

    echo Cloning vcpkg repository...
    git clone https://github.com/Microsoft/vcpkg.git "%VCPKG_ROOT%"
    
    if exist "%VCPKG_ROOT%" (
        cd "%VCPKG_ROOT%"

        echo Bootstrapping vcpkg...
        call bootstrap-vcpkg.bat -disableMetrics
        
        if exist vcpkg.exe (
            setx PATH "%PATH%;%VCPKG_ROOT%" >nul
            set PATH=%PATH%;%VCPKG_ROOT%
            
            echo [OK] vcpkg installed successfully
        ) else (
            echo [ERROR] vcpkg bootstrap failed!
            cd ..
            pause
            exit /b 1
        )
        
        cd ..
    ) else (
        echo [ERROR] Failed to clone vcpkg repository!
        pause
        exit /b 1
    )
) else (
    echo [OK] vcpkg is already installed
    for /f "tokens=*" %%i in ('where vcpkg') do set VCPKG_ROOT=%%~dpi
    set VCPKG_ROOT=%VCPKG_ROOT:~0,-1%
)

echo.
echo [3/6] Installing Raylib via vcpkg...
echo.

echo Installing Raylib for %ARCH_NAME%-windows...
"%VCPKG_ROOT%\vcpkg.exe" install raylib:%ARCH_NAME%-windows --triplet %ARCH_NAME%-windows

if errorlevel 1 (
    echo [WARNING] Raylib installation via vcpkg failed, trying alternative...
    goto :direct_download
)

echo.
echo [4/6] Installing additional libraries for QWEE Engine...
echo.

echo Installing development libraries...
echo.

set ADDITIONAL_LIBS=glfw3 opengl32 gdi32 winmm

for %%l in (%ADDITIONAL_LIBS%) do (
    echo Installing %%l...
    "%VCPKG_ROOT%\vcpkg.exe" install %%l:x64-windows --triplet x64-windows
)

echo.
echo [5/6] Copying libraries and headers...
echo.

if not exist "%LIB_DIR%" mkdir "%LIB_DIR%"
if not exist "%INCLUDE_DIR%" mkdir "%INCLUDE_DIR%"

set VCPKG_INSTALLED=%VCPKG_ROOT%\installed\%ARCH_NAME%-windows

if exist "%VCPKG_INSTALLED%" (
    echo Copying libraries...
    xcopy "%VCPKG_INSTALLED%\lib\*.lib" "%LIB_DIR%\" /Y /I >nul 2>&1
    xcopy "%VCPKG_INSTALLED%\lib\*.a" "%LIB_DIR%\" /Y /I >nul 2>&1
    
    echo Copying DLL files...
    if exist "%VCPKG_INSTALLED%\bin\*.dll" (
        xcopy "%VCPKG_INSTALLED%\bin\*.dll" "%LIB_DIR%\" /Y /I >nul 2>&1
    )
    
    echo Copying headers...
    xcopy "%VCPKG_INSTALLED%\include\*.h" "%INCLUDE_DIR%\" /Y /I >nul 2>&1

    if not exist "%INCLUDE_DIR%\raylib.h" (
        if exist "%VCPKG_INSTALLED%\include\raylib\raylib.h" (
            xcopy "%VCPKG_INSTALLED%\include\raylib\*" "%INCLUDE_DIR%\" /Y /I >nul 2>&1
        )
    )
    
    goto :verify_installation
) else (
    echo [ERROR] vcpkg installation directory not found: %VCPKG_INSTALLED%
    goto :direct_download
)

:direct_download
echo.
echo [ALTERNATIVE] Direct download of Raylib...
echo.

set RAYLIB_VERSION=5.0
set COMPILER=msvc

if not exist "%TEMP_DIR%" mkdir "%TEMP_DIR%"
cd "%TEMP_DIR%"

echo Downloading Raylib %RAYLIB_VERSION%...
set DOWNLOAD_URL=https://github.com/raysan5/raylib/releases/download/%RAYLIB_VERSION%/raylib-%RAYLIB_VERSION%_win%ARCH%-%COMPILER%.zip

powershell -Command "Invoke-WebRequest -Uri '%DOWNLOAD_URL%' -OutFile 'raylib.zip' -UserAgent 'Mozilla/5.0'"

if exist "raylib.zip" (
    echo Extracting Raylib...
    powershell -Command "Expand-Archive -Path 'raylib.zip' -DestinationPath 'extracted' -Force"

    echo Copying files...

    for /r "extracted" %%f in (*.lib) do copy "%%f" "..\%LIB_DIR%\" >nul
    for /r "extracted" %%f in (*.a) do copy "%%f" "..\%LIB_DIR%\" >nul
 
    for /r "extracted" %%f in (*.dll) do copy "%%f" "..\%LIB_DIR%\" >nul

    for /r "extracted" %%f in (*.h) do copy "%%f" "..\%INCLUDE_DIR%\" >nul

    if not exist "..\%INCLUDE_DIR%\raylib.h" (
        if exist "extracted\raylib-%RAYLIB_VERSION%\src\raylib.h" (
            copy "extracted\raylib-%RAYLIB_VERSION%\src\*.h" "..\%INCLUDE_DIR%\" >nul
        )
    )
    
    cd ..
) else (
    echo [ERROR] Failed to download Raylib!
    echo Please download manually from: https://www.raylib.com/
    cd ..
    pause
    exit /b 1
)

:verify_installation
echo.
echo [6/6] Verifying installation...
echo.

set MISSING_FILES=0

echo Checking required files:
echo.

if exist "%INCLUDE_DIR%\raylib.h" (
    echo [OK] raylib.h
) else (
    echo [MISSING] raylib.h
    set MISSING_FILES=1
)

if exist "%LIB_DIR%\raylib.lib" (
    echo [OK] raylib.lib
) else (
    if exist "%LIB_DIR%\libraylib.a" (
        echo [OK] libraylib.a (MinGW version)
    ) else (
        echo [MISSING] raylib library file
        set MISSING_FILES=1
    )
)

if exist "%LIB_DIR%\glfw3.lib" (
    echo [OK] glfw3.lib
) else (
    if exist "%LIB_DIR%\libglfw3.a" (
        echo [OK] libglfw3.a
    ) else (
        echo [WARNING] glfw3 library not found (may be needed for some features)
    )
)

echo.
echo Checking OpenGL libraries:

if exist "%LIB_DIR%\opengl32.lib" (
    echo [OK] opengl32.lib
) else (
    echo [WARNING] opengl32.lib not found
)

if exist "%LIB_DIR%\gdi32.lib" (
    echo [OK] gdi32.lib
) else (
    echo [WARNING] gdi32.lib not found
)

if exist "%LIB_DIR%\winmm.lib" (
    echo [OK] winmm.lib
) else (
    echo [WARNING] winmm.lib not found
)

echo.
if %MISSING_FILES% equ 0 (
    echo ================================================
    echo [SUCCESS] Raylib and dependencies installed!
    echo ================================================
    echo.
    echo Installation summary:
    echo   Headers:    %INCLUDE_DIR%\
    echo   Libraries:  %LIB_DIR%\
    echo   vcpkg:      %VCPKG_ROOT%\
    echo.
    echo To build QWEE Engine, run:
    echo   make install_deps      (first time)
    echo   make all               (build everything)
    echo   make run_arena         (run example game)
    echo.
    echo Press any key to exit...
) else (
    echo ================================================
    echo [WARNING] Some files are missing!
    echo ================================================
    echo.
    echo Please check the errors above.
    echo You may need to manually download Raylib from:
    echo   https://www.raylib.com/
    echo.
    echo Copy raylib.h to: %INCLUDE_DIR%\
    echo Copy library files to: %LIB_DIR%\
    echo.
    echo Press any key to exit...
)

if exist "%TEMP_DIR%" (
    echo.
    echo Cleaning temporary files...
    rmdir /s /q "%TEMP_DIR%" 2>nul
)

if exist "vcpkg_install" rmdir /s /q "vcpkg_install" 2>nul
if exist "temp" rmdir /s /q "temp" 2>nul

pause >nul
exit /b 0