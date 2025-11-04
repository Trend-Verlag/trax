add_executable(trax_modules_tests
    "./Test.cpp"
    "./TestReadWrite.cpp"
)

target_link_libraries(trax_modules_tests
    PRIVATE
        test_settings
        trax_core_collections_support
        trax_modules_support
)

configure_boost_test_exe(trax_modules_tests)

