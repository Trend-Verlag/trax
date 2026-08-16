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

# -------- Central Boost / Boost.Test configuration (header-only) --------

# ------------------------------------------------------------------
# Boost location
#
# Header-only Boost.Test: we only need the Boost *headers*, no built
# libraries, no stage/lib, no DLLs. Prefer the BOOST_ROOT environment
# variable, but also allow CMake -DBoost_ROOT=...
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

if(NOT EXISTS "${Boost_ROOT}/boost/version.hpp")
    message(FATAL_ERROR
        "Boost headers were not found under:\n"
        "  ${Boost_ROOT}\n"
        "Expected: ${Boost_ROOT}/boost/version.hpp"
    )
endif()

# Header-only Boost: expose the include root under the same variable name
# the library targets already use (${Boost_INCLUDE_DIRS}).
set(Boost_INCLUDE_DIRS "${Boost_ROOT}")

message(STATUS "Boost root (headers): ${Boost_ROOT}")

# ------------------------------------------------------------------
# Interface helper target all test executables can link
#
# Header-only: this only provides the Boost include path and the
# WITH_BOOST_TESTS define. No libraries are linked.
# ------------------------------------------------------------------

add_library(boost_test_support INTERFACE)

target_include_directories(
    boost_test_support
    INTERFACE
        "${Boost_ROOT}"
)

target_compile_definitions(
    boost_test_support
    INTERFACE
        WITH_BOOST_TESTS
)

# ------------------------------------------------------------------
# Helper function to configure a Boost.Test executable
# ------------------------------------------------------------------

function(configure_boost_test_exe target)

    target_link_libraries(
        ${target}
        PRIVATE
            boost_test_support
    )

endfunction()

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


add_library(boost_headers INTERFACE)
target_include_directories(boost_headers INTERFACE "${Boost_ROOT}")

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

add_subdirectory(Apps)
