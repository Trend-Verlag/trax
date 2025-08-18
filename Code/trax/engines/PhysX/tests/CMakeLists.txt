add_executable(tests_trax_core_physx
    TestBody.cpp
    TestStaticTrack.cpp
)

target_link_libraries(tests_trax_core_physx PRIVATE trax_core_physx)

configure_boost_test_exe(tests_trax_core_physx)