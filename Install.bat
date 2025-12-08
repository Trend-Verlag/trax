@echo off
echo Checking for Java (required for PlantUML diagrams)...
java -version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo WARNING: Java not found. PlantUML diagrams will not be generated.
    echo Download Java from: https://adoptium.net/
) else (
    echo Java found - PlantUML diagrams will be generated
)

echo This installs and builds the trax library.

pushd %~dp0
set "TRAX3_ROOT=%cd%"
setx TRAX3_ROOT "%cd%"
cmake -S . -B .\_Build
cmake --build .\_Build --target ALL_DOCUMENTATION
cmake --build .\_Build --config Release
popd
pause