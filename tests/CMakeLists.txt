list(APPEND CLM_TESTS_SOURCES
    clm_test_lexer.c
    clm_test_lexer.h
    clm_test_main.c
    clm_test_main.h

    $<TARGET_OBJECTS:clmObjectLibrary>
)

add_executable(clm_tests ${CLM_TESTS_SOURCES})
target_include_directories(clm_tests
    PUBLIC ${CLM_SOURCE_DIR}/src
)
