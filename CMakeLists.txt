cmake_minimum_required(VERSION 3.26)

# Read version from Version.h (single source of truth)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/Code/trax/Version.h" VERSION_FILE_CONTENT)

string(REGEX MATCH "TRAX_VERSION_MAJOR = ([0-9]+)" _ "${VERSION_FILE_CONTENT}")
set(VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "TRAX_VERSION_MINOR = ([0-9]+)" _ "${VERSION_FILE_CONTENT}")
set(VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "TRAX_VERSION_PATCH = ([0-9]+)" _ "${VERSION_FILE_CONTENT}")
set(VERSION_PATCH ${CMAKE_MATCH_1})

project(Trax3 
    VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
    LANGUAGES CXX
)

message(STATUS "Trax version: ${PROJECT_VERSION} (from Code/trax/Version.h)")


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

set(BOOST_MIN_VERSION 1.85.0)

# ------------------------------------------------------------------
# Boost location
#
# BuildBoost.bat sets BOOST_ROOT to the Boost root directory.
# Prefer the environment variable, but also allow CMake -DBoost_ROOT=...
# ------------------------------------------------------------------

if(DEFINED ENV{BOOST_ROOT} AND NOT DEFINED Boost_ROOT)
    set(Boost_ROOT "$ENV{BOOST_ROOT}")
endif()

if(NOT Boost_ROOT)
    message(FATAL_ERROR
        "Boost_ROOT is not set.\n"
        "Please set BOOST_ROOT to the Boost installation directory."
    )
endif()

get_filename_component(Boost_ROOT "${Boost_ROOT}" ABSOLUTE)

message(STATUS "Boost root: ${Boost_ROOT}")

# ------------------------------------------------------------------
# Find the Boost compiler tag from the libraries actually built.
#
# BuildBoost.bat uses:
#
#     b2 toolset=msvc ...
#
# Therefore the actual MSVC version is determined by Boost.Build.
#
# Examples:
#
#     boost_unit_test_framework-vc143-mt-x64-1_91.lib
#     boost_unit_test_framework-vc145-mt-x64-1_91.lib
#
# We deliberately do NOT hard-code vc143/vc145/etc.
# ------------------------------------------------------------------

set(BOOST_STAGE_LIB_DIR "${Boost_ROOT}/stage/lib")

if(NOT EXISTS "${BOOST_STAGE_LIB_DIR}")
    message(FATAL_ERROR
        "Boost stage/lib directory was not found:\n"
        "  ${BOOST_STAGE_LIB_DIR}\n"
        "Please build Boost using BuildBoost.bat."
    )
endif()

file(GLOB BOOST_TEST_LIBRARIES
    "${BOOST_STAGE_LIB_DIR}/boost_unit_test_framework-vc*-mt-x64-*.lib"
)

if(NOT BOOST_TEST_LIBRARIES)
    message(FATAL_ERROR
        "Could not find Boost.Test libraries in:\n"
        "  ${BOOST_STAGE_LIB_DIR}\n"
        "Expected a file such as:\n"
        "  boost_unit_test_framework-vc145-mt-x64-1_91.lib"
    )
endif()

# Extract the MSVC compiler tag from the first matching library.
#
# Example:
#   boost_unit_test_framework-vc145-mt-x64-1_91.lib
#
# produces:
#   Boost_COMPILER = -vc145
#
foreach(BOOST_TEST_LIBRARY ${BOOST_TEST_LIBRARIES})

    get_filename_component(
        BOOST_TEST_LIBRARY_NAME
        "${BOOST_TEST_LIBRARY}"
        NAME
    )

    if(BOOST_TEST_LIBRARY_NAME MATCHES
       "^boost_unit_test_framework-(vc[0-9]+)-")
        set(Boost_COMPILER "-${CMAKE_MATCH_1}")
        break()
    endif()

endforeach()

if(NOT Boost_COMPILER)
    message(FATAL_ERROR
        "Could not determine the MSVC compiler tag from Boost.Test libraries."
    )
endif()

message(STATUS "Boost compiler tag: ${Boost_COMPILER}")

# ------------------------------------------------------------------
# Boost configuration
# ------------------------------------------------------------------

set(Boost_NO_WARN_NEW_VERSIONS ON)

# We have explicitly identified the Boost installation.
set(Boost_NO_SYSTEM_PATHS ON)

# Toggle shared vs static Boost.Test.
option(
    BOOST_TEST_SHARED
    "Link Boost.Test as shared (DLL) library"
    ON
)

if(BOOST_TEST_SHARED)
    set(Boost_USE_STATIC_LIBS OFF)
else()
    set(Boost_USE_STATIC_LIBS ON)
endif()

# ------------------------------------------------------------------
# Find Boost
# ------------------------------------------------------------------

cmake_policy(SET CMP0167 OLD)

find_package(
    Boost ${BOOST_MIN_VERSION}
    REQUIRED
    COMPONENTS unit_test_framework
)

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost library directory: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost.Test library: ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")

# ------------------------------------------------------------------
# Interface helper target all test executables can link
# ------------------------------------------------------------------

add_library(boost_test_support INTERFACE)

target_link_libraries(
    boost_test_support
    INTERFACE
        Boost::unit_test_framework
)

# ------------------------------------------------------------------
# Common compile definitions for all tests
# ------------------------------------------------------------------

if(BOOST_TEST_SHARED)

    target_compile_definitions(
        boost_test_support
        INTERFACE
            BOOST_TEST_DYN_LINK
            BOOST_TEST_NO_LIB
            WITH_BOOST_TESTS
    )

else()

    target_compile_definitions(
        boost_test_support
        INTERFACE
            BOOST_TEST_NO_LIB
            WITH_BOOST_TESTS
    )

endif()

# ------------------------------------------------------------------
# Helper function to configure a Boost.Test executable
# ------------------------------------------------------------------

function(configure_boost_test_exe target)

    target_link_libraries(
        ${target}
        PRIVATE
            boost_test_support
    )

    # Add Boost DLL path for MSVC F5 if using shared libs.
    if(MSVC AND BOOST_TEST_SHARED)

        set_target_properties(
            ${target}
            PROPERTIES
                VS_DEBUGGER_ENVIRONMENT
                "PATH=%PATH%;$<SHELL_PATH:${BOOST_STAGE_LIB_DIR}>"
        )

    endif()

endfunction()

# -------- End central Boost config --------


# -------- Doxygen Documentation --------
option(BUILD_DOCUMENTATION "Build documentation using Doxygen" ON)

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    
    if(DOXYGEN_FOUND)
        # Try to find Graphviz/DOT for better diagrams
        find_program(DOT_EXECUTABLE NAMES dot)
        
        if(DOT_EXECUTABLE)
            message(STATUS "Graphviz found: ${DOT_EXECUTABLE}")
            message(STATUS "  Documentation will use DOT for high-quality diagrams")
            set(DOXYGEN_HAVE_DOT "YES")
            set(DOXYGEN_CLASS_GRAPH "YES")
            get_filename_component(DOT_PATH "${DOT_EXECUTABLE}" DIRECTORY)
            set(DOXYGEN_DOT_PATH "${DOT_PATH}")
        else()
            message(STATUS "Graphviz not found - using built-in text diagrams")
            message(STATUS "  Install Graphviz for better diagrams: winget install graphviz")
            set(DOXYGEN_HAVE_DOT "NO")
            set(DOXYGEN_CLASS_GRAPH "BUILTIN")
            set(DOXYGEN_DOT_PATH "")
        endif()
        
        # Set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doc/TraxAPI/Trax_Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.generated)
        
        # Set the output directory for generated HTML
        set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Doc/TraxHtml)
        
        # Set paths for Doxygen to use
        set(DOXYGEN_IMAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Doc/TraxAPI)
        set(DOXYGEN_INPUT_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/Code ${CMAKE_CURRENT_SOURCE_DIR}/README.md")
        
        # Configure the Doxyfile
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        # Add custom target to build documentation
        add_custom_target(ALL_DOCUMENTATION
            # Create the output directory first
            COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
            # Then run Doxygen from the source directory (so relative paths work)
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
             # Open the documentation after building
            COMMAND ${CMAKE_COMMAND} -E echo "Opening documentation..."
            COMMAND start ${DOXYGEN_OUTPUT_DIR}/html/index.html
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
            SOURCES 
                ${DOXYGEN_IN}
                LICENSE
                README.md
                "${CMAKE_CURRENT_SOURCE_DIR}/Doc/EisenbahnNow.pdf"
                "${CMAKE_CURRENT_SOURCE_DIR}/Doc/TrackConstraint.pdf"
        )
        
        message(STATUS "Doxygen found - documentation target 'ALL_DOCUMENTATION' available")
        message(STATUS "Documentation will be generated in: ${DOXYGEN_OUTPUT_DIR}")
    else()
        message(WARNING "Doxygen not found - documentation will not be built")
    endif()
endif()
# -------- End Doxygen config --------


add_subdirectory(Code/appframe)
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
add_subdirectory(Code/trax/rigid/trains/collections/support)
add_subdirectory(Code/trax/rigid/modules)
add_subdirectory(Code/trax/rigid/modules/support)
add_subdirectory(Code/trax/support)

add_subdirectory(Test)
add_subdirectory(Test/appframe)
add_subdirectory(Test/common)
add_subdirectory(Test/dim)
add_subdirectory(Test/spat)
add_subdirectory(Test/trax)
add_subdirectory(Test/trax/rigid)
add_subdirectory(Test/trax/rigid/modules)
add_subdirectory(Test/trax/rigid/trains)

add_subdirectory(Apps)
