#ifndef CLM_EXPRESSION_H_
#define CLM_EXPRESSION_H_

#include "clm_array_list.h"
#include "clm_type.h"

typedef enum ClmArithOp {
  ARITH_OP_ADD,
  ARITH_OP_SUB,
  ARITH_OP_MULT,
  ARITH_OP_DIV
} ClmArithOp;
typedef enum ClmBoolOp {
  BOOL_OP_AND,
  BOOL_OP_OR,
  BOOL_OP_EQ,
  BOOL_OP_NEQ,
  BOOL_OP_GT,
  BOOL_OP_LT,
  BOOL_OP_GTE,
  BOOL_OP_LTE
} ClmBoolOp;
typedef enum ClmUnaryOp {
  UNARY_OP_MINUS,
  UNARY_OP_TRANSPOSE,
  UNARY_OP_NOT
} ClmUnaryOp;

const char *clm_arith_op_to_string(ClmArithOp op);
const char *clm_bool_op_to_string(ClmBoolOp op);
const char *clm_unary_op_to_string(ClmUnaryOp op);

typedef enum {
  EXP_TYPE_INT,
  EXP_TYPE_FLOAT,
  EXP_TYPE_STRING,
  EXP_TYPE_ARITH,
  EXP_TYPE_BOOL,
  EXP_TYPE_CALL,
  EXP_TYPE_INDEX,
  EXP_TYPE_MAT_DEC,
  EXP_TYPE_PARAM,
  EXP_TYPE_UNARY
} ClmExpType;

typedef struct ClmArithExp {
  ClmArithOp operand;
  struct ClmExpNode *right;
  struct ClmExpNode *left;
} ClmArithExp;

typedef struct ClmBoolExp {
  ClmBoolOp operand;
  struct ClmExpNode *right;
  struct ClmExpNode *left;
} ClmBoolExp;

typedef struct ClmCallExp {
  char *name;
  ClmArrayList *params; // array list of ClmExpNode
} ClmCallExp;

typedef struct ClmIndexExp {
  char *id;
  struct ClmExpNode *rowIndex;
  struct ClmExpNode *colIndex;
} ClmIndexExp;

typedef struct ClmMatDecExp {
  float *arr;
  int length;
  int rows; // for constant sized matrix
  int cols;
  char *rowVar; // for variable sized matrix
  char *colVar;
} ClmMatDecExp;

typedef struct ClmParamExp {
  char *name;
  ClmType type;
  int rows;
  int cols;
  char *rowVar;
  char *colVar;
} ClmParamExp;

typedef struct ClmUnaryExp {
  ClmUnaryOp operand;
  struct ClmExpNode *node;
} ClmUnaryExp;

typedef struct ClmExpNode {
  ClmExpType type;
  union {
    int ival;   // a constant
    float fval; // a float constant
    char *str;  // a string...
    ClmArithExp *arithExp;
    ClmBoolExp *boolExp;
    ClmCallExp *callExp;
    ClmIndexExp *indExp;
    ClmMatDecExp *matDecExp;
    ClmParamExp *paramExp;
    ClmUnaryExp *unaryExp;
  };
  int lineNo;
  int colNo;
} ClmExpNode;

ClmExpNode *clm_exp_new_int(int val);
ClmExpNode *clm_exp_new_float(float fval);
ClmExpNode *clm_exp_new_string(const char *str);
ClmExpNode *clm_exp_new_arith(ClmArithOp operand, ClmExpNode *right,
                              ClmExpNode *left);
ClmExpNode *clm_exp_new_bool(ClmBoolOp operand, ClmExpNode *right,
                             ClmExpNode *left);
ClmExpNode *clm_exp_new_call(char *functionName, ClmArrayList *params);
ClmExpNode *clm_exp_new_index(const char *id, ClmExpNode *rowIndex,
                              ClmExpNode *colIndex);
ClmExpNode *clm_exp_new_mat_dec(float *arr, int length, int cols);
ClmExpNode *clm_exp_new_empty_mat_dec(int rows, int cols, const char *rowVar,
                                      const char *colVar);
ClmExpNode *clm_exp_new_param(const char *name, ClmType type, int rows,
                              int cols, const char *rowVar, const char *colVar);
ClmExpNode *clm_exp_new_unary(ClmUnaryOp operand, ClmExpNode *node);

void clm_exp_free(void *data);

void clm_exp_unbox_right(ClmExpNode *node);
void clm_exp_unbox_left(ClmExpNode *node);
void clm_exp_unbox_unary(ClmExpNode *node);

void clm_exp_print(ClmExpNode *node, int level);

int clm_exp_has_no_inds(ClmExpNode *node);

#endif