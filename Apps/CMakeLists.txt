add_executable(app_eepfileconverter
    EEPFileConverter.cpp
)

target_link_libraries(app_eepfileconverter 
    PRIVATE
        trax_core_collections_support
        trax_modules_support
)
