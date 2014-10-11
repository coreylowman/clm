#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "symtab.h"

typedef enum {
	SYM_ID, SYM_CONSTANT, SYM_LBRACK, SYM_RBRACK, SYM_RPAREN, SYM_LPAREN, SYM_BACKSLASH, SYM_HASH, SYM_COMMA,
	SYM_COLON, SYM_SEMI, SYM_MULT, SYM_DIV, SYM_SUB, SYM_ADD, SYM_ASSIGN, SYM_AT, SYM_FOR, SYM_BY, SYM_DO, SYM_PRINT,
	SYM_IF, SYM_THEN, SYM_ELSE, SYM_INT, SYM_AND, SYM_OR, SYM_NOT, SYM_EQ, SYM_NEQ, SYM_GTE, SYM_LTE, SYM_GT, 
	SYM_LT, SYM_QUESTION, SYM_PERIOD, SYM_RETURN
} Symbol;

typedef struct{
	Symbol sym;
	char *program_string;
	char *curTok;
	char *prevTok;
	int curInd;
	int lineNo;
	int compile;
	SYMTAB *symbol_table;	
	AST *parseTree;
}PARSE_DATA;

void getsym(void);
int accept(Symbol s);
int expect(Symbol s);
void error(const char *errstr,int type);

AST *start(void);
AST *statementList(AST *list,int base);
int paramSize();
SYMENTRY *parameter();
ASTNODE *statement(void);
EXPNODE** id_index(void);
INDEXNODE *lhs(void);
EXPNODE *expression_1(void);
EXPNODE *expression_2(void);
EXPNODE *expression_3(void);
EXPNODE *expression_4(void);
EXPNODE *expression_5(void);
EXPNODE *expression_6(void);

void free_parse_data(PARSE_DATA *data);

PARSE_DATA *parser_main(char *filename);

#endif