list(APPEND CLM_TESTS_SOURCES
    clm_test_lexer.c
    clm_test_lexer.h
    clm_test_main.c
    clm_test_main.h
    
    $<TARGET_OBJECTS:clmUtil>
    #TODO... will need to link against lexer et al. here
)

if(${CLM_BUILD_TESTS})
    add_executable(clm_tests ${CLM_TESTS_SOURCES})
    target_include_directories(clm_tests
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}
        PUBLIC ${CLM_SOURCE_DIR}/src/clm
        PUBLIC ${CLM_SOURCE_DIR}/src/include
    )
endif()