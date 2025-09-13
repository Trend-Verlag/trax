add_library(trax_rigid_support 
    STATIC
        "./source/RigidSupportXML.cpp"
)

target_sources(trax_rigid_support
    PUBLIC
        "./RigidSupportXML.h"
)

target_link_libraries(trax_rigid_support
     PUBLIC
        trax_rigid
)

target_include_directories(trax_rigid_support PRIVATE ${Boost_INCLUDE_DIRS})
