add_library(trax_trains_physx 
    STATIC
        "./source/PhysX_Coupling_Imp.cpp"
        "./source/PhysX_Swivel_Imp.cpp"
        "./source/PhysX_TrainScene.cpp"
)

set(PRIVATE_HEADERS
    "./source/PhysX_Coupling_Imp.h"
    "./source/PhysX_Joint_Imp.h"
    "./source/PhysX_Swivel_Imp.h"
    "./source/PhysX_TrainScene.h"
)

target_sources(trax_trains_physx
    PUBLIC
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Header Files" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_trains_physx
    PUBLIC
        trax_core_collections
        trax_rigid_support
        trax_rigid_physx
        trax_trains
)

# Reuse shared PhysX linking
include("${CMAKE_SOURCE_DIR}/CMake/TraxPhysX.cmake")
trax_link_physx(trax_trains_physx)