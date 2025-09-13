add_library(trax_trains_collections
    STATIC
        "./source/Fleet_Imp.cpp"
)

target_sources(trax_trains_collections
    PUBLIC
        "./Fleet.h"
    PRIVATE
        "./source/Fleet_Imp.h"
)

target_link_libraries(trax_trains_collections
    PUBLIC
        trax_trains
)