add_library(trax_core_support 
    STATIC
        "./source/TraxSupportXML.cpp"
)

target_sources(trax_core_support
    PUBLIC
        "./TraxSupportStream.h"
        "./TraxSupportXML.h"
)

target_link_libraries(trax_core_support 
     PUBLIC 
        dim_support
        spat_support
        trax_core
)

target_include_directories(trax_core_support PRIVATE ${Boost_INCLUDE_DIRS})
