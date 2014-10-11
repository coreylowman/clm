#ifndef AST_H
#define AST_H

#include "symtab.h"

typedef enum {_ADD,_SUB,_MULT,_DIV} Arith_op;
typedef enum {_AND,_OR,_EQ,_NEQ,_GT,_LT,_GTE,_LTE,_NOT} Bool_op;
typedef enum { arith, mat_dec, emp_mat_dec, _lhs, _int , param, _bool} Exp_type;
typedef enum { assign, loop, print_exp,func_dec,conditional,ret} Stmt_type;

typedef struct a_e{
	Arith_op operand;
	struct expnode *right;
	struct expnode *left;
} arith_exp;

typedef struct b_e{
	Bool_op operand;
	struct expnode *right;
	struct expnode *left;
} bool_exp;

typedef struct m_d_e{
	int *arr;
	int nrows;
	int ncols;
} mat_dec_exp;

typedef struct e_m_d_e{
	char *rname;
	char *cname;
} emp_mat_dec_exp;

typedef struct f_d_s {
	char *name;
	SYMTAB *sym_tab;
	struct ast *body;
	int rrows;
	int rcols;
} func_dec_stmt;

typedef struct a_s{
	struct indexnode *var;
	struct expnode *exp;
} assign_stmt;

typedef struct l_s{
	char *var;
	int start;
	int end;
	int change;
	struct ast *body;
} loop_stmt;

typedef struct c_s{
	struct expnode *condition;
	struct ast *true_body;
	struct ast *false_body;
}cond_stmt;

typedef struct indexnode {
	char * id;
	struct expnode *index1;
	struct expnode *index2;
} INDEXNODE;

typedef struct expnode {
	Exp_type exp_type;
	union {
		arith_exp *a;
		mat_dec_exp *md;
		emp_mat_dec_exp *emd;
		INDEXNODE *lhs_exp;
		int val;
		bool_exp *b;
	} exp;
	int rows;
	int cols;
} EXPNODE;

typedef struct astnode {
	Stmt_type stmt_type;
	union{
		assign_stmt *a;
		struct expnode *p;
		loop_stmt *l;
		func_dec_stmt *fd;
		cond_stmt *c;
		struct expnode *r;
	} stmt;
} ASTNODE;

typedef struct ast {
	ASTNODE *ele;
	struct ast * next;
} AST;

INDEXNODE *makeIndex(char * name,EXPNODE *i1,EXPNODE *i2);
EXPNODE *makeArithExp(Arith_op op,EXPNODE *l,EXPNODE *r);
EXPNODE *makeBoolExp(Bool_op op,EXPNODE *l,EXPNODE *r);
EXPNODE *makeDecExp(int * arr,int nrows,int ncols);
EXPNODE *makeLhsExp(INDEXNODE *var,int r,int c);
EXPNODE *makeIntExp(int val);
ASTNODE *makeAssignStmt(INDEXNODE *var, EXPNODE *exp);
ASTNODE *makePrintStmt(EXPNODE *exp);
ASTNODE *makeLoopStmt(char * name,int start,int end, int change, AST *body);
ASTNODE *makeFuncStmt(char *name,SYMTAB *parameters,AST *body,int ret_rows,int ret_cols);
ASTNODE *makeCondStmt(EXPNODE *cond,AST *true_body,AST *false_body);
ASTNODE *makeRetStmt(EXPNODE *exp);
AST *append(AST *list,ASTNODE *ele);

void printINDEXNODE(INDEXNODE *node,int level);
void printEXPNODE(EXPNODE *node,int level);
void printASTNODE(ASTNODE *node,int level);
void printAST(AST *ast,int level);

void free_ast(AST *tree);
void free_astnode(ASTNODE *node);
void free_indexnode(INDEXNODE *node);
void free_expnode(EXPNODE *node);

#endif