list(APPEND CLM_UTIL_SOURCES
    src/clm_array_list.c
    src/clm_error.c
    src/clm_expression.c
    src/clm_file.c
    src/clm_scope.c
    src/clm_statement.c
    src/clm_string.c
    src/clm_symbol.c
    src/clm_type_of.c
    src/clm_type.c
)

add_library(clmUtil OBJECT ${CLM_UTIL_SOURCES})
target_include_directories(clmUtil
    PUBLIC include
)