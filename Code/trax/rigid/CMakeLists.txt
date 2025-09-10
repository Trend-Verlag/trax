add_library(trax_rigid 
    STATIC
        "./source/Body_Imp.cpp"
        "./source/MovableTrack_Imp.cpp"
)

target_sources(trax_rigid
    PUBLIC
        "./MovableTrack.h"
        "./Body.h"
        "./TrackJoint.h"
        "./TrackJointFeeder.h"
        "./TrackJointLimits.h"
    PRIVATE
        "./source/Body_Imp.h"
        "./source/MovableTrack_Imp.h"
        "./source/TrackJointFeeder_Imp.cpp"
)

target_link_libraries(trax_rigid
    PUBLIC
        trax_core
)