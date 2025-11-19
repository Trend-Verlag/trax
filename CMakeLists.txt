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
add_subdirectory(Code/trax/rigid/modules/support)
add_subdirectory(Code/trax/support)

add_subdirectory(Test)
add_subdirectory(Test/common)
add_subdirectory(Test/dim)
add_subdirectory(Test/spat)
add_subdirectory(Test/trax)
add_subdirectory(Test/trax/rigid)
add_subdirectory(Test/trax/rigid/modules)
add_subdirectory(Test/trax/rigid/trains)

add_subdirectory(Apps)
