add_executable(trax_rigid_tests
    "./Test.cpp"
    "./TestStaticTrack.cpp"
 )

target_link_libraries(trax_rigid_tests 
    PRIVATE
        test_settings
        trax_core_support
        trax_core_collections_support
        trax_rigid_physx
)

configure_boost_test_exe(trax_rigid_tests)

# Add PhysX DLL path for MSVC F5 if using shared libs
if(MSVC)
    # Compute bin dirs once with correct escaping for VS
    set(_physx_bin_debug   "$<SHELL_PATH:$ENV{PHYSX_ROOT}/bin/win.x86_64.vc143.md/debug>;$<SHELL_PATH:$ENV{BOOST_ROOT}/stage/lib>")
    set(_physx_bin_release "$<SHELL_PATH:$ENV{PHYSX_ROOT}/bin/win.x86_64.vc143.md/release>;$<SHELL_PATH:$ENV{BOOST_ROOT}/stage/lib>")

    # One property, config-specific via generator expressions
    set_property(TARGET trax_rigid_tests PROPERTY VS_DEBUGGER_ENVIRONMENT
        "$<$<CONFIG:Debug>:PATH=%PATH%;${_physx_bin_debug}>"
        "$<$<CONFIG:Release>:PATH=%PATH%;${_physx_bin_release}>"
        "$<$<CONFIG:RelWithDebInfo>:PATH=%PATH%;${_physx_bin_release}>"
        "$<$<CONFIG:MinSizeRel>:PATH=%PATH%;${_physx_bin_release}>"
    )
endif()