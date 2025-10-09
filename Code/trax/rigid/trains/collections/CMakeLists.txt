add_library(trax_trains_collections
    STATIC
        "./source/CargoCollection_Imp.cpp"
        "./source/Fleet_Imp.cpp"
)

target_sources(trax_trains_collections
    PUBLIC
        "./CargoCollection.h"
        "./Fleet.h"
   PRIVATE
        "./source/CargoCollection_Imp.h"
        "./source/Fleet_Imp.h"
)

target_link_libraries(trax_trains_collections
    PUBLIC
        trax_trains
)