list(APPEND CLM_TESTS_SOURCES
    clm_test_code_gen.c
    clm_test_lexer.c
    clm_test_optimizer.c
    clm_test_parser.c
    clm_test_symbol_gen.c
    clm_test_type_check.c
    clm_tests.h
    main.c

    $<TARGET_OBJECTS:clmObjectLibrary>
)

add_executable(clm_tests ${CLM_TESTS_SOURCES})
target_include_directories(clm_tests
    PUBLIC ${CLM_SOURCE_DIR}/src
)
