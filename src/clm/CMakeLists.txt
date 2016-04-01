list(APPEND CLM_SOURCES
    clm.c
    clm_asm.h
    clm_asm.c
    clm_code_gen.c
    clm_code_gen.h
    clm_lexer.c
    clm_lexer.h
    clm_optimizer.c
    clm_optimizer.h
    clm_parser.c
    clm_parser.h
    clm_symbol_gen.c
    clm_symbol_gen.h
    clm_type_check.c
    clm_type_check.h

    $<TARGET_OBJECTS:clmUtil>
)

add_executable(clm ${CLM_SOURCES})
target_include_directories(clm
    PUBLIC ${CLM_SOURCE_DIR}/src/clm
    PUBLIC ${CLM_SOURCE_DIR}/src #util files
)