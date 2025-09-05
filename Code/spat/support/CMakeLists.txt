add_library(spat_support 
    INTERFACE
        SpatSupportStream.h
        SpatSupportXML.h
)

target_link_libraries(spat_support 
     INTERFACE 
        common_support
        spat
)
