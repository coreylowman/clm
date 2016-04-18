list(APPEND CLM_OBJECT_LIBRARY_SOURCES
    clm.c
    clm.h
    clm_asm.c
    clm_asm.h
    clm_code_gen.c
    clm_expression.c
    clm_expression.h
    clm_lexer.c
    clm_optimizer.c
    clm_parser.c
    clm_scope.c
    clm_scope.h
    clm_statement.c
    clm_statement.h
    clm_symbol_gen.c
    clm_type.c
    clm_type.h
    clm_type_check.c
    clm_type_gen.c
    clm_type_gen.h
    clm_type_of.c
    clm_type_of.h
)

add_library(clmObjectLibrary OBJECT ${CLM_OBJECT_LIBRARY_SOURCES})

list(APPEND CLM_SOURCES
    main.c

    $<TARGET_OBJECTS:clmObjectLibrary>
)

add_executable(clm ${CLM_SOURCES})
