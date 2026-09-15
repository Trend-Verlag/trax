add_library(trax_modules_support 
    STATIC
        "./source/Anl3ModuleReader.cpp"
        "./source/Anl3TrackSystemReader.cpp"
        "./source/Anl4ModuleReader.cpp"
        "./source/Anl4ModuleWriter.cpp"
        "./source/Anl4TrackSystemReader.cpp"
        "./source/AnlReader.cpp"
        "./source/AnlWriter.cpp"
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
        "./source/ModuleCommands_Imp.cpp"
        "./source/RailRunnerParser_Imp.cpp"
        "./source/RigidSupportXML.cpp"
        "./source/SupportXML_Collection.cpp"
        "./source/SupportXML_Twist.cpp"
        "./source/SupportXML_Train.cpp"
        "./source/TrackSystemParser_Imp.cpp"
        "./source/TrackSystemReader.cpp"
        "./source/TrainCollectionSupportXML.cpp"
        "./source/TraxSupportXML.cpp"
        "./source/XMLStreamIn.cpp"
        "./source/XMLStreamOut.cpp"
)

set(PUBLIC_HEADERS
    "./Anl3ModuleReader.h"
    "./Anl3TrackSystemReader.h"
    "./Anl4ModuleReader.h"
    "./Anl4ModuleWriter.h"
    "./Anl4TrackSystemReader.h"
    "./AnlReader.h"
    "./AnlWriter.h"
    "./CollectionSupportXML.h"
    "./ModuleCommands.h"
    "./RigidSupportXML.h"
    "./SupportXML.h"
    "./TrackSystemReader.h"
    "./TrainCollectionSupportXML.h"
    "./TraxSupportXML.h"
    "./XMLStreamIn.h"
    "./XMLStreamOut.h"
)

set(PRIVATE_HEADERS
    "./source/ModuleCommands_Imp.h"
    "./source/RailRunnerParser_Imp.h"
    "./source/TrackSystemParser_Imp.h"
)

target_sources(trax_modules_support
    PUBLIC
        ${PUBLIC_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_modules_support
     PUBLIC
        trax_modules
        trax_trains_support
        appframe
)

target_include_directories(trax_modules_support PRIVATE ${Boost_INCLUDE_DIRS})
