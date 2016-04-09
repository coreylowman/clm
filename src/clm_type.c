#include "clm_type.h"

const char *clm_type_to_string(ClmType type) {
  const char *strings[] = {
      "INT", "MATRIX", "MATRIX_POINTER", "STRING", "FLOAT", "FUNCTION", "NONE"};
  return strings[(int)type];
}
