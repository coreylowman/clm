#include <stdio.h>
#include <stdlib.h>

#include "util/string_util.h"

#include "clm_statement.h"

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
