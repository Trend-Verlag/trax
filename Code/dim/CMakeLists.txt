# Static library 'dim'

# Collect sources (exclude tests/)
file(GLOB_RECURSE DIM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
list(FILTER DIM_SOURCES EXCLUDE REGEX "[/\\]tests[/\\]")

# Collect headers (exclude tests/)
file(GLOB_RECURSE DIM_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
list(FILTER DIM_HEADERS EXCLUDE REGEX "[/\\]tests[/\\]")

if(DIM_SOURCES STREQUAL "")
    message(WARNING "dim: no .cpp source files found (after excluding tests/)")
endif()

add_library(dim STATIC
    ${DIM_SOURCES}
    ${DIM_HEADERS}   # for IDE visibility only; not strictly required
)

# Allow includes as: #include "dim/YourHeader.h"
set(_DIM_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(dim
    PUBLIC
        $<BUILD_INTERFACE:${_DIM_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Depends on common (header-only)
target_link_libraries(dim
    PUBLIC
        common
)

target_compile_definitions(dim PUBLIC DIM_STATIC_LIBRARY)
target_compile_features(dim PUBLIC cxx_std_17)

# Publish only non-test headers
target_sources(dim
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${_DIM_PARENT_DIR}
        FILES ${DIM_HEADERS}
)
