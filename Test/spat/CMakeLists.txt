add_executable(spat_tests
    SpatTest.cpp
    TestSpatial.cpp
    TestStream.cpp
)

target_link_libraries(spat_tests 
    PRIVATE 
        test_settings
        spat_support
)

configure_boost_test_exe(spat_tests)