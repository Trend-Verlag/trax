add_library(spat INTERFACE)

file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS *.h *.hpp)

set(_COMMON_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(spat
    INTERFACE
        $<BUILD_INTERFACE:${_COMMON_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(spat INTERFACE cxx_std_17)

target_sources(spat
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${_COMMON_PARENT_DIR}
        FILES ${COMMON_HEADERS}
)
