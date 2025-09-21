@echo off
echo Copy this file to the Boost root directory and execute it.

pushd "%~dp0"

set "BOOST_ROOT=%CD%"
setx BOOST_ROOT "%CD%"

rem Bootstrap Boost.Build
call bootstrap.bat

rem Build only Boost.Test as a 64-bit DLL with the latest MSVC toolset and stage it to stage\lib
rem - toolset=msvc picks the newest installed MSVC (no hardcoded version)
rem - address-model=64 ensures x64
rem - link=shared builds DLL
rem - variant=release reduces build time/noise unless you specifically need debug
rem - stage copies artifacts to .\stage\lib
b2 -j%NUMBER_OF_PROCESSORS% ^
  toolset=msvc ^
  address-model=64 ^
  variant=release ^
  link=shared ^
  threading=multi ^
  --with-test ^
  stage

popd
pause