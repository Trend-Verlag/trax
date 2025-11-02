add_library(trax_rigid_physx 
    STATIC
        "./source/PhysX_Body_Imp.cpp"
        "./source/PhysX_GeomBase_Imp.cpp"
        "./source/PhysX_GeomSpecials_Imp.cpp"
        "./source/PhysX_Gestalt_Imp.cpp"
        "./source/PhysX_Joints_Imp.cpp"
        "./source/PhysX_Scene.cpp"
        "./source/PhysX_Shape_Imp.cpp"
        "./source/PhysX_Simulator.cpp"
        "./source/PhysX_TrackJoint.cpp"
)

set(PRIVATE_HEADERS
    "./source/PhysX_Adapter.h"
    "./source/PhysX_Body_Imp.h"
    "./source/PhysX_GeomBase_Imp.h"
    "./source/PhysX_GeomSpecials_Imp.h"
    "./source/PhysX_Gestalt_Imp.h"
    "./source/PhysX_Imp.h"
    "./source/PhysX_Joints_Imp.h"
    "./source/PhysX_Scene.h"
    "./source/PhysX_Shape_Imp.h"
    "./source/PhysX_Simulator.h"
    "./source/PhysX_TrackJoint.h"
)

target_sources(trax_rigid_physx
    PUBLIC
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Private Headers" FILES ${PRIVATE_HEADERS} )

target_link_libraries(trax_rigid_physx
    PUBLIC
        trax_rigid
)

# Reuse shared PhysX linking
include("${CMAKE_SOURCE_DIR}/CMake/TraxPhysX.cmake")
trax_link_physx(trax_rigid_physx)

