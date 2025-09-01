add_executable(tests_trax_core
    Test.cpp
    TestCurves.cpp
    TestLocation.cpp
    TestReservation.cpp
    TestSensors.cpp
    TestSpatialDimensionatedValues.cpp
    TestTracks.cpp
    TestTracksNormalize.cpp
    TestTractionForceCharacteristic.cpp
    TestVersion.cpp
)

target_link_libraries(tests_trax_core PRIVATE trax_core)

configure_boost_test_exe(tests_trax_core)