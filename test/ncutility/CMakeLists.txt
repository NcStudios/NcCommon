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
