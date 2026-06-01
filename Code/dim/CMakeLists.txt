add_library(dim 
	INTERFACE
		Configuration.h
		DimensionedValues.h
		DimLimits.h
)

target_link_libraries(dim
    INTERFACE 
		common
)