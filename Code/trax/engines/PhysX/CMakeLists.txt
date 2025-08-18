# Static library 'trax_core_physx'

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
    message(WARNING "trax_core_physx: no .cpp source files found (after excluding tests/)")
endif()

add_library(trax_core_physx STATIC
    ${TRAX_SOURCES}
    ${TRAX_HEADERS}   # for IDE visibility only; not strictly required
)

# Allow includes as: #include "trax_core_physx/YourHeader.h"
set(_TRAX_CORE_PHYSX_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(trax_core_physx
    PUBLIC
        $<BUILD_INTERFACE:${_TRAX_CORE_PHYSX_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Depends on dim and spat
target_link_libraries(trax_core_physx
    PUBLIC
    common
    dim
    spat
    trax_core
)

target_compile_definitions(trax_core_physx PUBLIC TRAX_STATIC_LIBRARY)
target_compile_features(trax_core_physx PUBLIC cxx_std_17)

# Publish only non-test headers
target_sources(trax_core_physx
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${_TRAX_CORE_PHYSX_PARENT_DIR}
        FILES ${TRAX_HEADERS}
)

# ---------- Boost (header-only math/tools) ----------
# Root CMakeLists already did find_package(Boost ...).
# Add Boost include directory for header-only usage (math/tools/roots.hpp).
#target_include_directories(trax_core_physx  PRIVATE ${Boost_INCLUDE_DIRS})

# No extra Boost libraries needed for math/tools.

# ---------- PhysX linking ----------
if(NOT DEFINED ENV{PHYSX_ROOT})
    message(FATAL_ERROR "PHYSX_ROOT environment variable not set")
endif()
set(PHYSX_ROOT "$ENV{PHYSX_ROOT}")

# PhysX includes (assumes PHYSX_ROOT/include has PxPhysicsAPI.h etc.)
target_include_directories(trax_core_physx PRIVATE "${PHYSX_ROOT}/include")

# Guess library directory candidates (adjust if your layout differs)
set(_PHYSX_LIB_DIR_CANDIDATES
    "${PHYSX_ROOT}/lib"
    "${PHYSX_ROOT}/lib64"
    "${PHYSX_ROOT}/bin"
    "${PHYSX_ROOT}/bin/win.x86_64.vc143.mt/debug"
    "${PHYSX_ROOT}/bin/win.x86_64.vc143.mt/release"
    "${PHYSX_ROOT}/compiler/public"
)

# Helper macro to find a PhysX lib (you can add/remove names as needed)
function(_find_physx_lib out_var base_name)
    # Accept possible debug/release naming variants
    find_library(${out_var}_DEBUG
        NAMES
            ${base_name}DEBUG
            ${base_name}_DEBUG
            ${base_name}_debug
            ${base_name}d
            ${base_name}  # fallback if no separate debug lib
        PATHS ${_PHYSX_LIB_DIR_CANDIDATES}
        NO_DEFAULT_PATH
    )
    find_library(${out_var}_RELEASE
        NAMES
            ${base_name}
        PATHS ${_PHYSX_LIB_DIR_CANDIDATES}
        NO_DEFAULT_PATH
    )
    if(NOT ${out_var}_RELEASE AND NOT ${out_var}_DEBUG)
        message(FATAL_ERROR "Could not find PhysX library '${base_name}' (searched variants) under PHYSX_ROOT=${PHYSX_ROOT}")
    endif()
endfunction()

# Core PhysX libs typically needed (adjust to your actual usage)
_find_physx_lib(PX_FOUNDATION      PhysXFoundation_64)
_find_physx_lib(PX_COMMON          PhysXCommon_64)
_find_physx_lib(PX_PHYSX           PhysX_64)
_find_physx_lib(PX_COOKING         PhysXCooking_64)
_find_physx_lib(PX_EXTENSIONS      PhysXExtensions_static_64)
_find_physx_lib(PX_PVDSDK          PhysXPvdSDK_static_64)

# Link per configuration
target_link_libraries(trax_core_physx PRIVATE
    $<$<CONFIG:Debug>:${PX_FOUNDATION_DEBUG}>
    $<$<CONFIG:Release>:${PX_FOUNDATION_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_FOUNDATION_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_FOUNDATION_RELEASE}>

    $<$<CONFIG:Debug>:${PX_COMMON_DEBUG}>
    $<$<CONFIG:Release>:${PX_COMMON_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_COMMON_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_COMMON_RELEASE}>

    $<$<CONFIG:Debug>:${PX_PHYSX_DEBUG}>
    $<$<CONFIG:Release>:${PX_PHYSX_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_PHYSX_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_PHYSX_RELEASE}>

    $<$<CONFIG:Debug>:${PX_COOKING_DEBUG}>
    $<$<CONFIG:Release>:${PX_COOKING_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_COOKING_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_COOKING_RELEASE}>

    $<$<CONFIG:Debug>:${PX_EXTENSIONS_DEBUG}>
    $<$<CONFIG:Release>:${PX_EXTENSIONS_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_EXTENSIONS_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_EXTENSIONS_RELEASE}>
)

# If you have an export/import macro logic for building PhysX as static libs,
# you might need definitions like PX_PHYSX_STATIC_LIB or similar:
# target_compile_definitions(trax_core_physx PRIVATE PX_PHYSX_STATIC_LIB)
# (Uncomment only if required by your PhysX build configuration.)


# ---------- Tests subdirectory (if any) ----------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()