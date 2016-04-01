#include "tests/clm_test_lexer.h"

char *fileName;
int CLM_BUILD_TESTS = 1;

int main(int argc, char *argv[]) {
  int res;
  res = clm_test_lexer();
  printf("LEXER : %s\n", res ? "PASSED" : "FAILED");

  return 0;
}