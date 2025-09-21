echo This installs the trax library.

pushd %~dp0
set "TRAX3_ROOT=%cd%"
setx TRAX3_ROOT "%cd%"
cmake -S . -B .\_Build
cmake --build .\_Build
call .\Doc\TraxAPI\GenerateDocumentation.bat
popd
pause