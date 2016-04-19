list(APPEND CLM_SOURCES
    array_list.c
    array_list.h
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
    clm_type_gen.c
    clm_type_gen.h
    string_util.c
    string_util.h
)

add_library(clmObjectLibrary OBJECT ${CLM_SOURCES})

add_executable(clm clm.c ${CLM_SOURCES})
