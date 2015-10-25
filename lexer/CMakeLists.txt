list(APPEND CLM_LEXER_SOURCES
    src/clm_lexer.c
)

add_library(clmLexer STATIC ${CLM_LEXER_SOURCES})
target_include_directories(clmLexer
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/util/include
)
target_link_libraries(clmLexer
    clmUtil
)