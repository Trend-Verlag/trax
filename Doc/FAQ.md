# Frequently Asked Questions (FAQ)

- **I don't see any trains!** Use NVIDIA PhysX Visual Debugger (PVD) to monitor what's going 
on in the simulation. Switch a test to PVD mode by using the VisualDebugger fixture.

- **How many tracks and rolling stock can be simulated? Are there any limits?** Regarding the 
tracks, there is no performance limit, since as long as they ain't moving tracks, they 
do not need any calculations. If it comes to rolling stock, you can use the test suite
to simulate a large number of vehicles and assess performance. (e.g. take a look at TestTrain.cpp/
testMultipleTrainsRunning: https://github.com/Trend-Verlag/trax/blob/main/Test/trax/rigid/trains/TestTrain.cpp#L368)
