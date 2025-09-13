add_executable(trax_trains_tests
    "./Test.cpp"
    "./TestFleet.cpp"
    "./TestJacksNPlugs.cpp"
    "./TestRollingStock.cpp"
    "./TestTrain.cpp"
    "./TestWheelFrame.cpp"
)

target_link_libraries(trax_trains_tests
    PRIVATE
        test_settings
        trax_core_support
        trax_trains_support
        trax_trains_collections
        trax_trains_physx
)

configure_boost_test_exe(trax_trains_tests)

# Add PhysX DLL path for MSVC F5 if using shared libs
if(MSVC)
    # Compute bin dirs once with correct escaping for VS
    set(_physx_bin_debug   "$<SHELL_PATH:$ENV{PHYSX_ROOT}/bin/win.x86_64.vc143.md/debug>;$<SHELL_PATH:$ENV{BOOST_ROOT}/stage/lib>")
    set(_physx_bin_release "$<SHELL_PATH:$ENV{PHYSX_ROOT}/bin/win.x86_64.vc143.md/release>;$<SHELL_PATH:$ENV{BOOST_ROOT}/stage/lib>")

    # One property value; pick per-config without creating empty list elements
    set_property(TARGET trax_trains_tests PROPERTY VS_DEBUGGER_ENVIRONMENT
        "PATH=%PATH%;$<IF:$<CONFIG:Debug>,${_physx_bin_debug},${_physx_bin_release}>"
    )
endif()