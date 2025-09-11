add_library(trax_rigid_physx 
    STATIC
        "./source/PhysX_Body_Imp.cpp"
        "./source/PhysX_GeomBase_Imp.cpp"
        "./source/PhysX_GeomSpecials_Imp.cpp"
        "./source/PhysX_Gestalt_Imp.cpp"
        "./source/PhysX_Scene.cpp"
        "./source/PhysX_Shape_Imp.cpp"
        "./source/PhysX_Simulator.cpp"
        "./source/PhysX_TrackJoint.cpp"
)

target_sources(trax_rigid_physx
    PUBLIC
    PRIVATE
        "./source/PhysX_Adapter.h"
        "./source/PhysX_Body_Imp.h"
        "./source/PhysX_GeomBase_Imp.h"
        "./source/PhysX_GeomSpecials_Imp.h"
        "./source/PhysX_Gestalt_Imp.h"
        "./source/PhysX_Imp.h"
        "./source/PhysX_Scene.h"
        "./source/PhysX_Shape_Imp.h"
        "./source/PhysX_Simulator.h"
        "./source/PhysX_TrackJoint.h"
        # Add any additional headers needed
)

target_link_libraries(trax_rigid_physx
    PUBLIC
        trax_rigid
)

# ---------- PhysX linking ----------
if(NOT DEFINED ENV{PHYSX_ROOT})
    message(FATAL_ERROR "PHYSX_ROOT environment variable not set")
endif()
set(PHYSX_ROOT "$ENV{PHYSX_ROOT}")

# PhysX includes (assumes PHYSX_ROOT/include has PxPhysicsAPI.h etc.)
target_include_directories(trax_rigid_physx PRIVATE "${PHYSX_ROOT}/include")

# Guess library directory candidates (adjust if your layout differs)
set(_PHYSX_DEBUG_LIB_DIR_CANDIDATES
    "${PHYSX_ROOT}/lib"
    "${PHYSX_ROOT}/lib64"
    "${PHYSX_ROOT}/bin"
    "${PHYSX_ROOT}/bin/win.x86_64.vc143.md/debug"
    "${PHYSX_ROOT}/compiler/public"
)

set(_PHYSX_RELEASE_LIB_DIR_CANDIDATES
    "${PHYSX_ROOT}/lib"
    "${PHYSX_ROOT}/lib64"
    "${PHYSX_ROOT}/bin"
    "${PHYSX_ROOT}/bin/win.x86_64.vc143.md/release"
    "${PHYSX_ROOT}/compiler/public"
)

# Helper macro to find a PhysX lib (you can add/remove names as needed)
function(_find_physx_lib out_var base_name)
    # Accept possible debug/release naming variants
    find_library(${out_var}_DEBUG
        NAMES
			${base_name}
        PATHS ${_PHYSX_DEBUG_LIB_DIR_CANDIDATES}
        NO_DEFAULT_PATH
    )
    find_library(${out_var}_RELEASE
        NAMES
            ${base_name}
        PATHS ${_PHYSX_RELEASE_LIB_DIR_CANDIDATES}
        NO_DEFAULT_PATH
    )
    if(NOT ${out_var}_RELEASE)
        message(FATAL_ERROR "Could not find Release PhysX library '${base_name}' (searched variants) under PHYSX_ROOT=${PHYSX_ROOT}")
    else()
        message(STATUS "Found PhysX library '${base_name}' at ${${out_var}_RELEASE}")
    endif()
    if(NOT ${out_var}_DEBUG)
        message(FATAL_ERROR "Could not find Debug PhysX library '${base_name}' (searched variants) under PHYSX_ROOT=${PHYSX_ROOT}")
    else()
        message(STATUS "Found PhysX library '${base_name}' at ${${out_var}_DEBUG}")
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
target_link_libraries(trax_rigid_physx PRIVATE
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

    $<$<CONFIG:Debug>:${PX_PVDSDK_DEBUG}>
    $<$<CONFIG:Release>:${PX_PVDSDK_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${PX_PVDSDK_RELEASE}>
    $<$<CONFIG:MinSizeRel>:${PX_PVDSDK_RELEASE}>
)

# If you have an export/import macro logic for building PhysX as static libs,
# you might need definitions like PX_PHYSX_STATIC_LIB or similar:
# target_compile_definitions(trax_rigid_physx PRIVATE PX_PHYSX_STATIC_LIB)
# (Uncomment only if required by your PhysX build configuration.)
