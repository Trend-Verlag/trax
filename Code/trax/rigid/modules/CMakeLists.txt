add_library(trax_modules
    STATIC
        "./source/Module_Imp.cpp"
        "./source/ModuleCollection_Imp.cpp"
)

set(PUBLIC_HEADERS
    "./Module.h"
    "./ModuleCollection.h"
)

set(PRIVATE_HEADERS
    "./source/Module_Imp.h"
    "./source/ModuleCollection_Imp.h"
)

target_sources(trax_modules
    PUBLIC
        ${PUBLIC_HEADERS}
    PRIVATE
        ${PRIVATE_HEADERS}
)

# Create source groups (filters) for Visual Studio
source_group("Interface" FILES ${PUBLIC_HEADERS})
source_group("Header Files" FILES ${PRIVATE_HEADERS})

target_link_libraries(trax_modules
    PUBLIC
        trax_rigid_collections
        trax_trains_collections
)