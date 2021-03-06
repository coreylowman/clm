#include <stdio.h>
#include <stdlib.h>

#include "clm.h"
#include "clm_ast.h"

static void insert_whitespace(int level){
  while(level-- > 0){
    printf("  ");
  }
}

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
  node->matDecExp.size.rows = length / cols;
  node->matDecExp.size.cols = cols;
  node->matDecExp.size.rowVar = NULL;
  node->matDecExp.size.colVar = NULL;
  return node;
}

ClmExpNode *clm_exp_new_empty_mat_dec(int rows, int cols, const char *rowVar,
                                      const char *colVar) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_MAT_DEC;
  node->matDecExp.arr = NULL;
  node->matDecExp.length = 0;
  node->matDecExp.size.rows = rows;
  node->matDecExp.size.cols = cols;
  node->matDecExp.size.rowVar = string_copy(rowVar);
  node->matDecExp.size.colVar = string_copy(colVar);
  return node;
}

ClmExpNode *clm_exp_new_param(const char *name, ClmType type, int rows,
                              int cols, const char *rowVar,
                              const char *colVar) {
  ClmExpNode *node = malloc(sizeof(*node));
  node->type = EXP_TYPE_PARAM;
  node->paramExp.name = string_copy(name);
  node->paramExp.type = type;
  node->paramExp.size.rows = rows;
  node->paramExp.size.cols = cols;
  node->paramExp.size.rowVar = string_copy(rowVar);
  node->paramExp.size.colVar = string_copy(colVar);
  return node;
}

ClmExpNode *clm_exp_new_unary(UnaryOp operand, ClmExpNode *node) {
  ClmExpNode *unaryNode = malloc(sizeof(*unaryNode));
  unaryNode->type = EXP_TYPE_UNARY;
  node->unaryExp.operand = operand;
  node->unaryExp.node = node;
  return unaryNode;
}

static void matrix_size_free(MatrixSize size) {
  free(size.rowVar);
  free(size.colVar);
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
    matrix_size_free(node->matDecExp.size);
    break;
  case EXP_TYPE_PARAM:
    free(node->paramExp.name);
    matrix_size_free(node->paramExp.size);
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
  insert_whitespace(level);
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
    insert_whitespace(level + 1);
    printf("right:");
    clm_exp_print(node->arithExp.right, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("left:");
    clm_exp_print(node->arithExp.left, level + 2);
    break;
  case EXP_TYPE_BOOL:
    printf("type : bool, op : %d\n", node->boolExp.operand);

    insert_whitespace(level + 1);
    printf("right:");
    clm_exp_print(node->boolExp.right, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
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

    insert_whitespace(level + 1);
    printf("rowIndex:");
    clm_exp_print(node->indExp.rowIndex, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("colIndex:");
    clm_exp_print(node->indExp.colIndex, level + 2);
    break;
  case EXP_TYPE_MAT_DEC:
    if (node->matDecExp.arr == NULL) {
      printf("type : mat dec, rows : %d", node->matDecExp.size.rows);
      printf(", cols : %d", node->matDecExp.size.cols);
      printf(", rowVar : %s", node->matDecExp.size.rowVar);
      printf(", colVar : %s", node->matDecExp.size.colVar);
    } else {
      int i;
      printf("type : mat dec, data : ");
      for (i = 0; i < node->matDecExp.size.rows * node->matDecExp.size.cols - 1;
           i++) {
        printf("%f ", node->matDecExp.arr[i]);
      }
      printf("%f", node->matDecExp.arr[i]);
      printf(", rows : %d, cols : %d", node->matDecExp.size.rows,
             node->matDecExp.size.cols);
    }
    break;
  case EXP_TYPE_PARAM:
    printf("type : param, name : %s", node->paramExp.name);
    printf(", rows : %d", node->paramExp.size.rows);
    printf(", cols : %d", node->paramExp.size.cols);
    printf(", rowVar : %s", node->paramExp.size.rowVar);
    printf(", colVar : %s", node->paramExp.size.colVar);
    break;
  case EXP_TYPE_UNARY:
    printf("type : unary, op : %d\n", node->unaryExp.operand);
    
    insert_whitespace(level + 1);
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
                              int returnRows, int returnCols,
                              char *returnRowsVars, char *returnColsVar,
                              ArrayList *body) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_FUNC_DEC;
  node->funcDecStmt.name = string_copy(name);
  node->funcDecStmt.parameters = params;
  node->funcDecStmt.returnType = returnType;
  node->funcDecStmt.returnSize.rows = returnRows;
  node->funcDecStmt.returnSize.cols = returnCols;
  node->funcDecStmt.returnSize.rowVar = string_copy(returnRowsVars);
  node->funcDecStmt.returnSize.colVar = string_copy(returnColsVar);
  node->funcDecStmt.body = body;
  return node;
}

ClmStmtNode *clm_stmt_new_for_loop(char *varId, ClmExpNode *start, ClmExpNode *end,
                               ClmExpNode *delta, ArrayList *body) {
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_FOR_LOOP;
  node->forLoopStmt.varId = string_copy(varId);
  node->forLoopStmt.start = start;
  node->forLoopStmt.end = end;
  node->forLoopStmt.delta = delta;
  node->forLoopStmt.body = body;
  return node;
}

ClmStmtNode *clm_stmt_new_while_loop(ClmExpNode *condition, ArrayList *loopBody){
  ClmStmtNode *node = malloc(sizeof(*node));
  node->type = STMT_TYPE_WHILE_LOOP;
  node->whileLoopStmt.condition = condition;
  node->whileLoopStmt.body = loopBody;
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
    matrix_size_free(node->funcDecStmt.returnSize);
    array_list_free(node->funcDecStmt.parameters);
    array_list_free(node->funcDecStmt.body);
    break;
  case STMT_TYPE_FOR_LOOP:
    free(node->forLoopStmt.varId);
    clm_exp_free(node->forLoopStmt.start);
    clm_exp_free(node->forLoopStmt.end);
    clm_exp_free(node->forLoopStmt.delta);
    array_list_free(node->forLoopStmt.body);
    break;
  case STMT_TYPE_WHILE_LOOP:
    clm_exp_free(node->whileLoopStmt.condition);
    array_list_free(node->whileLoopStmt.body);
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
  printf("\n");
  insert_whitespace(level);

  printf("Statement ");
  switch (node->type) {
  case STMT_TYPE_ASSIGN:
    printf("type : assign\n");

    insert_whitespace(level + 1);
    printf("lhs:");
    clm_exp_print(node->assignStmt.lhs, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("rhs:");
    clm_exp_print(node->assignStmt.rhs, level + 2);
    break;
  case STMT_TYPE_CALL:
    printf("type : func call\n");

    insert_whitespace(level + 1);
    printf("expression: ");
    clm_exp_print(node->callExpr, level + 2);
    break;
  case STMT_TYPE_CONDITIONAL:
    printf("type : conditional\n");

    insert_whitespace(level + 1);
    printf("condition:");
    clm_exp_print(node->conditionStmt.condition, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("truebody:");
    array_list_foreach_2(node->conditionStmt.trueBody, level + 2,
                         clm_stmt_print);
    printf("\n");

    insert_whitespace(level + 1);
    printf("falsebody:");
    array_list_foreach_2(node->conditionStmt.falseBody, level + 2,
                         clm_stmt_print);
    break;
  case STMT_TYPE_FUNC_DEC:
    printf("type : func dec, name : %s\n", node->funcDecStmt.name);

    insert_whitespace(level + 1);
    printf("return type : %d, ", node->funcDecStmt.returnType);
    printf("ret rows : %d | %s, ", node->funcDecStmt.returnSize.rows,
           node->funcDecStmt.returnSize.rowVar);
    printf("ret cols : %d | %s", node->funcDecStmt.returnSize.cols,
           node->funcDecStmt.returnSize.colVar);
    printf("\n");

    insert_whitespace(level + 1);
    printf("parameters:");
    array_list_foreach_2(node->funcDecStmt.parameters, level + 2,
                         clm_exp_print);
    printf("\n");

    insert_whitespace(level + 1);
    printf("body:");
    array_list_foreach_2(node->funcDecStmt.body, level + 2, clm_stmt_print);
    break;
  case STMT_TYPE_FOR_LOOP:
    printf("type : loop, var : %s\n", node->forLoopStmt.varId);

    insert_whitespace(level + 1);
    printf("start:");
    clm_exp_print(node->forLoopStmt.start, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("delta:");
    clm_exp_print(node->forLoopStmt.delta, level + 2);
    printf("\n");

    insert_whitespace(level + 1);
    printf("end:");
    clm_exp_print(node->forLoopStmt.end, level + 2);
    printf("\n");
    
    insert_whitespace(level + 1);
    printf("body:");
    array_list_foreach_2(node->forLoopStmt.body, level + 2, clm_stmt_print);
    break;
  case STMT_TYPE_WHILE_LOOP:
    printf("type : while loop\n");
    break;
  case STMT_TYPE_PRINT:
    printf("type : print, newline : %d\n", node->printStmt.appendNewline);
    insert_whitespace(level + 1);
    printf("expression:");
    clm_exp_print(node->printStmt.expression, level + 2);
    break;
  case STMT_TYPE_RET:
    printf("type : return\n");
    insert_whitespace(level + 1);
    printf("expression: ");
    clm_exp_print(node->returnExpr, level + 2);
    break;
  }
}
