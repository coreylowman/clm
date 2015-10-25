list(APPEND CLM_TYPE_CHECK_SOURCES
    src/clm_type_check.c
)

add_library(clmTypeCheck OBJECT ${CLM_TYPE_CHECK_SOURCES})
target_include_directories(clmTypeCheck
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/util/include
)