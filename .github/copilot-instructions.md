# Copilot Instructions

## Testing Guidelines
- The Trax3 project has a separate unit test project that uses Boost.Test. Tests for a component go in their own file named Test<Component>.cpp, for example TestHeightField.cpp.
- Never run the full test suite in Debug mode (it takes too long); run tests in Release and filter to the relevant suite, e.g. ALL_TESTS.exe --run_test=<Suite>.