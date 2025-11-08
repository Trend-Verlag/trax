echo This installs and builds the trax library.

pushd %~dp0
set "TRAX3_ROOT=%cd%"
setx TRAX3_ROOT "%cd%"
cmake -S . -B .\_Build
cmake --build .\_Build --target docs
cmake --build .\_Build
popd
pause