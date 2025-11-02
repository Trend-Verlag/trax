add_library(trax_rigid 
    STATIC
        "./source/Body_Imp.cpp"
        "./source/Geom_Imp.cpp"
        "./source/Gestalt_Imp.cpp"
        "./source/Material_Imp.cpp"
        "./source/MovableTrack_Imp.cpp"
        "./source/Scene_Imp.cpp"
        "./source/Shape_Imp.cpp"
        "./source/Simulator_Imp.cpp"
        "./source/StaticTrack_Imp.cpp"
        "./source/TrackJointFeeder_Imp.cpp"
        "./source/TractionForceCharacteristic_Imp.cpp"
)

# Store public headers in a variable
set(PUBLIC_HEADERS
    "./Body.h"
    "./Geom.h"
    "./GeomType.h"
    "./Gestalt.h"
    "./Joint.h"
    "./Material.h"
    "./MovableTrack.h"
    "./Scene.h"
    "./Shape.h"
    "./Simulator.h"
    "./StaticTrack.h"
    "./TrackJoint.h"
    "./TrackJointFeeder.h"
    "./TrackJointLimits.h"
    "./TractionForceCharacteristic.h"
    "./Wheelset.h"
)

# Store private headers in a variable
set(PRIVATE_HEADERS
    "./source/Body_Imp.h"
    "./source/Geom_Imp.h"
    "./source/Gestalt_Imp.h"
    "./source/MovableTrack_Imp.h"
    "./source/Scene_Imp.h"
    "./source/Shape_Imp.h"
    "./source/Simulator_Imp.h"
    "./source/StaticTrack_Imp.h"
    "./source/TractionForceCharacteristic_Imp.h"
)

target_sources(trax_rigid
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Header Files" FILES ${PRIVATE_HEADERS})


target_link_libraries(trax_rigid
    PUBLIC
        trax_core
)