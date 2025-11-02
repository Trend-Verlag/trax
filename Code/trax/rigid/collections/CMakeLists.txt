add_library(trax_rigid_collections STATIC
    "./source/MovableTrackAutoConnecting_Imp.cpp"
    "./source/SceneCollection_Imp.cpp"
    "./source/TrackSystemMovable_Imp.cpp"
)

set(PUBLIC_HEADERS
    "./MovableTrackAutoConnecting.h"
    "./SceneCollection.h"
)

set(PRIVATE_HEADERS
    "./source/MovableTrackAutoConnecting_Imp.h"
    "./source/SceneCollection_Imp.h"
    "./source/TrackSystemMovable_Imp.h"
)

target_sources(trax_rigid_collections
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_rigid_collections
    PUBLIC
        trax_rigid
        trax_core_collections
)

target_include_directories(trax_rigid_collections PRIVATE ${Boost_INCLUDE_DIRS})
