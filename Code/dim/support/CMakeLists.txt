add_library(dim_support 
    STATIC
        "./source/DimSupportStream.cpp"
)

target_sources(dim_support
    PUBLIC
        DimSupportStream.h
        DimSupportXML.h
)

target_compile_definitions(dim_support PUBLIC DIM_STATIC_LIBRARY)

target_link_libraries(dim_support
    PUBLIC
        common_support
        dim
)
