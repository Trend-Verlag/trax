add_library(trax_core_collections 
    STATIC
        "./source/Collections_Imp.cpp"
        "./source/ConnectorCollection_Imp.cpp"
        "./source/IndicatorCollection_Imp.cpp"
        "./source/PulseCounterCollection_Imp.cpp"
        "./source/SignalCollection_Imp.cpp"
        "./source/TimerCollection_Imp.cpp"
        "./source/TrackCollectionContainer_Imp.cpp"
        "./source/TrackCollection_Imp.cpp"
        "./source/TrackSystem_Imp.cpp"
)

set(PUBLIC_HEADERS
    "./CollectionDecorator.h"
    "./Collections.h"
    "./ConnectorCollection.h"
    "./IndicatorCollection.h"
    "./Interfaces.h"
    "./ObjectIDDecorator.h"
    "./PulseCounterCollection.h"
    "./SignalCollection.h"
    "./TimerCollection.h"
    "./TrackCollection.h"
    "./TrackCollectionContainer.h"
    "./TrackSystem.h"
    "./TrackSystemParser.h"
)

set(PRIVATE_HEADERS
    "./source/ConnectorCollection_Imp.h"
    "./source/IndicatorCollection_Imp.h"
    "./source/PulseCounterCollection_Imp.h"
    "./source/SignalCollection_Imp.h"
    "./source/TimerCollection_Imp.h"
    "./source/TrackCollectionContainer_Imp.h"
    "./source/TrackCollection_Imp.h"
    "./source/TrackSystem_Imp.h"
)

target_sources(trax_core_collections
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_core_collections
    PUBLIC
        trax_core
)

target_include_directories(trax_core_collections PRIVATE ${Boost_INCLUDE_DIRS})
