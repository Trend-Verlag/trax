add_library(dim 
	INTERFACE
		DimensionedValues.h
		DimLimits.h
)

target_link_libraries(dim
    INTERFACE 
		common
)