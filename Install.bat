echo This installs the trax library.

pushd %~dp0
setx TRAX_ROOT "%cd%"
call .\Build\Build.bat
call .\Doc\TraxAPI\GenerateDocumentation.bat
popd
pause