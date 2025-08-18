# Static library 'trax_collections'

# Collect sources (exclude tests/)
file(GLOB_RECURSE TRAX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
list(FILTER TRAX_SOURCES EXCLUDE REGEX "[/\\]tests[/\\]")

# Collect headers (exclude tests/)
file(GLOB_RECURSE TRAX_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
list(FILTER TRAX_HEADERS EXCLUDE REGEX "[/\\]tests[/\\]")

if(TRAX_SOURCES STREQUAL "")
    message(WARNING "trax_collections: no .cpp source files found (after excluding tests/)")
endif()

add_library(trax_collections STATIC
    ${TRAX_SOURCES}
    ${TRAX_HEADERS}   # for IDE visibility only; not strictly required
)

# Allow includes as: #include "trax/collections/YourHeader.h"
set(_TRAX_COLLECTIONS_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

target_include_directories(trax_collections
    PUBLIC
        $<BUILD_INTERFACE:${_TRAX_COLLECTIONS_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Depends on 
target_link_libraries(trax_collections
    PUBLIC
    common
    dim
    spat
    trax_core
)

target_compile_definitions(trax_collections PUBLIC TRAX_STATIC_LIBRARY)
target_compile_features(trax_collections PUBLIC cxx_std_17)

# Publish only non-test headers
target_sources(trax_collections
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${_TRAX_COLLECTIONS_PARENT_DIR}
        FILES ${TRAX_HEADERS}
)

# ---------- Boost (header-only) ----------
# Root CMakeLists already did find_package(Boost ...).
# Add Boost include directory for header-only usage
target_include_directories(trax_collections PRIVATE ${Boost_INCLUDE_DIRS})

# No extra Boost libraries needed for.


# ---------- Tests subdirectory (if any) ----------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()