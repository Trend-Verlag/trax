echo This rebuilds the trax library.

cmake -S . -B .\_Build
call .\Doc\TraxAPI\GenerateDocumentation.bat
pause
