#ifndef CLM_STATEMENT_H_
#define CLM_STATEMENT_H_

#include "array_list.h"
#include "clm_expression.h"
#include "clm_type.h"

typedef enum StmtType {
  STMT_TYPE_ASSIGN,
  STMT_TYPE_CALL,
  STMT_TYPE_CONDITIONAL,
  STMT_TYPE_FUNC_DEC,
  STMT_TYPE_LOOP,
  STMT_TYPE_PRINT,
  STMT_TYPE_RET
} StmtType;

typedef struct ClmStmtNode {
  StmtType type;

  union {
    struct {
      ClmExpNode *lhs;
      ClmExpNode *rhs;
    } assignStmt;

    struct {
      ClmExpNode *condition;
      ArrayList *trueBody;  // array list of ClmStmtNode
      ArrayList *falseBody; // array list of ClmStmtNode
    } conditionStmt;

    ClmExpNode *callExpr;

    struct {
      char *name;
      ArrayList *parameters; // array list of ClmExpNode
      ClmType returnType;
      int returnRows;
      char *returnRowsVar;
      int returnCols;
      char *returnColsVar;
      ArrayList *body; // array list of ClmStmtNode
    } funcDecStmt;

    struct {
      char *varId;
      int startInclusive;
      int endInclusive;
      ClmExpNode *start;
      ClmExpNode *end;
      ClmExpNode *delta;
      ArrayList *body; // array list of ClmStmtNode
    } loopStmt;

    struct {
      ClmExpNode *expression;
      int appendNewline;
    } printStmt;

    ClmExpNode *returnExpr;
  };

  int lineNo;
  int colNo;
} ClmStmtNode;

ClmStmtNode *clm_stmt_new_assign(ClmExpNode *lhs, ClmExpNode *rhs);
ClmStmtNode *clm_stmt_new_call(ClmExpNode *callExpr);
ClmStmtNode *clm_stmt_new_cond(ClmExpNode *condition, ArrayList *trueBody,
                               ArrayList *falseBody);
ClmStmtNode *clm_stmt_new_dec(char *name, ArrayList *params, ClmType returnType,
                              int returnRows, char *returnRowsVars,
                              int returnCols, char *returnColsVar,
                              ArrayList *functionBody);
ClmStmtNode *clm_stmt_new_loop(char *varId, ClmExpNode *start, ClmExpNode *end,
                               ClmExpNode *delta, ArrayList *loopBody,
                               int startInclusive, int endInclusive);
ClmStmtNode *clm_stmt_new_print(ClmExpNode *expression, int appendNewline);
ClmStmtNode *clm_stmt_new_return(ClmExpNode *returnExpr);

void clm_stmt_print(void *data, int level);

void clm_stmt_free(void *data);

#endif