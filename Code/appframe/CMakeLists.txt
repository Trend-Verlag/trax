add_library(appframe 
    STATIC
        "./source/Command_Imp.cpp"
)

target_sources(appframe
    PUBLIC
        "AppFrame.h"
        "Command.h"
)

target_compile_definitions(appframe PUBLIC APPFRAME_STATIC_LIBRARY)

target_link_libraries(appframe
    PUBLIC
        project_settings
)
