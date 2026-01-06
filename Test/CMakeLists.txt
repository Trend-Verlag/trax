add_executable(ALL_TESTS
	"./Test.cpp"
	"./common/TestHelpers.cpp"
	"./common/TestInterval.cpp"
	"./common/TestNarrowCast.cpp"
    "./dim/TestDimDocu.cpp"
	"./dim/TestDimensionatedValues.cpp"
    "./spat/TestSpatial.cpp"
    "./spat/TestStream.cpp"
    "./trax/TestCoreDocu.cpp"
	"./trax/TestCurves.cpp"
    "./trax/TestLocation.cpp"
    "./trax/TestReservation.cpp"
    "./trax/TestSensors.cpp"
    "./trax/TestSignals.cpp"
    "./trax/TestSpatialDimensionatedValues.cpp"
    "./trax/TestTracks.cpp"
    "./trax/TestTracksNormalize.cpp"
    "./trax/TestVersion.cpp"
    "./trax/collections/TestTrackSystem.cpp"
    "./trax/support/TestSupportXML.cpp"
    "./trax/rigid/modules/TestReadWrite.cpp"
    "./trax/rigid/TestBody.cpp"
    "./trax/rigid/TestStaticTrack.cpp"
    "./trax/rigid/TestTractionForceCharacteristic.cpp"
    "./trax/rigid/trains/TestFleet.cpp"
    "./trax/rigid/trains/TestJacksNPlugs.cpp"
    "./trax/rigid/trains/TestRollingStock.cpp"
    "./trax/rigid/trains/TestTrain.cpp"
    "./trax/rigid/trains/TestWheelFrame.cpp"
)

target_link_libraries(ALL_TESTS
	PRIVATE
		test_settings
        trax_core_support
        trax_core_collections_support
        trax_rigid_support
        trax_rigid_physx
        trax_modules_support
        trax_trains_support
        trax_trains_collections
 )

set(TEST_DEBUG_ARGS "--log_level=test_suite --report_level=short --color_output=yes")
set_property(TARGET ALL_TESTS PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${TEST_DEBUG_ARGS}")

configure_boost_test_exe(ALL_TESTS)

include("${CMAKE_SOURCE_DIR}/CMake/AddPhysXDebuggerEnv.cmake")
add_physx_debugger_env(ALL_TESTS)
