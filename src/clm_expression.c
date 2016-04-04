#include <stdio.h>
#include <stdlib.h>

#include "string_util.h"

#include "clm_expression.h"

const char *arith_op_to_string(ArithOp op) {
  const char *strings[] = {"ADD", "SUB", "MULT", "DIV"};
  return strings[(int)op];
}

const char *bool_op_to_string(BoolOp op) {
  const char *strings[] = {"AND", "OR", "EQ", "NEQ", "GT", "LT", "GTE", "LTE"};
  return strings[(int)op];
}

const char *unary_op_to_string(UnaryOp op) {
  const char *strings[] = {"MINUS", "TRANSPOSE", "NOT"};
  return strings[(int)op];
}

ClmExpNode *clm_exp_new_int(int val) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_INT;
  node->ival = val;
  return node;
}

ClmExpNode *clm_exp_new_float(float fval) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_FLOAT;
  node->fval = fval;
  return node;
}

ClmExpNode *clm_exp_new_string(const char *str) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_STRING;
  node->str = string_copy(str);
  return node;
}

ClmExpNode *clm_exp_new_arith(ArithOp operand, ClmExpNode *right,
                              ClmExpNode *left) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_ARITH;
  node->arithExp.operand = operand;
  node->arithExp.right = right;
  node->arithExp.left = left;
  return node;
}

ClmExpNode *clm_exp_new_bool(BoolOp operand, ClmExpNode *right,
                             ClmExpNode *left) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_BOOL;
  node->boolExp.operand = operand;
  node->boolExp.right = right;
  node->boolExp.left = left;
  return node;
}

ClmExpNode *clm_exp_new_call(char *name, ArrayList *params) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_CALL;
  node->callExp.name = string_copy(name);
  node->callExp.params = params;
  return node;
}

ClmExpNode *clm_exp_new_index(const char *id, ClmExpNode *rowIndex,
                              ClmExpNode *colIndex) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_INDEX;
  node->indExp.id = string_copy(id);
  node->indExp.rowIndex = rowIndex;
  node->indExp.colIndex = colIndex;
  return node;
}

ClmExpNode *clm_exp_new_mat_dec(float *arr, int length, int cols) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_MAT_DEC;
  node->matDecExp.arr = arr;
  node->matDecExp.length = length;
  node->matDecExp.rows = length / cols;
  node->matDecExp.cols = cols;
  node->matDecExp.rowVar = NULL;
  node->matDecExp.colVar = NULL;
  return node;
}

ClmExpNode *clm_exp_new_empty_mat_dec(int rows, int cols, const char *rowVar,
                                      const char *colVar) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_MAT_DEC;
  node->matDecExp.arr = NULL;
  node->matDecExp.length = 0;
  node->matDecExp.rows = rows;
  node->matDecExp.cols = cols;
  node->matDecExp.rowVar = string_copy(rowVar);
  node->matDecExp.colVar = string_copy(colVar);
  return node;
}

ClmExpNode *clm_exp_new_param(const char *name, ClmType type, int rows,
                              int cols, const char *rowVar,
                              const char *colVar) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_PARAM;
  node->paramExp.name = string_copy(name);
  node->paramExp.type = type;
  node->paramExp.rows = rows;
  node->paramExp.cols = cols;
  node->paramExp.rowVar = string_copy(rowVar);
  node->paramExp.colVar = string_copy(colVar);
  return node;
}

ClmExpNode *clm_exp_new_unary(UnaryOp operand, ClmExpNode *node) {
  ClmExpNode *unaryNode = malloc(sizeof(*unaryNode));
  unaryNode->type = EXP_TYPE_UNARY;
  node->unaryExp.operand = operand;
  node->unaryExp.node = node;
  return unaryNode;
}

void clm_exp_free(void *data) {
  if (data == NULL)
    return;

  ClmExpNode *node = (ClmExpNode *)data;
  switch (node->type) {
  case EXP_TYPE_INT:
  case EXP_TYPE_FLOAT:
    break;
  case EXP_TYPE_STRING:
    free(node->str);
    break;
  case EXP_TYPE_ARITH:
    clm_exp_free(node->arithExp.left);
    clm_exp_free(node->arithExp.right);
    break;
  case EXP_TYPE_BOOL:
    clm_exp_free(node->boolExp.left);
    clm_exp_free(node->boolExp.right);
    break;
  case EXP_TYPE_CALL:
    free(node->callExp.name);
    array_list_free(node->callExp.params);
    break;
  case EXP_TYPE_INDEX:
    free(node->indExp.id);
    clm_exp_free(node->indExp.rowIndex);
    clm_exp_free(node->indExp.colIndex);
    break;
  case EXP_TYPE_MAT_DEC:
    free(node->matDecExp.arr);
    free(node->matDecExp.rowVar);
    free(node->matDecExp.colVar);
    break;
  case EXP_TYPE_PARAM:
    free(node->paramExp.name);
    free(node->paramExp.rowVar);
    free(node->paramExp.colVar);
    break;
  case EXP_TYPE_UNARY:
    clm_exp_free(node->unaryExp.node);
    break;
  }
  free(node);
}

void clm_exp_unbox_right(ClmExpNode *node) {
  switch (node->type) {
  case EXP_TYPE_ARITH: {
    clm_exp_free(node->arithExp.left);

    ClmExpNode *unboxed = node->arithExp.right;
    *node = *unboxed;

    free(unboxed);
    break;
  }
  case EXP_TYPE_BOOL: {
    clm_exp_free(node->boolExp.left);

    ClmExpNode *unboxed = node->boolExp.right;
    *node = *unboxed;

    free(unboxed);
    break;
  }
  default:
    break;
  }
}

void clm_exp_unbox_left(ClmExpNode *node) {
  switch (node->type) {
  case EXP_TYPE_ARITH: {
    clm_exp_free(node->arithExp.right);

    ClmExpNode *unboxed = node->arithExp.left;
    *node = *unboxed;

    free(unboxed);
    break;
  }
  case EXP_TYPE_BOOL: {
    clm_exp_free(node->boolExp.right);

    ClmExpNode *unboxed = node->boolExp.left;
    *node = *unboxed;

    free(unboxed);
    break;
  }
  default:
    break;
  }
}

void clm_exp_unbox_unary(ClmExpNode *node) {
  ClmExpNode *unboxed = node->unaryExp.node;
  *node = *unboxed;
  free(unboxed);
}

void clm_exp_print(void *data, int level) {
  ClmExpNode *node = data;
  int q = level;
  printf("\n");
  while (q-- > 0)
    printf("  ");
  printf("Expression ");
  switch (node->type) {
  case EXP_TYPE_INT:
    printf("type : int, val : %d", node->ival);
    break;
  case EXP_TYPE_FLOAT:
    printf("type : float, val : %f", node->fval);
    break;
  case EXP_TYPE_STRING:
    printf("type : string, val : %s", node->str);
    break;
  case EXP_TYPE_ARITH:
    printf("type : arith, op : %d\n", node->arithExp.operand);
    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("right:");
    clm_exp_print(node->arithExp.right, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("left:");
    clm_exp_print(node->arithExp.left, level + 2);
    break;
  case EXP_TYPE_BOOL:
    printf("type : bool, op : %d\n", node->boolExp.operand);

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("right:");
    clm_exp_print(node->boolExp.right, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("left:");
    clm_exp_print(node->boolExp.left, level + 2);
    break;
  case EXP_TYPE_CALL:
    printf("type : func call, name : %s", node->callExp.name);
    printf(", params : ");
    array_list_foreach_2(node->callExp.params, level + 2, clm_exp_print);
    break;
  case EXP_TYPE_INDEX:
    printf("type : index, id : %s\n", node->indExp.id);

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("rowIndex:");
    clm_exp_print(node->indExp.rowIndex, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("colIndex:");
    clm_exp_print(node->indExp.colIndex, level + 2);
    break;
  case EXP_TYPE_MAT_DEC:
    if (node->matDecExp.arr == NULL) {
      printf("type : mat dec, rows : %d", node->matDecExp.rows);
      printf(", cols : %d", node->matDecExp.cols);
      printf(", rowVar : %s", node->matDecExp.rowVar);
      printf(", colVar : %s", node->matDecExp.colVar);
    } else {
      int i;
      printf("type : mat dec, data : ");
      for (i = 0; i < node->matDecExp.rows * node->matDecExp.cols - 1; i++) {
        printf("%f ", node->matDecExp.arr[i]);
      }
      printf("%f", node->matDecExp.arr[i]);
      printf(", rows : %d, cols : %d", node->matDecExp.rows,
             node->matDecExp.cols);
    }
    break;
  case EXP_TYPE_PARAM:
    printf("type : param, name : %s", node->paramExp.name);
    printf(", rows : %d", node->paramExp.rows);
    printf(", cols : %d", node->paramExp.cols);
    printf(", rowVar : %s", node->paramExp.rowVar);
    printf(", colVar : %s", node->paramExp.colVar);
    break;
  case EXP_TYPE_UNARY:
    printf("type : unary, op : %d\n", node->unaryExp.operand);
    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("expression:");
    clm_exp_print(node->unaryExp.node, level + 2);
    break;
  }
}

int clm_exp_has_no_inds(ClmExpNode *node) {
  if (node->type == EXP_TYPE_INDEX) {
    return node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL;
  }
  return 0;
}
