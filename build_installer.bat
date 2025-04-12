@echo off
setlocal enabledelayedexpansion

:: Get the installation directory from the first argument
set "INSTALL_DIR=%~1"
if "%INSTALL_DIR%"=="" (
    echo Error: Installation directory not provided
    exit /b 1
)

:: Convert backslashes to forward slashes for CMake
set "INSTALL_DIR_FORWARD=%INSTALL_DIR:\=/%"

:: Create a log file in the installation directory
echo Build Log > "%INSTALL_DIR%\build_log.txt"
echo Time: %date% %time% >> "%INSTALL_DIR%\build_log.txt"
echo Installation Directory: %INSTALL_DIR% >> "%INSTALL_DIR%\build_log.txt"
echo. >> "%INSTALL_DIR%\build_log.txt"

:: Verify required executables exist
if not exist "%INSTALL_DIR%\libs\cmake\bin\cmake.exe" (
    echo Error: CMake not found at %INSTALL_DIR%\libs\cmake\bin\cmake.exe >> "%INSTALL_DIR%\build_log.txt"
    exit /b 1
)

if not exist "%INSTALL_DIR%\libs\mingw64\bin\gcc.exe" (
    echo Error: GCC not found at %INSTALL_DIR%\libs\mingw64\bin\gcc.exe >> "%INSTALL_DIR%\build_log.txt"
    exit /b 1
)

if not exist "%INSTALL_DIR%\libs\mingw64\bin\g++.exe" (
    echo Error: G++ not found at %INSTALL_DIR%\libs\mingw64\bin\g++.exe >> "%INSTALL_DIR%\build_log.txt"
    exit /b 1
)

:: Create and enter build directory
cd /d "%INSTALL_DIR%"
echo Creating build directory... >> "%INSTALL_DIR%\build_log.txt"
if exist "build" (
    echo Removing existing build directory... >> "%INSTALL_DIR%\build_log.txt"
    rmdir /s /q "build"
)
mkdir build
cd build

:: Run CMake with full paths using forward slashes
echo Running CMake configuration... >> "%INSTALL_DIR%\build_log.txt"
"%INSTALL_DIR%\libs\cmake\bin\cmake.exe" -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_C_COMPILER="%INSTALL_DIR_FORWARD%/libs/mingw64/bin/gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%INSTALL_DIR_FORWARD%/libs/mingw64/bin/g++.exe" ^
    -DGLFW_BUILD_DOCS=OFF ^
    -DGLFW_BUILD_TESTS=OFF ^
    -DGLFW_BUILD_EXAMPLES=OFF ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    .. >> "%INSTALL_DIR%\build_log.txt" 2>&1

if errorlevel 1 (
    echo CMake configuration failed >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

:: Run make
echo Running build... >> "%INSTALL_DIR%\build_log.txt"
"%INSTALL_DIR%\libs\mingw64\bin\mingw32-make.exe" -j4 >> "%INSTALL_DIR%\build_log.txt" 2>&1

if errorlevel 1 (
    echo Build failed >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

:: Copy required MinGW DLLs to the build directory
echo Copying required DLLs to build directory... >> "%INSTALL_DIR%\build_log.txt"
echo Current directory: %CD% >> "%INSTALL_DIR%\build_log.txt"

:: Verify DLLs exist in source location
if not exist "%INSTALL_DIR%\libs\mingw64\bin\libgcc_s_seh-1.dll" (
    echo Error: libgcc_s_seh-1.dll not found in MinGW bin directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%INSTALL_DIR%\libs\mingw64\bin\libstdc++-6.dll" (
    echo Error: libstdc++-6.dll not found in MinGW bin directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%INSTALL_DIR%\libs\mingw64\bin\libwinpthread-1.dll" (
    echo Error: libwinpthread-1.dll not found in MinGW bin directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%INSTALL_DIR%\SDL3.dll" (
    echo Error: SDL3.dll not found in installation directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

:: Copy DLLs to build directory
echo Copying libgcc_s_seh-1.dll... >> "%INSTALL_DIR%\build_log.txt"
copy "%INSTALL_DIR%\libs\mingw64\bin\libgcc_s_seh-1.dll" "%CD%" >> "%INSTALL_DIR%\build_log.txt" 2>&1
if errorlevel 1 (
    echo Failed to copy libgcc_s_seh-1.dll >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

echo Copying libstdc++-6.dll... >> "%INSTALL_DIR%\build_log.txt"
copy "%INSTALL_DIR%\libs\mingw64\bin\libstdc++-6.dll" "%CD%" >> "%INSTALL_DIR%\build_log.txt" 2>&1
if errorlevel 1 (
    echo Failed to copy libstdc++-6.dll >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

echo Copying libwinpthread-1.dll... >> "%INSTALL_DIR%\build_log.txt"
copy "%INSTALL_DIR%\libs\mingw64\bin\libwinpthread-1.dll" "%CD%" >> "%INSTALL_DIR%\build_log.txt" 2>&1
if errorlevel 1 (
    echo Failed to copy libwinpthread-1.dll >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

echo Copying SDL3.dll... >> "%INSTALL_DIR%\build_log.txt"
copy "%INSTALL_DIR%\SDL3.dll" "%CD%" >> "%INSTALL_DIR%\build_log.txt" 2>&1
if errorlevel 1 (
    echo Failed to copy SDL3.dll >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

:: Verify DLLs were copied successfully
if not exist "%CD%\libgcc_s_seh-1.dll" (
    echo Error: Failed to copy libgcc_s_seh-1.dll to build directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%CD%\libstdc++-6.dll" (
    echo Error: Failed to copy libstdc++-6.dll to build directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%CD%\libwinpthread-1.dll" (
    echo Error: Failed to copy libwinpthread-1.dll to build directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)
if not exist "%CD%\SDL3.dll" (
    echo Error: Failed to copy SDL3.dll to build directory >> "%INSTALL_DIR%\build_log.txt"
    cd ..
    exit /b 1
)

echo All DLLs copied successfully to build directory >> "%INSTALL_DIR%\build_log.txt"
echo Build completed successfully! >> "%INSTALL_DIR%\build_log.txt"
cd ..
exit /b 0 