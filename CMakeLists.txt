cmake_minimum_required(VERSION 3.26)
project(Trax3 LANGUAGES CXX)

# Disallow in-source builds to keep the source tree clean.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported.\n"
    "Use: cmake -S . -B _Build (or choose an out-of-repository preset).")
endif()

# Global standard (header-only still benefits: consumers inherit the requirement)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------- Project-wide settings (all targets) --------
add_library(project_settings INTERFACE)
target_include_directories(project_settings 
    INTERFACE 
        "${CMAKE_CURRENT_SOURCE_DIR}/Code"

)

target_compile_features(project_settings 
    INTERFACE 
    cxx_std_17
)

# -------- test settings (all test targets) --------
add_library(test_settings INTERFACE)
target_include_directories(test_settings 
    INTERFACE 
        "${CMAKE_CURRENT_SOURCE_DIR}/Test"

)

target_compile_features(test_settings 
    INTERFACE 
    cxx_std_17
)

# -------- Central Boost / Boost.Test configuration --------
set(BOOST_MIN_VERSION 1.85)

# Suppress “new Boost version” advisory
set(Boost_NO_WARN_NEW_VERSIONS ON)

# Honor BOOST_ROOT if user/export set (optional strictness)
if(DEFINED ENV{BOOST_ROOT})
    set(Boost_NO_SYSTEM_PATHS ON)
endif()

# Toggle shared vs static Boost.Test (default: shared)
option(BOOST_TEST_SHARED "Link Boost.Test as shared (DLL) library" ON)

if(BOOST_TEST_SHARED)
    set(Boost_USE_STATIC_LIBS OFF)
else()
    set(Boost_USE_STATIC_LIBS ON)
endif()

# Find Boost once (add components here as project grows)
cmake_policy(SET CMP0167 OLD)
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS unit_test_framework)

# Interface helper target all test executables can link
add_library(boost_test_support INTERFACE)

target_link_libraries(boost_test_support INTERFACE Boost::unit_test_framework)

# Common compile defs for all tests
if(BOOST_TEST_SHARED)
    target_compile_definitions(boost_test_support INTERFACE
        BOOST_TEST_DYN_LINK
        BOOST_TEST_NO_LIB
        WITH_BOOST_TESTS
    )
else()
    target_compile_definitions(boost_test_support INTERFACE
        BOOST_TEST_NO_LIB
        WITH_BOOST_TESTS
    )
endif()

# Helper function to configure a Boost.Test executable
function(configure_boost_test_exe target)
    target_link_libraries(${target} PRIVATE boost_test_support)

    # Add Boost DLL path for MSVC F5 if using shared libs
    if(MSVC AND BOOST_TEST_SHARED AND DEFINED ENV{BOOST_ROOT})
        set_target_properties(${target} PROPERTIES
            VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;$<SHELL_PATH:$ENV{BOOST_ROOT}/stage/lib>"
        )
    endif()
endfunction()
# -------- End central Boost config --------



add_subdirectory(Code/common)
add_subdirectory(Code/common/support)
add_subdirectory(Code/dim)
add_subdirectory(Code/dim/support)
add_subdirectory(Code/spat)
add_subdirectory(Code/spat/support)
add_subdirectory(Code/trax)
add_subdirectory(Code/trax/collections)
add_subdirectory(Code/trax/collections/support)
add_subdirectory(Code/trax/rigid)
add_subdirectory(Code/trax/rigid/collections)
add_subdirectory(Code/trax/rigid/engines/PhysX)
add_subdirectory(Code/trax/rigid/support)
add_subdirectory(Code/trax/rigid/trains)
add_subdirectory(Code/trax/rigid/trains/collections)
add_subdirectory(Code/trax/rigid/trains/support)
add_subdirectory(Code/trax/rigid/modules)

add_subdirectory(Code/trax/support)

add_subdirectory(Test)
add_subdirectory(Test/common)
add_subdirectory(Test/dim)
add_subdirectory(Test/spat)
add_subdirectory(Test/trax)
add_subdirectory(Test/trax/rigid)
add_subdirectory(Test/trax/rigid/trains)
