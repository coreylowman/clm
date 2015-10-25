list(APPEND CLM_SYMBOL_GEN_SOURCES
    src/clm_symbol_gen.c
)

add_library(clmSymbolGen STATIC ${CLM_SYMBOL_GEN_SOURCES})
target_include_directories(clmSymbolGen
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/util/include
)
target_link_libraries(clmSymbolGen
    clmUtil
)