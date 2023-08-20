add_library(lz4 STATIC)

target_sources(lz4
    PRIVATE
        lz4.c
        lz4hc.c
)

target_include_directories(lz4
    PUBLIC
        ${PROJECT_SOURCE_DIR}/source/external
)
