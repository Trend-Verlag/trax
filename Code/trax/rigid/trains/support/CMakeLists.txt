add_library(trax_trains_support 
    STATIC
        "./source/FixturesTrain.cpp"
        "./source/RailRunnerParser_Imp.cpp"
        "./source/RollingStockCreator.cpp"
        "./source/RollingStockFileReader.cpp"
        "./source/SupportXML_Train.cpp"
        "./source/TrainCreator.cpp"
        "./source/TrainFileReader.cpp"
)

set(PUBLIC_HEADERS
    "./FixturesTrain.h"
    "./RailRunnerParser.h"
    "./RollingStockCreator.h"
    "./RollingStockFileReader.h"
    "./TrainCreator.h"
    "./TrainFileReader.h"
)

set(PRIVATE_HEADERS
    "./source/RailRunnerParser_Imp.h"
)

target_sources(trax_trains_support
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_trains_support
    PUBLIC
        trax_trains
)

target_include_directories(trax_trains_support PRIVATE ${Boost_INCLUDE_DIRS})
