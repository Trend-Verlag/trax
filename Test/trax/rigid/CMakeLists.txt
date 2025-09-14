add_executable(trax_rigid_tests
    "./Test.cpp"
    "./TestStaticTrack.cpp"
    "./TestTractionForceCharacteristic.cpp"
)

target_link_libraries(trax_rigid_tests 
    PRIVATE
        test_settings
        trax_core_support
        trax_core_collections_support
        trax_rigid_physx
)

configure_boost_test_exe(trax_rigid_tests)

include("${CMAKE_SOURCE_DIR}/CMake/AddPhysXDebuggerEnv.cmake")
add_physx_debugger_env(trax_rigid_tests)
