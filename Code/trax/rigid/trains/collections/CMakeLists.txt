add_library(trax_trains_collections
    STATIC
        "./source/CargoCollection_Imp.cpp"
        "./source/Fleet_Imp.cpp"
)

set(PUBLIC_HEADERS
    "./CargoCollection.h"
    "./Fleet.h"
)

set(PRIVATE_HEADERS
    "./source/CargoCollection_Imp.h"
    "./source/Fleet_Imp.h"
)

target_sources(trax_trains_collections
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_trains_collections
    PUBLIC
        trax_trains
)