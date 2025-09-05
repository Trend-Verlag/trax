add_executable(trax_core_tests
    "./Test.cpp"
    "./TestCurves.cpp"
    "./TestLocation.cpp"
    "./TestReservation.cpp"
    "./TestSensors.cpp"
    "./TestSpatialDimensionatedValues.cpp"
    "./TestTracks.cpp"
    "./TestTracksNormalize.cpp"
    "./TestTractionForceCharacteristic.cpp"
    "./TestVersion.cpp"
    "./support/TestSupportXML.cpp"
)

target_link_libraries(trax_core_tests 
    PRIVATE
        test_settings
        trax_core_support
)

configure_boost_test_exe(trax_core_tests)