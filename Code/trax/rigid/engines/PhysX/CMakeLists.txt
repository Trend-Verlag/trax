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

# Reuse shared PhysX linking
include("${CMAKE_SOURCE_DIR}/CMake/TraxPhysX.cmake")
trax_link_physx(trax_rigid_physx)

