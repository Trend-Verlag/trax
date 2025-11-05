add_executable(app_eepfileconverter
    EEPFileConverter.cpp
)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_LIB_PREFIX "lib")  # Force looking for lib* prefix

# Find Boost once (add components here as project grows)
cmake_policy(SET CMP0167 OLD)
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS program_options)

target_link_libraries(app_eepfileconverter 
    PRIVATE
        Boost::program_options
        trax_core_collections_support
        trax_modules_support
)
