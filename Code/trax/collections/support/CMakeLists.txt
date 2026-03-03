add_library(trax_core_collections_support 
    STATIC
        "./source/Anl3TrackSystemReader.cpp"
        "./source/Anl4TrackSystemReader.cpp"
        "./source/Anl4TrackSystemWriter.cpp"
        "./source/FixturesCollections.cpp"
        "./source/SupportXML_Collection.cpp"
        "./source/TrackSystemParser_Imp.cpp"
        "./source/TrackSystemReader.cpp"
)

set(PRIVATE_HEADERS
    "./source/TrackSystemParser_Imp.h"
)

set(PUBLIC_HEADERS
    "./Anl3TrackSystemReader.h"
    "./Anl4TrackSystemReader.h"
    "./Anl4TrackSystemWriter.h"
    "./CollectionSupportXML.h"
    "./CollectionSupportWriteXML.h"
    "./FixturesCollections.h"
    "./TrackSystemReader.h"
)

target_sources(trax_core_collections_support
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS} )

target_link_libraries(trax_core_collections_support 
     PUBLIC 
        trax_core_collections
)

target_include_directories(trax_core_collections_support PRIVATE ${Boost_INCLUDE_DIRS})
