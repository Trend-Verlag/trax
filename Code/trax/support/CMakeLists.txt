add_library(trax_core_support 
    STATIC
        "./source/Fixtures.cpp"
        "./source/TraxSupportXML.cpp"
)

target_sources(trax_core_support
    PUBLIC
        "./Fixtures.h"
        "./TraxSupportStream.h"
        "./TraxSupportXML.h"
)

target_link_libraries(trax_core_support 
     PUBLIC 
        dim_support
        spat_support
        trax_core
)

target_include_directories(trax_core_support PRIVATE ${Boost_INCLUDE_DIRS})

# ---- Fixtures path (project root /Fixtures) exposed to C++ ----
# CMAKE_SOURCE_DIR points at the top-level CMakeLists.txt directory.
set(TRAX_FIXTURES_DIR_PATH "${CMAKE_SOURCE_DIR}/Fixtures")

# Normalize to forward slashes (safe on Windows & Unix)
file(TO_CMAKE_PATH "${TRAX_FIXTURES_DIR_PATH}" TRAX_FIXTURES_DIR_PATH)

# Export as a preprocessor definition. Usage in C++: std::string path = TRAX_FIXTURES_DIR;
target_compile_definitions(trax_core_support
    PUBLIC
        TRAX_FIXTURES_DIR=\"${TRAX_FIXTURES_DIR_PATH}\"
)
