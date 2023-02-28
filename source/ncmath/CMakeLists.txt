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

# Allow DirectXMath to find Microsoft annotations (sal.h)
if(NOT MSVC)
    target_include_directories(NcMath
        PUBLIC
            $<INSTALL_INTERFACE:include/platform/unix>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/platform/unix>
    )
endif()

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

if(NOT MSVC)
    install(DIRECTORY   ${PROJECT_SOURCE_DIR}/external/platform
            DESTINATION include
    )
endif()