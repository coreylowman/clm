#ifndef CLM_AST_H_
#define CLM_AST_H_

#include "clm_type.h"

//
// Forward Declarations
//
typedef struct ArrayList ArrayList;

//
// Expression
//
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

typedef struct MatrixSize {
  int rows;
  int cols;
  char *rowVar;
  char *colVar;
} MatrixSize;

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
      MatrixSize size;
    } matDecExp;

    struct {
      char *name;
      ClmType type;
      MatrixSize size;
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

//
// Statements
//
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
      MatrixSize returnSize;
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
