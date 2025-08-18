add_library(common INTERFACE)

file(GLOB_RECURSE COMMON_HEADERS CONFIGURE_DEPENDS *.h *.hpp)

set(_COMMON_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(common
    INTERFACE
        $<BUILD_INTERFACE:${_COMMON_PARENT_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(common INTERFACE cxx_std_17)

target_sources(common
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS ${_COMMON_PARENT_DIR}
        FILES ${COMMON_HEADERS}
)

# Always add tests; they are just normal executables now
add_subdirectory(tests)