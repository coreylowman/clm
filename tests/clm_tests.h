#ifndef CLM_TESTS_H_
#define CLM_TESTS_H_

#define CLM_ASSERT(x)                                                          \
  do {                                                                         \
    if (!(x)) {                                                                \
      printf("Assertion : " #x " FAILED\n");                                   \
      return 0;                                                                \
    }                                                                          \
  } while (0)

int clm_test_lexer();
int clm_test_parser();
int clm_test_symbol_gen();
int clm_test_type_check();
int clm_test_optimizer();
int clm_test_code_gen();

#endif
