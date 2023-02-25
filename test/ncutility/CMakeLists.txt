### Algorithm Tests ###
add_executable(Algorithm_unit_tests
    Algorithm_unit_test.cpp
)

target_include_directories(Algorithm_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(Algorithm_unit_tests
    PUBLIC
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(Algorithm_unit_tests
    PRIVATE
        gtest_main
)

add_test(Algorithm_unit_tests Algorithm_unit_tests)

### ScopeExit Tests ###
add_executable(ScopeExit_unit_tests
    ScopeExit_unit_test.cpp
)

target_include_directories(ScopeExit_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(ScopeExit_unit_tests
    PUBLIC
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(ScopeExit_unit_tests
    PRIVATE
        gtest_main
)

add_test(ScopeExit_unit_tests ScopeExit_unit_tests)

### StringHash Tests ###
set(NC_HASH_TEST_COLLATERAL_DIRECTORY ${PROJECT_SOURCE_DIR}/test/utility/collateral/)

add_executable(StringHash_unit_tests
    StringHash_unit_test.cpp
)

target_compile_definitions(StringHash_unit_tests
    PRIVATE
        NC_HASH_TEST_COLLATERAL_DIRECTORY="${NC_HASH_TEST_COLLATERAL_DIRECTORY}"
)

target_include_directories(StringHash_unit_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(StringHash_unit_tests
    PUBLIC
        ${NC_COMMON_COMPILE_OPTIONS}
)

target_link_libraries(StringHash_unit_tests
    PRIVATE
        gtest
)

add_test(StringHash_unit_tests StringHash_unit_tests)
