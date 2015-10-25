list(APPEND CLM_CLM_SOURCES
    src/clm_compiler.c
)

add_executable(clm ${CLM_CLM_SOURCES})
target_include_directories(clm
    PUBLIC ${CLM_SOURCE_DIR}/util/include
    PUBLIC ${CLM_SOURCE_DIR}/lexer/include
    PUBLIC ${CLM_SOURCE_DIR}/parser/include
    PUBLIC ${CLM_SOURCE_DIR}/symbolGen/include
    PUBLIC ${CLM_SOURCE_DIR}/typeCheck/include
)
target_link_libraries(clm
    clmUtil
    clmLexer
    clmParser
    clmSymbolGen
    clmTypeCheck
)