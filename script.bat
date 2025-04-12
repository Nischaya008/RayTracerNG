@echo off
setlocal enabledelayedexpansion

:: Define versions and download URLs
set "GLFW_VERSION=3.3.8"
set "GLFW_URL=https://github.com/glfw/glfw/archive/refs/tags/%GLFW_VERSION%.zip"
set "GLFW_SHA256=8106e1a432305a8780b986c24922380df6a009a96b2ca590392cb0859062c8ff"

set "GLM_VERSION=0.9.9.8"
set "GLM_URL=https://github.com/g-truc/glm/archive/refs/tags/%GLM_VERSION%.zip"
set "GLM_SHA256=4605259c22feadf35388c027f07b345ad3aa3b12631a5a316347f7566c6f1839"

set "GLAD_VERSION=0.1.36"
set "GLAD_URL=https://github.com/Dav1dde/glad/archive/refs/tags/v%GLAD_VERSION%.zip"
set "GLAD_SHA256=3b407840e2c3ab7896486fe7d2f0324f83f01f943b1b723e79ae4e775e63322b"

set "IMGUI_VERSION=docking"
set "IMGUI_URL=https://github.com/ocornut/imgui/archive/refs/heads/docking.zip"
set "IMGUI_SHA256=skip"

set "CMAKE_VERSION=3.28.1"
set "CMAKE_URL=https://github.com/Kitware/CMake/releases/download/v%CMAKE_VERSION%/cmake-%CMAKE_VERSION%-windows-x86_64.zip"
set "CMAKE_SHA256=857ea19b34d3c89c0c43b6e1f4e590f9392be75b3d7b28b4315f864ef981da52"

set "MINGW_VERSION=13.2.0"
set "MINGW_URL=https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0-16.0.6-11.0.0-msvcrt-r2/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64msvcrt-11.0.0-r2.7z"
set "MINGW_SHA256=e01309a645fec3c7fd2bb12a5fa4e5ee9fe4eff51ee5de3e5f5e0492f6270531"

set "PYTHON_VERSION=3.11.7"
set "PYTHON_URL=https://www.python.org/ftp/python/%PYTHON_VERSION%/python-%PYTHON_VERSION%-embed-amd64.zip"
set "PYTHON_SHA256=c7d0e7e8f71b8f06f53afa298e8f3f6e2c0f0ad2f9c7d8f9d1a7dd8c090b4d8"

:: Set directories
set "BASE_DIR=%~dp0"
set "LIBS_DIR=%BASE_DIR%libs"
set "DOWNLOADS_DIR=%BASE_DIR%downloads"

:: Main script execution starts here
goto :main

:: Function declarations
:extract_file
if "%~1"=="" (
    echo ERROR: No zip file specified for extraction
    exit /b 1
)
if "%~2"=="" (
    echo ERROR: No destination directory specified for extraction
    exit /b 1
)
set "zip_file=%~1"
set "dest_dir=%~2"
echo Extracting "%zip_file%" to "%dest_dir%"...
if not exist "%zip_file%" (
    echo ERROR: Zip file does not exist: "%zip_file%"
    exit /b 1
)
powershell -Command "try { Expand-Archive -LiteralPath \"%zip_file%\" -DestinationPath \"%dest_dir%\" -Force -ErrorAction Stop } catch { Write-Error $_; exit 1 }"
if %errorlevel% neq 0 (
    echo Failed to extract %zip_file%
    exit /b 1
)
exit /b 0

:download_file
if "%~1"=="" (
    echo ERROR: No URL specified for download
    exit /b 1
)
if "%~2"=="" (
    echo ERROR: No output file specified for download
    exit /b 1
)
set "url=%~1"
set "output=%~2"
if exist "%output%" (
    echo File "%output%" already exists. Skipping download.
) else (
    echo Downloading "%url%" to "%output%"...
    powershell -Command "try { $ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri '%url%' -OutFile \"%output%\" -ErrorAction Stop } catch { Write-Error $_; exit 1 }"
    if %errorlevel% neq 0 (
        echo Failed to download from %url%
        exit /b 1
    )
)
exit /b 0

:verify_checksum
if "%~1"=="" (
    echo ERROR: No file specified for checksum verification
    exit /b 1
)
if "%~2"=="" (
    echo ERROR: No expected hash specified for checksum verification
    exit /b 1
)
set "file=%~1"
set "expected_hash=%~2"

if "%expected_hash%"=="skip" (
    echo Skipping checksum verification for %file%
    exit /b 0
)

if not exist "%file%" (
    echo ERROR: File does not exist: "%file%"
    exit /b 1
)
for /f "usebackq" %%A in (`powershell -Command "Get-FileHash -LiteralPath \"%file%\" -Algorithm SHA256 -ErrorAction Stop | ForEach-Object { $_.Hash }"`) do (
    set "actual_hash=%%A"
)
if /i "!actual_hash!" neq "%expected_hash%" (
    echo ERROR: Checksum verification failed for %file%.
    echo Expected: %expected_hash%
    echo Actual:   !actual_hash!
    exit /b 1
) else (
    echo Checksum verification passed for %file%.
)
exit /b 0

:main

:main
echo Starting project setup...

:: Check if PowerShell is available
where powershell >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: PowerShell is required but not found.
    exit /b 1
)

:: Create and verify directories
echo Creating project directories...
if not exist "%BASE_DIR%" (
    echo ERROR: Base directory does not exist: "%BASE_DIR%"
    exit /b 1
)

echo Creating libs directory...
if not exist "%LIBS_DIR%" (
    mkdir "%LIBS_DIR%" || (
        echo ERROR: Failed to create libs directory: "%LIBS_DIR%"
        exit /b 1
    )
)

echo Creating downloads directory...
if not exist "%DOWNLOADS_DIR%" (
    mkdir "%DOWNLOADS_DIR%" || (
        echo ERROR: Failed to create downloads directory: "%DOWNLOADS_DIR%"
        exit /b 1
    )
)

echo Directories created successfully.

echo.
echo Downloading and setting up dependencies...

echo.
echo Setting up GLFW...
set "GLFW_ARCHIVE=%DOWNLOADS_DIR%\glfw-%GLFW_VERSION%.zip"
call :download_file "%GLFW_URL%" "%GLFW_ARCHIVE%" || exit /b 1
call :verify_checksum "%GLFW_ARCHIVE%" "%GLFW_SHA256%" || exit /b 1
call :extract_file "%GLFW_ARCHIVE%" "%LIBS_DIR%\glfw" || exit /b 1
echo GLFW setup complete.

echo.
echo Setting up GLM...
set "GLM_ARCHIVE=%DOWNLOADS_DIR%\glm-%GLM_VERSION%.zip"
call :download_file "%GLM_URL%" "%GLM_ARCHIVE%" || exit /b 1
call :verify_checksum "%GLM_ARCHIVE%" "%GLM_SHA256%" || exit /b 1
call :extract_file "%GLM_ARCHIVE%" "%LIBS_DIR%\glm" || exit /b 1
echo GLM setup complete.

echo.
echo Setting up Dear ImGui...
set "IMGUI_ARCHIVE=%DOWNLOADS_DIR%\imgui-%IMGUI_VERSION%.zip"
call :download_file "%IMGUI_URL%" "%IMGUI_ARCHIVE%" || exit /b 1
call :verify_checksum "%IMGUI_ARCHIVE%" "%IMGUI_SHA256%" || exit /b 1
call :extract_file "%IMGUI_ARCHIVE%" "%LIBS_DIR%\imgui-temp" || exit /b 1
if exist "%LIBS_DIR%\imgui\imgui-docking" rmdir /s /q "%LIBS_DIR%\imgui\imgui-docking"
if not exist "%LIBS_DIR%\imgui" mkdir "%LIBS_DIR%\imgui"
move "%LIBS_DIR%\imgui-temp\imgui-docking" "%LIBS_DIR%\imgui\imgui-docking" || exit /b 1
rmdir /s /q "%LIBS_DIR%\imgui-temp"
echo Dear ImGui setup complete.

=======

echo.
echo Setting up build tools...

:: Check/Setup MinGW
echo Checking for MinGW...
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo MinGW not found. Downloading and setting up...
    set "MINGW_ARCHIVE=%DOWNLOADS_DIR%\mingw-%MINGW_VERSION%.7z"
    call :download_file "%MINGW_URL%" "%MINGW_ARCHIVE%" || exit /b 1
    call :verify_checksum "%MINGW_ARCHIVE%" "%MINGW_SHA256%" || exit /b 1
    
    echo Extracting MinGW...
    if not exist "%LIBS_DIR%\mingw64" mkdir "%LIBS_DIR%\mingw64"
    powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://7-zip.org/a/7zr.exe' -OutFile '%DOWNLOADS_DIR%\7zr.exe'" || exit /b 1
    "%DOWNLOADS_DIR%\7zr.exe" x "%MINGW_ARCHIVE%" -o"%LIBS_DIR%" -y || exit /b 1
    set "PATH=%LIBS_DIR%\mingw64\bin;%PATH%"
    
    :: Verify both C and C++ compilers
    echo Verifying MinGW C compiler...
    where gcc >nul 2>nul || (
        echo ERROR: MinGW GCC not found
        exit /b 1
    )
    echo Verifying MinGW C++ compiler...
    where g++ >nul 2>nul || (
        echo ERROR: MinGW G++ not found
        exit /b 1
    )
    
    echo Verifying MinGW installation...
    where gcc >nul 2>nul
    if %errorlevel% neq 0 (
        echo ERROR: MinGW installation failed. Build cannot proceed.
        exit /b 1
    )
    echo MinGW setup complete.
) else (
    echo MinGW is already installed.
)

:: Check/Setup CMake
echo Checking for CMake...
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo CMake not found. Downloading and setting up...
    set "CMAKE_ARCHIVE=%DOWNLOADS_DIR%\cmake-%CMAKE_VERSION%.zip"
    call :download_file "%CMAKE_URL%" "%CMAKE_ARCHIVE%" || exit /b 1
    call :verify_checksum "%CMAKE_ARCHIVE%" "%CMAKE_SHA256%" || exit /b 1
    call :extract_file "%CMAKE_ARCHIVE%" "%LIBS_DIR%\cmake" || exit /b 1
    set "PATH=%LIBS_DIR%\cmake\bin;%PATH%"
    
    echo Verifying CMake installation...
    where cmake >nul 2>nul
    if %errorlevel% neq 0 (
        echo ERROR: CMake installation failed. Build cannot proceed.
        exit /b 1
    )
    echo CMake setup complete.
) else (
    echo CMake is already installed.
)

:: Check/Setup Python
echo.
echo Checking for Python...
where python >nul 2>nul
if %errorlevel% neq 0 (
    echo Python not found. Downloading and setting up...
    set "PYTHON_ARCHIVE=%DOWNLOADS_DIR%\python-%PYTHON_VERSION%.zip"
    call :download_file "%PYTHON_URL%" "%PYTHON_ARCHIVE%" || exit /b 1
    call :verify_checksum "%PYTHON_ARCHIVE%" "%PYTHON_SHA256%" || exit /b 1
    
    echo Extracting Python...
    if not exist "%LIBS_DIR%\python" mkdir "%LIBS_DIR%\python"
    call :extract_file "%PYTHON_ARCHIVE%" "%LIBS_DIR%\python" || exit /b 1
    set "PATH=%LIBS_DIR%\python;%PATH%"
    
    echo Installing pip...
    powershell -Command "Invoke-WebRequest -Uri 'https://bootstrap.pypa.io/get-pip.py' -OutFile '%DOWNLOADS_DIR%\get-pip.py'"
    python "%DOWNLOADS_DIR%\get-pip.py" --no-warn-script-location || exit /b 1
    
    echo Installing GLAD...
    python -m pip install glad || exit /b 1
    
    echo Python setup complete.
) else (
    echo Python is already installed.
    where pip >nul 2>nul
    if %errorlevel% neq 0 (
        echo Installing pip...
        powershell -Command "Invoke-WebRequest -Uri 'https://bootstrap.pypa.io/get-pip.py' -OutFile '%DOWNLOADS_DIR%\get-pip.py'"
        python "%DOWNLOADS_DIR%\get-pip.py" --no-warn-script-location || exit /b 1
    )
    
    echo Setting up GLAD...
    
    :: Install GLAD via pip
    echo Installing GLAD via pip...
    python -m pip uninstall -y glad glad2 >nul 2>&1
    python -m pip install glad==0.1.36 || (
        echo ERROR: Failed to install GLAD
        exit /b 1
    )

    :: Set up GLAD output directory
    set "GLAD_OUTPUT_DIR=!LIBS_DIR!\glad-gen"
    echo Generating GLAD files in "!GLAD_OUTPUT_DIR!"...
    if exist "!GLAD_OUTPUT_DIR!" rmdir /s /q "!GLAD_OUTPUT_DIR!"

    :: Generate GLAD files
    echo Generating OpenGL bindings...
    python -m glad --generator=c --spec=gl --out-path="!GLAD_OUTPUT_DIR!" --api=gl=4.5 --profile=core --extensions=GL_KHR_debug --reproducible || (
        echo ERROR: Failed to generate GLAD files
        exit /b 1
    )

    :: Fix header include in glad.c using proper path
    powershell -Command "$gladPath = '!GLAD_OUTPUT_DIR!\src\glad.c'; if (Test-Path $gladPath) { (Get-Content $gladPath) -replace '#include <glad/glad.h>', '#include \"glad/glad.h\"' | Set-Content $gladPath }"

    :: Verify generated files
    if not exist "!GLAD_OUTPUT_DIR!\src\glad.c" (
        echo ERROR: glad.c not generated in !GLAD_OUTPUT_DIR!\src\
        exit /b 1
    )
    if not exist "!GLAD_OUTPUT_DIR!\include\glad\glad.h" (
        echo ERROR: glad.h not generated in !GLAD_OUTPUT_DIR!\include\glad\
        exit /b 1
    )
    if not exist "!GLAD_OUTPUT_DIR!\include\KHR\khrplatform.h" (
        echo ERROR: khrplatform.h not generated in !GLAD_OUTPUT_DIR!\include\KHR\
        exit /b 1
    )

    echo GLAD setup complete!
)

echo.
echo Configuring build...
if exist "build" rmdir /s /q "build"
mkdir build
cd build

echo Running CMake configuration...
cmake -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_C_COMPILER=gcc ^
    -DCMAKE_CXX_COMPILER=g++ ^
    -DGLFW_BUILD_DOCS=OFF ^
    -DGLFW_BUILD_TESTS=OFF ^
    -DGLFW_BUILD_EXAMPLES=OFF ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    .. || (
    echo CMake configuration failed.
    cd ..
    exit /b 1
)

echo.
echo Building project...
mingw32-make -j4 || (
    echo Build failed.
    cd ..
    exit /b 1
)

echo.
echo Project built successfully!
echo Executable location: %CD%\RayTracerNG.exe
cd ..
exit /b 0

goto :eof

