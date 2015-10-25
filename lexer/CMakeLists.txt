list(APPEND CLM_LEXER_SOURCES
    src/clm_lexer.c
)

add_library(clmLexer OBJECT ${CLM_LEXER_SOURCES})
target_include_directories(clmLexer
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/util/include
)