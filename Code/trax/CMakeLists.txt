# Static library 'trax_core'

# Collect sources (exclude tests/)
file(GLOB_RECURSE TRAX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
list(FILTER TRAX_SOURCES EXCLUDE REGEX "[/\\]tests[/\\]")
list(FILTER TRAX_SOURCES EXCLUDE REGEX "[/\\]collections[/\\]")
list(FILTER TRAX_SOURCES EXCLUDE REGEX "[/\\]engines[/\\]")

# Collect headers (exclude tests/)
file(GLOB_RECURSE TRAX_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
list(FILTER TRAX_HEADERS EXCLUDE REGEX "[/\\]tests[/\\]")
list(FILTER TRAX_HEADERS EXCLUDE REGEX "[/\\]collections[/\\]")
list(FILTER TRAX_HEADERS EXCLUDE REGEX "[/\\]engines[/\\]")

if(TRAX_SOURCES STREQUAL "")
    message(WARNING "trax_core: no .cpp source files found (after excluding tests/)")
endif()

add_library(trax_core STATIC
    ${TRAX_SOURCES}
    ${TRAX_HEADERS}   # for IDE visibility only; not strictly required
)

# Allow includes as: #include "trax_core/YourHeader.h"
set(_TRAX_CORE_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(trax_core
    PUBLIC
        $<BUILD_INTERFACE:${_TRAX_CORE_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Depends on dim and spat
target_link_libraries(trax_core
    PUBLIC
    common
    dim
    spat
)

target_compile_definitions(trax_core PUBLIC TRAX_STATIC_LIBRARY)
target_compile_features(trax_core PUBLIC cxx_std_17)

# Publish only non-test headers
target_sources(trax_core
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${_TRAX_CORE_PARENT_DIR}
        FILES ${TRAX_HEADERS}
)

# ---------- Boost (header-only math/tools) ----------
# Root CMakeLists already did find_package(Boost ...).
# Add Boost include directory for header-only usage (math/tools/roots.hpp).
target_include_directories(trax_core PRIVATE ${Boost_INCLUDE_DIRS})

# No extra Boost libraries needed for math/tools.
