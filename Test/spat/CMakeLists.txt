add_executable(tests_spat
    SpatTest.cpp
    TestSpatial.cpp
    TestStream.cpp
)

target_link_libraries(tests_spat PRIVATE spat)

configure_boost_test_exe(tests_spat)