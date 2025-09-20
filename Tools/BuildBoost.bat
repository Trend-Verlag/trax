echo Copy this file to the Boost root directory and execute it.

pushd %~dp0
setx BOOST_ROOT "%cd%"
call bootstrap
b2 toolset=msvc-14.3 link=static link=shared architecture=x86 address-model=64 with-test
b2 toolset=msvc-14.3 link=static link=shared architecture=x86 address-model=32 with-test
popd
pause