add_library(NcUtility STATIC)

target_sources(NcUtility
    PRIVATE
        Compression.cpp
        $<TARGET_OBJECTS:lz4>
)

target_compile_options(NcUtility
    PRIVATE
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(NcUtility
    PUBLIC
        fmt::fmt
)

target_include_directories(NcUtility
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source/external>
)

install(TARGETS     NcUtility
        DESTINATION lib
)

install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/ncutility
        DESTINATION include
)

install(DIRECTORY   ${PROJECT_SOURCE_DIR}/source/external/nlohmann
        DESTINATION include
)
