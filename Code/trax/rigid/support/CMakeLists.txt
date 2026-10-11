add_library(trax_rigid_support 
    STATIC
        "./source/stb_image.cpp"
        "./source/SupportTerrain.cpp"
)

set(PUBLIC_HEADERS
    "./SupportTerrain.h"
)

set(PRIVATE_HEADERS
    "./source/stb_image.h"
)

target_sources(trax_rigid_support
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_rigid_support
     PUBLIC
        trax_rigid
)

# target_include_directories(trax_rigid_support PRIVATE ${Boost_INCLUDE_DIRS})
