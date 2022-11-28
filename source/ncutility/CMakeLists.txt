add_library(NcUtility INTERFACE)

target_link_libraries(NcUtility
    INTERFACE
        fmt::fmt
)

target_include_directories(NcUtility
    INTERFACE
        $<INSTALL_INTERFACE:include/>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

set_target_properties(NcUtility PROPERTIES
    PUBLIC_HEADERS
        ${PROJECT_SOURCE_DIR}/ncutility/Hash.h
        ${PROJECT_SOURCE_DIR}/ncutility/NcError.h
        ${PROJECT_SOURCE_DIR}/ncutility/platform/SourceLocation.h
)

install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/ncutility
        DESTINATION include
)
