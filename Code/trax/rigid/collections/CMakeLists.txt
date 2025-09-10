add_library(trax_rigid_collections STATIC
    "./source/MovableTrackAutoConnecting_Imp.cpp"
    "./source/TrackSystemMovable_Imp.cpp"
)

target_sources(trax_rigid_collections
    PUBLIC
        "./MovableTrackAutoConnecting.h"
    PRIVATE
        "./source/MovableTrackAutoConnecting_Imp.h"
        "./source/TrackSystemMovable_Imp.h"
)

target_link_libraries(trax_rigid_collections
    PUBLIC
        trax_rigid
        trax_core_collections
)

target_include_directories(trax_rigid_collections PRIVATE ${Boost_INCLUDE_DIRS})
