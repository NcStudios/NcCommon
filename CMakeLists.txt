cmake_minimum_required(VERSION 3.20)
project("NcCommon"
        VERSION "1.0.0"
        LANGUAGES CXX
)

# Options
option(NC_COMMON_BUILD_TESTS "Enable building tests." OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NC_COMMON_COMPILE_OPTIONS -Werror
                                   -Wall
                                   -Wextra
                                   -Wfatal-errors
                                   -Wno-cast-function-type
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NC_COMMON_COMPILE_OPTIONS /W3
                                  /WX
                                  /MP
    )
endif()


include(FetchContent)

FetchContent_Declare(DirectXMath
                     GIT_REPOSITORY https://github.com/NcStudios/DirectXMath.git
                     GIT_TAG        4533e8faeb4f9d251a87ca3f67b863ae25196d2c # origin/main
)

# We only need {fmt} because GCC hasn't implemented std::format yet. Can be removed eventually.
set(FMT_INSTALL ON)
FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                     GIT_TAG        fc07217d85e6dcec52878807d6bbd89a9d9156a5
)

FetchContent_MakeAvailable(DirectXMath fmt)

add_library(NcCommon STATIC)

target_compile_options(NcCommon
    PRIVATE
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(NcCommon
    PUBLIC
        DirectXMath
        fmt::fmt
)

target_include_directories(NcCommon
    PUBLIC
        $<INSTALL_INTERFACE:include/>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

add_subdirectory(source)

if(NC_COMMON_BUILD_TESTS)
    add_subdirectory(test)
endif()

install(DIRECTORY   ${PROJECT_SOURCE_DIR}/include/
        DESTINATION include
)
