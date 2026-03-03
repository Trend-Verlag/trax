add_executable(trax_modules_tests
    "./Test.cpp"
    "./TestReadWrite.cpp"
)

target_link_libraries(trax_modules_tests
    PRIVATE
        test_settings
        trax_modules_support
        trax_rigid_physx
)

configure_boost_test_exe(trax_modules_tests)

include("${CMAKE_SOURCE_DIR}/CMake/AddPhysXDebuggerEnv.cmake")
add_physx_debugger_env(trax_modules_tests)