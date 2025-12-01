add_library(trax_trains 
    STATIC
        "./source/Bogie_Imp.cpp"
        "./source/Cargo_Imp.cpp"
        "./source/RollingStock_Imp.cpp"
        "./source/Train_Imp.cpp"
        "./source/TrainComponent_Imp.cpp"
        "./source/WheelFrame_Imp.cpp"
        "./source/WheelFrameSensors_Imp.cpp"
)

set(PUBLIC_HEADERS
    "./Bogie.h"
    "./Cargo.h"
    "./RailRunner.h"
    "./RollingStock.h"
    "./Train.h"
    "./TrainComponent.h"
    "./WheelFrame.h"
    "./WheelFrameSensors.h"
)

set(PRIVATE_HEADERS
    "./source/Bogie_Imp.h"
    "./source/Cargo_Imp.h"
    "./source/RailRunner_Imp.h"
    "./source/RollingStock_Imp.h"
    "./source/Train_Imp.h"
    "./source/TrainComponent_Imp.h"
    "./source/WheelFrame_Imp.h"
    "./source/WheelFrameSensors_Imp.h"
)

target_sources(trax_trains
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS} )

target_link_libraries(trax_trains
    PUBLIC
        trax_rigid
)