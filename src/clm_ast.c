#include <stdio.h>
#include <stdlib.h>

#include "clm.h"
#include "clm_ast.h"

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

ClmStmtNode *clm_stmt_new_assign(ClmExpNode *lhs, ClmExpNode *rhs) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_ASSIGN;
  node->assignStmt.lhs = lhs;
  node->assignStmt.rhs = rhs;
  return node;
}

ClmStmtNode *clm_stmt_new_call(ClmExpNode *callExpr) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_CALL;
  node->callExpr = callExpr;
  return node;
}

ClmStmtNode *clm_stmt_new_cond(ClmExpNode *condition, ArrayList *trueBody,
                               ArrayList *falseBody) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_CONDITIONAL;
  node->conditionStmt.condition = condition;
  node->conditionStmt.trueBody = trueBody;
  node->conditionStmt.falseBody = falseBody;
  return node;
}

ClmStmtNode *clm_stmt_new_dec(char *name, ArrayList *params, ClmType returnType,
                              int returnRows, char *returnRowsVars,
                              int returnCols, char *returnColsVar,
                              ArrayList *body) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_FUNC_DEC;
  node->funcDecStmt.name = string_copy(name);
  node->funcDecStmt.parameters = params;
  node->funcDecStmt.returnType = returnType;
  node->funcDecStmt.returnRows = returnRows;
  node->funcDecStmt.returnRowsVar = string_copy(returnRowsVars);
  node->funcDecStmt.returnCols = returnCols;
  node->funcDecStmt.returnColsVar = string_copy(returnColsVar);
  node->funcDecStmt.body = body;
  return node;
}

ClmStmtNode *clm_stmt_new_loop(char *varId, ClmExpNode *start, ClmExpNode *end,
                               ClmExpNode *delta, ArrayList *body,
                               int startInclusive, int endInclusive) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_LOOP;
  node->loopStmt.varId = string_copy(varId);
  node->loopStmt.start = start;
  node->loopStmt.end = end;
  node->loopStmt.delta = delta;
  node->loopStmt.body = body;
  node->loopStmt.startInclusive = startInclusive;
  node->loopStmt.endInclusive = endInclusive;
  return node;
}

ClmStmtNode *clm_stmt_new_print(ClmExpNode *expression, int appendNewline) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_PRINT;
  node->printStmt.expression = expression;
  node->printStmt.appendNewline = appendNewline;
  return node;
}

ClmStmtNode *clm_stmt_new_return(ClmExpNode *returnExpr) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_RET;
  node->returnExpr = returnExpr;
  return node;
}

void clm_stmt_free(void *data) {
  if (data == NULL)
    return;

  ClmStmtNode *node = (ClmStmtNode *)data;
  switch (node->type) {
  case STMT_TYPE_ASSIGN:
    clm_exp_free(node->assignStmt.lhs);
    clm_exp_free(node->assignStmt.rhs);
    break;
  case STMT_TYPE_CALL:
    clm_exp_free(node->callExpr);
    break;
  case STMT_TYPE_CONDITIONAL:
    clm_exp_free(node->conditionStmt.condition);
    array_list_free(node->conditionStmt.trueBody);
    array_list_free(node->conditionStmt.falseBody);
    break;
  case STMT_TYPE_FUNC_DEC:
    free(node->funcDecStmt.name);
    array_list_free(node->funcDecStmt.parameters);
    array_list_free(node->funcDecStmt.body);
    break;
  case STMT_TYPE_LOOP:
    free(node->loopStmt.varId);
    clm_exp_free(node->loopStmt.start);
    clm_exp_free(node->loopStmt.end);
    clm_exp_free(node->loopStmt.delta);
    array_list_free(node->loopStmt.body);
    break;
  case STMT_TYPE_PRINT:
    clm_exp_free(node->printStmt.expression);
    break;
  case STMT_TYPE_RET:
    clm_exp_free(node->returnExpr);
    break;
  }
  free(node);
}

void clm_stmt_print(void *data, int level) {
  ClmStmtNode *node = data;
  int q = level;
  printf("\n");
  while (q-- > 0)
    printf("  ");
  printf("Statement ");
  switch (node->type) {
  case STMT_TYPE_ASSIGN:
    printf("type : assign\n");

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("lhs:");
    clm_exp_print(node->assignStmt.lhs, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("rhs:");
    clm_exp_print(node->assignStmt.rhs, level + 2);
    break;
  case STMT_TYPE_CALL:
    printf("type : func call\n");

    int q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("expression: ");
    clm_exp_print(node->callExpr, level + 2);
    break;
  case STMT_TYPE_CONDITIONAL:
    printf("type : conditional\n");

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("condition:");
    clm_exp_print(node->conditionStmt.condition, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("truebody:");
    array_list_foreach_2(node->conditionStmt.trueBody, level + 2,
                         clm_stmt_print);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("falsebody:");
    array_list_foreach_2(node->conditionStmt.falseBody, level + 2,
                         clm_stmt_print);
    break;
  case STMT_TYPE_FUNC_DEC:
    printf("type : func dec, name : %s\n", node->funcDecStmt.name);

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("return type : %d, ", node->funcDecStmt.returnType);
    printf("ret rows : %d | %s, ", node->funcDecStmt.returnRows,
           node->funcDecStmt.returnRowsVar);
    printf("ret cols : %d | %s", node->funcDecStmt.returnCols,
           node->funcDecStmt.returnColsVar);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("parameters:");
    array_list_foreach_2(node->funcDecStmt.parameters, level + 2,
                         clm_exp_print);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("body:");
    array_list_foreach_2(node->funcDecStmt.body, level + 2, clm_stmt_print);
    break;
  case STMT_TYPE_LOOP:
    printf("type : loop, var : %s\n", node->loopStmt.varId);

    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("start:");
    clm_exp_print(node->loopStmt.start, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("delta:");
    clm_exp_print(node->loopStmt.delta, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("end:");
    clm_exp_print(node->loopStmt.end, level + 2);

    q = level + 1;
    printf("\n");
    while (q-- > 0)
      printf("  ");
    printf("body:");
    array_list_foreach_2(node->loopStmt.body, level + 2, clm_stmt_print);
    break;
  case STMT_TYPE_PRINT:
    printf("type : print, newline : %d\n", node->printStmt.appendNewline);
    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("expression:");
    clm_exp_print(node->printStmt.expression, level + 2);
    break;
  case STMT_TYPE_RET:
    printf("type : return\n");
    q = level + 1;
    while (q-- > 0)
      printf("  ");
    printf("expression: ");
    clm_exp_print(node->returnExpr, level + 2);
    break;
  }
}
