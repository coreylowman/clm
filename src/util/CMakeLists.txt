list(APPEND CLM_UTIL_SOURCES
    array_list.c
    array_list.h
    string_util.c
    string_util.h
)

add_library(clmUtil OBJECT ${CLM_UTIL_SOURCES})
target_include_directories(clmUtil
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)
