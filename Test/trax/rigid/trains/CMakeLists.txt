add_executable(trax_trains_tests
    "./Test.cpp"
    "./TestFleet.cpp"
    "./TestJacksNPlugs.cpp"
    "./TestRollingStock.cpp"
    "./TestTrain.cpp"
    "./TestWheelFrame.cpp"
)

target_link_libraries(trax_trains_tests
    PRIVATE
        test_settings
        trax_core_support
        trax_trains_support
        trax_trains_collections
        trax_trains_physx
)

configure_boost_test_exe(trax_trains_tests)

include("${CMAKE_SOURCE_DIR}/CMake/AddPhysXDebuggerEnv.cmake")
add_physx_debugger_env(trax_trains_tests)
