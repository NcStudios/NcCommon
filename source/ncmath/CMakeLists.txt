add_library(NcMath STATIC)

target_compile_options(NcMath
    PRIVATE
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(NcMath
    PUBLIC
        DirectXMath
        fmt::fmt
)

target_include_directories(NcMath
    PUBLIC
        $<INSTALL_INTERFACE:include/>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_sources(NcMath
    PRIVATE
        Quaternion.cpp
)

install(TARGETS     NcMath
        DESTINATION lib
)

install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/ncmath
        DESTINATION include
)
