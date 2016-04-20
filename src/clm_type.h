#ifndef CLM_TYPE_H_
#define CLM_TYPE_H_

typedef struct ClmExpNode ClmExpNode;
typedef struct ClmScope ClmScope;

typedef enum ClmType {
  CLM_TYPE_INT,
  CLM_TYPE_MATRIX,
  CLM_TYPE_STRING,
  CLM_TYPE_FLOAT,
  CLM_TYPE_FUNCTION,
  CLM_TYPE_NONE
} ClmType;

const char *clm_type_to_string(ClmType type);
ClmType clm_type_of_ind(ClmExpNode *node, ClmScope *scope);
ClmType clm_type_of_exp(ClmExpNode *node, ClmScope *scope);
int clm_type_is_number(ClmType type);
int clm_exp_has_size(ClmExpNode *node, ClmScope *scope);
void clm_size_of_exp(ClmExpNode *node, ClmScope *scope, int *out_rows,
                     int *out_cols);

#endif
