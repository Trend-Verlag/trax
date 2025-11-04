add_library(trax_modules_support 
    STATIC
        "./source/Anl3ModuleReader.cpp"
        "./source/Anl4ModuleReader.cpp"
        "./source/Anl4ModuleWriter.cpp"
        "./source/AnlReader.cpp"
        "./source/AnlWriter.cpp"
)

set(PUBLIC_HEADERS
    "./Anl3ModuleReader.h"
    "./Anl4ModuleReader.h"
    "./Anl4ModuleWriter.h"
    "./AnlReader.h"
    "./AnlWriter.h"
)

target_sources(trax_modules_support
    PUBLIC
        ${PUBLIC_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})

target_link_libraries(trax_modules_support
     PUBLIC
        trax_core_support
        trax_modules
)

target_include_directories(trax_modules_support PRIVATE ${Boost_INCLUDE_DIRS})
