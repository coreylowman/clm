list(APPEND CLM_UTIL_SOURCES
    clm_array_list.c
    clm_array_list.h
    clm_error.c
    clm_error.h
    clm_expression.c
    clm_expression.h
    clm_file.c
    clm_file.h
    clm_scope.c
    clm_scope.h
    clm_statement.c
    clm_statement.h
    clm_string.c
    clm_string.h
    clm_symbol.c
    clm_symbol.h
    clm_type.c
    clm_type.h
    clm_type_of.c
    clm_type_of.h
)

add_library(clmUtil OBJECT ${CLM_UTIL_SOURCES})
target_include_directories(clmUtil
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)