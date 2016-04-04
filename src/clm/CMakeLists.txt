list(APPEND CLM_SOURCES
    clm.c
    clm_asm.c
    clm_asm.h
    clm_code_gen.c
    clm_code_gen.h
    clm_error.c
    clm_error.h
    clm_expression.c
    clm_expression.h
    clm_lexer.c
    clm_lexer.h
    clm_optimizer.c
    clm_optimizer.h
    clm_parser.c
    clm_parser.h
    clm_scope.c
    clm_scope.h
    clm_statement.c
    clm_statement.h
    clm_symbol.c
    clm_symbol.h
    clm_symbol_gen.c
    clm_symbol_gen.h
    clm_type.c
    clm_type.h
    clm_type_check.c
    clm_type_check.h
    clm_type_of.c
    clm_type_of.h
    clm_type_gen.h
    clm_type_gen.c

    $<TARGET_OBJECTS:clmUtil>
)

add_executable(clm ${CLM_SOURCES})
target_include_directories(clm
    PUBLIC ${CLM_SOURCE_DIR}/src/clm
    PUBLIC ${CLM_SOURCE_DIR}/src #util files
)
