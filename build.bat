@echo off
setlocal enabledelayedexpansion

:: Create a log file
echo Build Log > build_log.txt
echo Time: %date% %time% >> build_log.txt
echo. >> build_log.txt

:: Delete build folder if it exists
if exist "build" (
    echo Cleaning build directory...
    rmdir /s /q "build"
)

:: Create build directory and enter it
echo Creating fresh build directory...
mkdir build
cd build

:: Configure with CMake
echo Configuring with CMake...
echo CMake Configuration Start >> ..\build_log.txt
cmake -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_C_COMPILER=gcc ^
    -DCMAKE_CXX_COMPILER=g++ ^
    -DGLFW_BUILD_DOCS=OFF ^
    -DGLFW_BUILD_TESTS=OFF ^
    -DGLFW_BUILD_EXAMPLES=OFF ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    .. 2>> ..\build_log.txt || (
    echo CMake configuration failed. Check build_log.txt for details.
    cd ..
    exit /b 1
)
echo CMake Configuration End >> ..\build_log.txt

:: Build the project
echo Building project...
echo Build Start >> ..\build_log.txt
mingw32-make -j4 2>> ..\build_log.txt || (
    echo Build failed. Check build_log.txt for details.
    cd ..
    exit /b 1
)
echo Build End >> ..\build_log.txt

echo.
echo Build completed successfully!
echo Executable location: %CD%\RayTracerNG.exe
cd ..