list(APPEND CLM_TESTS_SOURCES
    src/clm_test_main.c
    src/clm_test_lexer.c
)

if(${CLM_BUILD_TESTS})

add_executable(clm_tests ${CLM_TESTS_SOURCES})
target_include_directories(clm_tests
    PUBLIC include
    PUBLIC ${CLM_SOURCE_DIR}/lexer/include
)
target_link_libraries(clm_tests
    clmUtil
    clmLexer
)

endif()