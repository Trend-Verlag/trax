echo This installs the trax library.

pushd %~dp0
setx TRAX_ROOT "%cd%"
cmake -S . -B .\_Build
cmake --build .\_Build
call .\Doc\TraxAPI\GenerateDocumentation.bat
popd
pause