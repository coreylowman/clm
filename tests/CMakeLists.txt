list(APPEND CLM_TESTS_SOURCES
    src/clm_test_main.c
    src/clm_test_lexer.c
    
    include/tests/clm_test_main.h
    include/tests/clm_test_lexer.h

    $<TARGET_OBJECTS:clmUtil>
    $<TARGET_OBJECTS:clmLexer>
)

if(${CLM_BUILD_TESTS})

add_executable(clm_tests ${CLM_TESTS_SOURCES})
target_include_directories(clm_tests
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/lexer/include
)

endif()