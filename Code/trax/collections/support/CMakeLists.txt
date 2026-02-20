add_library(trax_core_collections_support 
    STATIC
        "./source/Anl3TrackSystemReader.cpp"
        "./source/Anl4TrackSystemReader.cpp"
        "./source/Anl4TrackSystemWriter.cpp"
        "./source/FixturesCollections.cpp"
        "./source/SupportXML_Collection.cpp"
        "./source/SupportXML_Twist.cpp"
        "./source/TrackSystemParser_Imp.cpp"
        "./source/TrackSystemReader.cpp"
        "./source/curves/SupportXML_Arc.cpp"
        "./source/curves/SupportXML_ArcP.cpp"
        "./source/curves/SupportXML_Clothoid.cpp"
        "./source/curves/SupportXML_Cubic.cpp"
        "./source/curves/SupportXML_Curve.cpp"
        "./source/curves/SupportXML_EEPCurve.cpp"
        "./source/curves/SupportXML_Helix.cpp"
        "./source/curves/SupportXML_HelixP.cpp"
        "./source/curves/SupportXML_Line.cpp"
        "./source/curves/SupportXML_LineP.cpp"
        "./source/curves/SupportXML_PolygonalChain.cpp"
        "./source/curves/SupportXML_Rotator.cpp"
        "./source/curves/SupportXML_SampledCurve.cpp"
        "./source/curves/SupportXML_Spline.cpp"
)

set(PRIVATE_HEADERS
    "./source/TrackSystemParser_Imp.h"
)

set(PUBLIC_HEADERS
    "./Anl3TrackSystemReader.h"
    "./Anl4TrackSystemReader.h"
    "./Anl4TrackSystemWriter.h"
    "./CollectionSupportXML.h"
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
