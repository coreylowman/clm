#ifndef CLM_EXPRESSION_H_
#define CLM_EXPRESSION_H_

#include "clm_type.h"

#include "util/array_list.h"

typedef enum ArithOp {
  ARITH_OP_ADD,
  ARITH_OP_SUB,
  ARITH_OP_MULT,
  ARITH_OP_DIV
} ArithOp;

typedef enum BoolOp {
  BOOL_OP_AND,
  BOOL_OP_OR,
  BOOL_OP_EQ,
  BOOL_OP_NEQ,
  BOOL_OP_GT,
  BOOL_OP_LT,
  BOOL_OP_GTE,
  BOOL_OP_LTE
} BoolOp;

typedef enum UnaryOp {
  UNARY_OP_MINUS,
  UNARY_OP_TRANSPOSE,
  UNARY_OP_NOT
} UnaryOp;

const char *arith_op_to_string(ArithOp op);
const char *bool_op_to_string(BoolOp op);
const char *unary_op_to_string(UnaryOp op);

typedef enum ExpType {
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
} ExpType;

typedef struct ClmExpNode {
  ExpType type;

  union {
    int ival;

    float fval;

    char *str;

    struct {
      ArithOp operand;
      struct ClmExpNode *right;
      struct ClmExpNode *left;
    } arithExp;

    struct {
      BoolOp operand;
      struct ClmExpNode *right;
      struct ClmExpNode *left;
    } boolExp;

    struct {
      char *name;
      ArrayList *params; // array list of ClmExpNode
    } callExp;

    struct {
      char *id;
      struct ClmExpNode *rowIndex;
      struct ClmExpNode *colIndex;
    } indExp;

    struct {
      float *arr;
      int length;
      int rows; // for constant sized matrix
      int cols;
      char *rowVar; // for variable sized matrix
      char *colVar;
    } matDecExp;

    struct {
      char *name;
      ClmType type;
      int rows;
      int cols;
      char *rowVar;
      char *colVar;
    } paramExp;

    struct {
      UnaryOp operand;
      struct ClmExpNode *node;
    } unaryExp;
  };

  int lineNo;
  int colNo;
} ClmExpNode;

ClmExpNode *clm_exp_new_int(int val);
ClmExpNode *clm_exp_new_float(float fval);
ClmExpNode *clm_exp_new_string(const char *str);
ClmExpNode *clm_exp_new_arith(ArithOp operand, ClmExpNode *right,
                              ClmExpNode *left);
ClmExpNode *clm_exp_new_bool(BoolOp operand, ClmExpNode *right,
                             ClmExpNode *left);
ClmExpNode *clm_exp_new_call(char *functionName, ArrayList *params);
ClmExpNode *clm_exp_new_index(const char *id, ClmExpNode *rowIndex,
                              ClmExpNode *colIndex);
ClmExpNode *clm_exp_new_mat_dec(float *arr, int length, int cols);
ClmExpNode *clm_exp_new_empty_mat_dec(int rows, int cols, const char *rowVar,
                                      const char *colVar);
ClmExpNode *clm_exp_new_param(const char *name, ClmType type, int rows,
                              int cols, const char *rowVar, const char *colVar);
ClmExpNode *clm_exp_new_unary(UnaryOp operand, ClmExpNode *node);

void clm_exp_free(void *data);

void clm_exp_unbox_right(ClmExpNode *node);
void clm_exp_unbox_left(ClmExpNode *node);
void clm_exp_unbox_unary(ClmExpNode *node);

void clm_exp_print(void *data, int level);

int clm_exp_has_no_inds(ClmExpNode *node);

#endif
