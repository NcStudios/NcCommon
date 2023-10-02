cmake_minimum_required(VERSION 3.20)
project("NcCommon"
        VERSION "1.0.1"
        LANGUAGES CXX C
)

option(NC_COMMON_BUILD_TESTS "Enable building tests." OFF)
option(NC_COMMON_STATIC_ANALYSIS "Enable static analysis (MSVC Only)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(NC_COMMON_COMPILE_OPTIONS
        -Werror
        -Wall
        -Wextra
        -Wshadow
        -Wconversion
        -Wfatal-errors
        -Wno-cast-function-type
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(NC_COMMON_COMPILE_OPTIONS
        /W4
        /WX
        /MP
    )
    if(${NC_COMMON_STATIC_ANALYSIS})
        set(NC_COMMON_COMPILE_OPTIONS
            ${NC_COMMON_COMPILE_OPTIONS}
            /analyze
            /analyze:external-
            /external:I${PROJECT_BINARY_DIR}/_deps
            /wd6326 # Suppress 'potential comparison of constants'
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(NC_COMMON_COMPILE_OPTIONS
        -Werror
        -Wall
        -Wextra
        -Wshadow
        -Wconversion
        -fexperimental-library
    )
endif()

include(FetchContent)

FetchContent_Declare(DirectXMath
                     GIT_REPOSITORY https://github.com/NcStudios/DirectXMath.git
                     GIT_TAG        21b8256207a68d413c517f97391f8354e5211692 # origin/release
                     GIT_SHALLOW    TRUE
)

# We only need {fmt} because GCC hasn't implemented std::format yet. Can be removed eventually.
set(FMT_INSTALL ON)
FetchContent_Declare(fmt
                     GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                     GIT_TAG        10.1.1
                     GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(DirectXMath fmt)

# Silence warnings from DirectXMath
get_target_property(_DirectXMath_Include_Prop DirectXMath INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(DirectXMath SYSTEM INTERFACE ${_DirectXMath_Include_Prop})

# Silence warnings from fmt
get_target_property(_fmt_Include_Prop fmt INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(fmt SYSTEM INTERFACE ${_fmt_Include_Prop})

add_subdirectory(source)

if(NC_COMMON_BUILD_TESTS)
    add_subdirectory(test)
endif()
