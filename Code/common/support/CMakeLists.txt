add_library(common_support 
    INTERFACE
        CommonSupportConsole.h
        CommonSupportStream.h
        CommonSupportXML.h
)

target_link_libraries(common_support 
     INTERFACE 
        common
)
