#ifndef CLM_TEST_UTILS_H_
#define CLM_TEST_UTILS_H_

#define CLM_ASSERT(x)                                                          \
  do {                                                                         \
    if (!(x)) {                                                                \
      printf("Assertion : " #x " FAILED\n");                                   \
      return 0;                                                                \
    }                                                                          \
  } while (0)

#endif