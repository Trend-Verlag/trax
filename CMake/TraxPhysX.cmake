# Guard against multiple includes
if(DEFINED TRAX_PHYSX_CMAKE_INCLUDED)
    return()
endif()
set(TRAX_PHYSX_CMAKE_INCLUDED TRUE)

# Function: trax_link_physx(<target>)
# Links the given target with PhysX and sets include directories using PHYSX_ROOT.
function(trax_link_physx target)
    if(NOT DEFINED ENV{PHYSX_ROOT})
        message(FATAL_ERROR "PHYSX_ROOT environment variable not set")
    endif()
    set(PHYSX_ROOT "$ENV{PHYSX_ROOT}")

    # PhysX includes (assumes PHYSX_ROOT/include has PxPhysicsAPI.h etc.)
    target_include_directories(${target} PRIVATE "${PHYSX_ROOT}/include")

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
    set(_PHYSX_CHECKED_LIB_DIR_CANDIDATES
        "${PHYSX_ROOT}/lib"
        "${PHYSX_ROOT}/lib64"
        "${PHYSX_ROOT}/bin"
        "${PHYSX_ROOT}/bin/win.x86_64.vc143.md/checked"
        "${PHYSX_ROOT}/compiler/public"
    )
    set(_PHYSX_PROFILE_LIB_DIR_CANDIDATES
        "${PHYSX_ROOT}/lib"
        "${PHYSX_ROOT}/lib64"
        "${PHYSX_ROOT}/bin"
        "${PHYSX_ROOT}/bin/win.x86_64.vc143.md/profile"
        "${PHYSX_ROOT}/compiler/public"
    )

    # Helper to find a PhysX lib in all 4 configurations
    function(_trax_find_physx_lib out_var base_name)
        find_library(${out_var}_DEBUG
            NAMES ${base_name}
            PATHS ${_PHYSX_DEBUG_LIB_DIR_CANDIDATES}
            NO_DEFAULT_PATH
        )
        find_library(${out_var}_RELEASE
            NAMES ${base_name}
            PATHS ${_PHYSX_RELEASE_LIB_DIR_CANDIDATES}
            NO_DEFAULT_PATH
        )
        find_library(${out_var}_CHECKED
            NAMES ${base_name}
            PATHS ${_PHYSX_CHECKED_LIB_DIR_CANDIDATES}
            NO_DEFAULT_PATH
        )
        find_library(${out_var}_PROFILE
            NAMES ${base_name}
            PATHS ${_PHYSX_PROFILE_LIB_DIR_CANDIDATES}
            NO_DEFAULT_PATH
        )

        if(NOT ${out_var}_RELEASE)
            message(FATAL_ERROR "Could not find Release PhysX library '${base_name}' under PHYSX_ROOT=${PHYSX_ROOT}")
        else()
            message(STATUS "Found PhysX Release library '${base_name}' at ${${out_var}_RELEASE}")
        endif()
        if(NOT ${out_var}_DEBUG)
            message(FATAL_ERROR "Could not find Debug PhysX library '${base_name}' under PHYSX_ROOT=${PHYSX_ROOT}")
        else()
            message(STATUS "Found PhysX Debug library '${base_name}' at ${${out_var}_DEBUG}")
        endif()

        # Checked/Profile are optional; fall back to Release if not present
        if(NOT ${out_var}_CHECKED)
            set(${out_var}_CHECKED "${${out_var}_RELEASE}" PARENT_SCOPE)
            message(WARNING "PhysX Checked library '${base_name}' not found, falling back to Release")
        else()
            set(${out_var}_CHECKED "${${out_var}_CHECKED}" PARENT_SCOPE)
            message(STATUS "Found PhysX Checked library '${base_name}' at ${${out_var}_CHECKED}")
        endif()
        if(NOT ${out_var}_PROFILE)
            set(${out_var}_PROFILE "${${out_var}_RELEASE}" PARENT_SCOPE)
            message(WARNING "PhysX Profile library '${base_name}' not found, falling back to Release")
        else()
            set(${out_var}_PROFILE "${${out_var}_PROFILE}" PARENT_SCOPE)
            message(STATUS "Found PhysX Profile library '${base_name}' at ${${out_var}_PROFILE}")
        endif()

        # Also expose Debug/Release to parent
        set(${out_var}_DEBUG "${${out_var}_DEBUG}" PARENT_SCOPE)
        set(${out_var}_RELEASE "${${out_var}_RELEASE}" PARENT_SCOPE)
    endfunction()

    # Core PhysX libs typically needed (adjust to your actual usage)
    _trax_find_physx_lib(PX_FOUNDATION      PhysXFoundation_64)
    _trax_find_physx_lib(PX_COMMON          PhysXCommon_64)
    _trax_find_physx_lib(PX_PHYSX           PhysX_64)
    _trax_find_physx_lib(PX_COOKING         PhysXCooking_64)
    _trax_find_physx_lib(PX_EXTENSIONS      PhysXExtensions_static_64)
    _trax_find_physx_lib(PX_PVDSDK          PhysXPvdSDK_static_64)

    # Map CMake configs to PhysX configs:
    # - Debug           -> Debug
    # - Release         -> Release
    # - RelWithDebInfo  -> Checked   (good default for development; flip to Profile if profiling perf)
    # - MinSizeRel      -> Release
    # Also support explicit 'Checked' and 'Profile' if you add them to CMAKE_CONFIGURATION_TYPES.
    target_link_libraries(${target} PRIVATE
        # PhysXFoundation
        $<$<CONFIG:Debug>:${PX_FOUNDATION_DEBUG}>
        $<$<CONFIG:Release>:${PX_FOUNDATION_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_FOUNDATION_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_FOUNDATION_RELEASE}>
        $<$<CONFIG:Checked>:${PX_FOUNDATION_CHECKED}>
        $<$<CONFIG:Profile>:${PX_FOUNDATION_PROFILE}>

        # PhysXCommon
        $<$<CONFIG:Debug>:${PX_COMMON_DEBUG}>
        $<$<CONFIG:Release>:${PX_COMMON_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_COMMON_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_COMMON_RELEASE}>
        $<$<CONFIG:Checked>:${PX_COMMON_CHECKED}>
        $<$<CONFIG:Profile>:${PX_COMMON_PROFILE}>

        # PhysX
        $<$<CONFIG:Debug>:${PX_PHYSX_DEBUG}>
        $<$<CONFIG:Release>:${PX_PHYSX_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_PHYSX_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_PHYSX_RELEASE}>
        $<$<CONFIG:Checked>:${PX_PHYSX_CHECKED}>
        $<$<CONFIG:Profile>:${PX_PHYSX_PROFILE}>

        # PhysXCooking
        $<$<CONFIG:Debug>:${PX_COOKING_DEBUG}>
        $<$<CONFIG:Release>:${PX_COOKING_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_COOKING_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_COOKING_RELEASE}>
        $<$<CONFIG:Checked>:${PX_COOKING_CHECKED}>
        $<$<CONFIG:Profile>:${PX_COOKING_PROFILE}>

        # PhysXExtensions
        $<$<CONFIG:Debug>:${PX_EXTENSIONS_DEBUG}>
        $<$<CONFIG:Release>:${PX_EXTENSIONS_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_EXTENSIONS_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_EXTENSIONS_RELEASE}>
        $<$<CONFIG:Checked>:${PX_EXTENSIONS_CHECKED}>
        $<$<CONFIG:Profile>:${PX_EXTENSIONS_PROFILE}>

        # PhysXPvdSDK
        $<$<CONFIG:Debug>:${PX_PVDSDK_DEBUG}>
        $<$<CONFIG:Release>:${PX_PVDSDK_RELEASE}>
        $<$<CONFIG:RelWithDebInfo>:${PX_PVDSDK_CHECKED}>
        $<$<CONFIG:MinSizeRel>:${PX_PVDSDK_RELEASE}>
        $<$<CONFIG:Checked>:${PX_PVDSDK_CHECKED}>
        $<$<CONFIG:Profile>:${PX_PVDSDK_PROFILE}>
    )

    # If building/linking PhysX static libs requires defines, add them here:
    # target_compile_definitions(${target} PRIVATE PX_PHYSX_STATIC_LIB)
endfunction()