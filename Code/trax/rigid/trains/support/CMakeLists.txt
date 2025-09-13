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

target_sources(trax_trains_support
    PUBLIC
        "./FixturesTrain.h"
        "./RailRunnerParser.h"
        "./RollingStockCreator.h"
        "./RollingStockFileReader.h"
        "./TrainCreator.h"
        "./TrainFileReader.h"
    PRIVATE
        "./source/RailRunnerParser_Imp.h"
)

target_link_libraries(trax_trains_support
    PUBLIC
        trax_trains
)

target_include_directories(trax_trains_support PRIVATE ${Boost_INCLUDE_DIRS})
