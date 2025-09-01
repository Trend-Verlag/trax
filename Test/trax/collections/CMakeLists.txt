add_executable(tests_trax_collections
    Test.cpp
    TestTrackSystem.cpp
)

target_link_libraries(tests_trax_collections PRIVATE trax_collections)

configure_boost_test_exe(tests_trax_collections)