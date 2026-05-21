add_executable(appframe_tests
    AppFrameTest.cpp
    TestCommands.cpp
)

target_link_libraries(appframe_tests 
    PRIVATE 
        test_settings
        appframe
)

configure_boost_test_exe(appframe_tests)