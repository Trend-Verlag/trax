add_library(common 
    INTERFACE
        Helpers.h
        Interval.h
        NarrowCast.h
        RedirectStandardOutput_Base.h
)

target_link_libraries(common
    INTERFACE 
        project_settings
)
