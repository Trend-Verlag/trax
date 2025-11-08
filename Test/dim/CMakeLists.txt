add_executable(dim_tests
    DimTest.cpp
    TestDimensionatedValues.cpp
    TestDimDocu.cpp
)

target_link_libraries(dim_tests 
    PRIVATE 
        test_settings
        dim_support
)

configure_boost_test_exe(dim_tests)