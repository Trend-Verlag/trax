add_library(trax_rigid_support 
    STATIC
        "./source/RigidSupportXML.cpp"
)

set(PUBLIC_HEADERS
    "./RigidSupportXML.h"
)

target_sources(trax_rigid_support
    PUBLIC
        ${PUBLIC_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})

target_link_libraries(trax_rigid_support
     PUBLIC
        trax_rigid
)

target_include_directories(trax_rigid_support PRIVATE ${Boost_INCLUDE_DIRS})
