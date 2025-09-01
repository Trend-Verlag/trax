add_executable(tests_dim
    DimTest.cpp
    TestDimensionatedValues.cpp
)

target_link_libraries(tests_dim PRIVATE dim)

configure_boost_test_exe(tests_dim)