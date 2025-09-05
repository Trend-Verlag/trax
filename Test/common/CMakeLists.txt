add_executable(common_tests
    Test.cpp
    TestHelpers.cpp
    TestInterval.cpp
    TestNarrowCast.cpp
)

target_link_libraries(common_tests 
    PRIVATE 
        test_settings
        common_support
)

configure_boost_test_exe(common_tests)

