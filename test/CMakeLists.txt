# Fetch gtest
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

enable_testing()

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/NcStudios/googletest.git
                     GIT_TAG        origin/main
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_subdirectory(math)
add_subdirectory(utility)
