#ifndef CLM_STATEMENT_H_
#define CLM_STATEMENT_H_

#include "clm_expression.h"
#include "clm_array_list.h"
#include "clm_type.h"

typedef enum ClmStmtType {
    STMT_TYPE_ASSIGN,
    STMT_TYPE_CALL,
    STMT_TYPE_CONDITIONAL,
    STMT_TYPE_FUNC_DEC,
    STMT_TYPE_LOOP,
    STMT_TYPE_PRINT,
    STMT_TYPE_RET
} ClmStmtType;

typedef struct ClmAssignStmt {
    ClmExpNode *lhs;
    ClmExpNode *rhs;
} ClmAssignStmt;

typedef struct ClmConditionStmt {
    ClmExpNode *condition;
    ClmArrayList *trueBody; //array list of ClmStmtNode
    ClmArrayList *falseBody; //array list of ClmStmtNode
} ClmConditionStmt;

typedef struct ClmFuncDecStmt {
    char *name;
    ClmArrayList *parameters; //array list of ClmExpNode
    ClmType returnType;
    int returnRows;
    char *returnRowsVar;
    int returnCols;
    char *returnColsVar;
    ClmArrayList *body; //array list of ClmStmtNode
} ClmFuncDecStmt;

typedef struct ClmLoopStmt {
    char *varId;
    ClmExpNode *start;
    ClmExpNode *end;
    ClmExpNode *delta;
    ClmArrayList *body; //array list of ClmStmtNode
} ClmLoopStmt;

typedef struct ClmPrintStmt {
    ClmExpNode *expression;
    int appendNewline;
} ClmPrintStmt;

typedef struct ClmStmtNode {
    ClmStmtType type;    
    union {
        ClmAssignStmt *assignStmt;
        ClmExpNode *callExpr;
        ClmConditionStmt *conditionStmt;        
        ClmFuncDecStmt *funcDecStmt;
        ClmLoopStmt *loopStmt;
        ClmPrintStmt *printStmt;
        ClmExpNode *returnExpr;
    };
    int lineNo;
    int colNo;
} ClmStmtNode;

ClmStmtNode *clm_stmt_new_assign(ClmExpNode *lhs,ClmExpNode *rhs);
ClmStmtNode *clm_stmt_new_call(ClmExpNode *callExpr);
ClmStmtNode *clm_stmt_new_cond(ClmExpNode *condition,ClmArrayList *trueBody,ClmArrayList *falseBody);
ClmStmtNode *clm_stmt_new_dec(char *name,ClmArrayList *params,ClmType returnType, int returnRows,char *returnRowsVars,int returnCols,char *returnColsVar, ClmArrayList *functionBody);
ClmStmtNode *clm_stmt_new_loop(char *varId,ClmExpNode *start,ClmExpNode *end,ClmExpNode *delta,ClmArrayList *loopBody);
ClmStmtNode *clm_stmt_new_print(ClmExpNode *expression,int appendNewline);
ClmStmtNode *clm_stmt_new_return(ClmExpNode *returnExpr);

void clm_stmt_print(ClmStmtNode *node, int level);

void clm_stmt_free(void *data);

#endif