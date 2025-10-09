add_library(trax_modules
    STATIC
        "./source/Module_Imp.cpp"
        "./source/ModuleSet_Imp.cpp"
)

target_sources(trax_modules
    PUBLIC
        "./Module.h"
        "./ModuleSet.h"
    PRIVATE
        "./source/Module_Imp.h"
        "./source/ModuleSet_Imp.h"
)

target_link_libraries(trax_modules
    PUBLIC
        trax_rigid_collections
        trax_trains_collections
)