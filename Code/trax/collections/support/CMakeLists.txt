add_library(trax_core_collections_support 
    STATIC
        "./source/FixturesCollections.cpp"
)

set(PRIVATE_HEADERS
)

set(PUBLIC_HEADERS
    "./FixturesCollections.h"
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
