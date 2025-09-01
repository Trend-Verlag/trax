add_executable(tests_common
    Test.cpp
    TestHelpers.cpp
    TestInterval.cpp
    TestNarrowCast.cpp
)

target_link_libraries(tests_common PRIVATE common)

configure_boost_test_exe(tests_common)

