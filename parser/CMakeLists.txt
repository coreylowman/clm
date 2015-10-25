list(APPEND CLM_PARSER_SOURCES
    src/clm_parser.c
)

add_library(clmParser STATIC ${CLM_PARSER_SOURCES})
target_include_directories(clmParser
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/util/include
    PUBLIC ${CLM_SOURCE_DIR}/lexer/include
)
target_link_libraries(clmParser
    clmUtil
    clmLexer
)