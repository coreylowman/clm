#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtab.h"
#include "parser.h"
#include "files.h"
#include "ast.h"
#include "clm_errors.h"

/**
	parser.c contains all of the code that parses through the input file's contents, catches syntax/type/size errors, and converts the input into
	an abstract syntax tree (AST). It passes a struct containing the AST, variable declarations, and whether or not to continue with compilation to
	the main clm function. 
	
	The main entry point for parser.c is PARSE_DATA *parser_main(char *filename);
*/

static PARSE_DATA *data;

/*
	isDig is used by the parser to tell whether or not the current character in the file contents is a digit or not.
	It is called in getSym() to set the current parse token to INT, as well as to allow variable names to have numbers in them.
*/
int isDig(char c){
	return c >= '0' && c <= '9';
}

/*
	isLetter
*/
int isLetter(char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/*

*/
int isWhiteSpace(char c){
	if(c == '\n' || c == '\r'){
		data->lineNo = data->lineNo + 1;
	}
	return c == '\t' || c == '\n' || c == '\r' || c == ' ';
}

/*
	accept is used to move the current symbol to the next symbol. 
	an important thing about this function is that it doesn't consume the symbol if it isn't equal to the parameter.
	this allows multiple different paths... for instance the base expressions in clm can be integers, or ids, so part
	of the expression parsing contains
	...
	if(accept(ID))
		...
	else if(accept(INT))
		...
	...
*/
int accept(Symbol s){
	if(data->sym == s){
		getsym();
		return 1;
	}
	return 0;
}

/*
	faccept is used to merely see if the current symbol is a certain symbol, and returns true if is and false if not.
	it is used when you don't want to move onto the next symbol. one use is for ID's. in clm,
	ID's can always be followed by indexes (@ index,index), but there is a separate function to
	handle this, so many times if an ID is possible it will be called with faccept
*/
int faccept(Symbol s){
	if(data->sym == s)
		return 1;
	return 0;
}

/*
	expect is used to say what should come next. if what you expect isn't there, it should be an error.
	it uses accept, just acts as a wrapper with an error call if accept fails.
	for example, the for loop header has very specific syntax:
	for (variable name) = (start number),(end number) do
	this is written as 
	if(accept(FOR)){
		expect(ID);
		...
		expect(ASSIGN);
		...
		expect(INT);
		...
		expect(COMMA);
		...
		expect(INT);
		...
		expect(DO);
		...
	}

	if any of these fails it is a syntax error!
*/
int expect(Symbol s){
	if(accept(s)){
		return 1;
	}
	error(symToString(s),3);
	return 0;
}

/*
	setToks captures the current token from the file's contents, so if the current token is FOR, then curTok = "for"
	it is used to capture variable names as well as integers. although once you've accepted a token, the current token is something else.
	this is why there is a previous token as well (prevTok). so if an integer is accepted, you can call atoi(prevTok) to get it's value.
	if(accept(INT)){
		i = atoi(prevTok);
	}
*/
void setToks(int start,int end){	
	if(data->curTok != NULL){		
		free(data->prevTok);
		data->prevTok = (char *)malloc(strlen(data->curTok) + 1);
		strcpy(data->prevTok,data->curTok);
	}
	free(data->curTok);
	data->curTok = (char *)malloc(end - start + 1);
	strncpy(data->curTok,data->program_string + start,end - start);
	data->curTok[end - start] = '\0';
}

/*
	error prints out certain error types along with a more specific error message. it also sets compilation to false, so code generation
	does not occur.
*/
void error(const char *errstr,int type){
	switch(type){
		case 0:
			printf("PARSE");
			break;
		case 1:
			printf("TYPE");
			break;
		case 2:
			printf("SIZE");
			break;
		case 3:
			printf("EXPECT");
			break;
		default:
			printf("UNKNOWN");
			break;
	}
	printf(" ERROR: %s at line %d\n",errstr,data->lineNo);
	data->compile = 0;
}

/*
	getsym reads through the file's contents and converts the characters into tokens.
	it ignores whitespace, and looks for certain predefined language tokens.
	it sets the sym and then calls setToks to set curTok and prevTok
*/
void getsym(){	
	char c = data->program_string[data->curInd++];	
	while(isWhiteSpace(c)) c = data->program_string[data->curInd++];
	if(isDig(c)){
		int start = data->curInd - 1;
		c = data->program_string[data->curInd++];
		while(isDig(c)) c = data->program_string[data->curInd++];
		data->curInd--;
		data->sym = SYM_CONSTANT;
		setToks(start,data->curInd);		
	}else if(isLetter(c)){
		int start = data->curInd - 1;
		c = data->program_string[data->curInd++];
		while(isLetter(c) || isDig(c)) c = data->program_string[data->curInd++];
		data->curInd--;
		setToks(start,data->curInd);
		if (!strcmp("for",data->curTok)){
			data->sym = SYM_FOR;
		}else if(!strcmp("by",data->curTok)){
			data->sym = SYM_BY;
		}else if(!strcmp("do",data->curTok)){
			data->sym = SYM_DO;
		}else if(!strcmp("print",data->curTok)){
			data->sym = SYM_PRINT;
		}else if(!strcmp("if",data->curTok)){
			data->sym = SYM_IF;
		}else if(!strcmp("then",data->curTok)){
			data->sym = SYM_THEN;
		}else if(!strcmp("else",data->curTok)){
			data->sym = SYM_ELSE;
		}else if(!strcmp("int",data->curTok)){
			data->sym = SYM_INT;
		}else if(!strcmp("and",data->curTok)){
			data->sym = SYM_AND;
		}else if (!strcmp("or", data->curTok)){
			data->sym = SYM_OR;
		}else if (!strcmp("return",data->curTok)){
			data->sym = SYM_RETURN;
		}else{
			data->sym = SYM_ID;
		}
	}else{
		int start = data->curInd - 1;	
		int comment = 0;
		switch(c){
			case '>':
				if(data->program_string[data->curInd] == '='){
					data->curInd++;
					data->sym = SYM_GTE;
				}else
					data->sym = SYM_GT;
				break;
			case '<':
				if(data->program_string[data->curInd] == '='){
					data->curInd++;
					data->sym = SYM_LTE;
				}else
					data->sym = SYM_LT;
				break;
			case '?':
				data->sym = SYM_QUESTION;
				break;
			case '.':
				data->sym = SYM_PERIOD;
				break;
			case '\\':
				data->sym = SYM_BACKSLASH;
				break;
			case '[':
				data->sym = SYM_LBRACK;
				break;
			case ']':
				data->sym = SYM_RBRACK;
				break;
			case '(':
				data->sym = SYM_LPAREN;
				break;
			case ')':
				data->sym = SYM_RPAREN;
				break;
			case '#':
				data->sym = SYM_HASH;
				break;
			case ',':
				data->sym = SYM_COMMA;
				break;
			case ':':
				data->sym = SYM_COLON;
				break;
			case ';':
				data->sym = SYM_SEMI;
				break;
			case '*':
				data->sym = SYM_MULT;
				break;
			case '/':
				if (data->program_string[data->curInd] == '/'){
					data->curInd++;
					comment = 1;
					c = data->program_string[data->curInd++];
					while (!(c == '\n' || c == '\r')) c = data->program_string[data->curInd++];
					data->lineNo++;
					data->curInd--;
					getsym();
				}else
					data->sym = SYM_DIV;
				break;
			case '-':
				data->sym = SYM_SUB;
				break;
			case '+':
				data->sym = SYM_ADD;
				break;	
			case '=':
				if(data->program_string[data->curInd] == '='){
					data->curInd++;
					data->sym = SYM_EQ;
				}else
					data->sym = SYM_ASSIGN;
				break;
			case '!':
				if(data->program_string[data->curInd] == '='){
					data->curInd++;
					data->sym = SYM_NEQ;
				}else
					data->sym = SYM_NOT;
				break;
			case '@':
				data->sym = SYM_AT;
				break;
			default:
				printf("fail getsym: %c\n",c);
				break;		
		}
		if (!comment)
			setToks(start,data->curInd);
	}
}

/*
	start is the starting point for the parsing. it initializes curTok and sym by first calling getsym, and then starts parsing with
	statementList, as clm program syntax is built up of statements
*/
AST *start(){
	AST *tree;
	getsym();
	tree = statementList(NULL,0);
	return tree;
}

/*
	statementList parses through statements, and concatenates them together into the AST
	if the program is not finished, it parses a statement, and the appends it to the end of a list
	that is passed along throughout the parsing. then it calls itself.
*/
AST *statementList(AST *list,int base){
	if (!base && faccept(SYM_SEMI));
	else if (accept(SYM_SEMI));
	else{		
		ASTNODE *anode;
		anode = statement();
		list = append(list,anode);
		list = statementList(list,base);
	}
	return list;
}

/*
	lhs is used to parse variable name uses and indexing. It populates and INDEXNODE, which is made up
	of a name, a row index, and a column index. There a few possibilities for the row and column indices:
	1. NULL: a hash sign was used, this means to take all of the elements of either a row or column
	2. -1: no index was present (so just for regular variable use)
	3. Int: a constant index was present
	4. Expression: anything else (a variable, an arithmetic expression, etc...)
*/
INDEXNODE *lhs(){
	INDEXNODE *inode;
	if (accept(SYM_ID)){
		inode = makeIndex(data->prevTok,makeIntExp(-1),makeIntExp(-1));
		if (accept(SYM_AT)){
			EXPNODE **ind;
			ind = id_index();
			inode->index1 = ind[0];
			inode->index2 = ind[1];

			if(inode->index1 == NULL && inode->index2 == NULL){
				inode->index1 = makeIntExp(-1);
				inode->index2 = makeIntExp(-1);
			}

			if(inode->index1 != NULL && (inode->index1->rows != 1 || inode->index1->cols != 1)){
				error(ERR_IND_SIZE,2);
				printf("%d %d\n",inode->index1->rows,inode->index1->cols);
			}else if(inode->index2 != NULL && (inode->index2->rows != 1 || inode->index2->cols != 1)){
				error(ERR_IND_SIZE,2);
				printf("%d %d\n",inode->index2->rows,inode->index2->cols);
			}
		}
	}else
		error("lhs",0);		
	return inode;
}

/*
	paramSize is used to parse parameter sizes of function declarations. It is called in the parameter parse method.
	There are 3 possibilities for a function parameter size:
	1. Constant int (A[4:4] means A is a matrix with 4 rows and 4 cols)
	2. Unknown (only useful in return values, since a parameter can just be assigned a variable name for the size)
	3. Variable (A[m:n] means A is a matrix with m rows and n cols, so both sizes would be 0
*/
int paramSize(){
	if (accept(SYM_CONSTANT)){
		return atoi(data->prevTok);
	}
	else if (accept(SYM_QUESTION)){
		return -1;
	}
	else{
		expect(SYM_ID);
		return 0;
	}
}

/*
	parameter is used to parse parameters in function declarations. It expects at least a name,
	then optionally followed by brackets with sizes. If it does find sizes, and they are variables,
	(e.g. A[m:n]) then it adds the size's name to the symbol's rname/cname. These are then added
	to the symbol table in the lines following this functions call.
*/
SYMENTRY *parameter(){
	SYMENTRY *p = NULL;
	if (accept(SYM_ID)){		
		p = makeVarEntry(data->prevTok, 1, 1);
		if (accept(SYM_LBRACK)){
			p->rows = paramSize();
			if (!p->rows){
				p->rname = (char *)malloc(strlen(data->prevTok) + 1);
				strcpy(p->rname, data->prevTok);
			}
			expect(SYM_COLON);
			p->cols = paramSize();
			if (!p->cols){
				p->cname = (char *)malloc(strlen(data->prevTok) + 1);
				strcpy(p->cname, data->prevTok);
			}
			expect(SYM_RBRACK);
		}
	}
	return p;
}

//returns ASTNODE
ASTNODE *statement(){
	if (faccept(SYM_ID)){
		INDEXNODE *inode;
		EXPNODE *enode;
		SYMENTRY *sentry;

		inode = lhs();
		expect(SYM_ASSIGN);
		enode = expression_1();
		sentry = findSym(data->symbol_table, inode->id);
		if (inode->index1 == NULL){
			//column - A @ #,1
			if (sentry == NULL)
				error(ERR_UNDEC_VAR, 1);
			else if (sentry->rows != enode->rows || 1 != enode->cols)
				error(ERR_SIZE_DIFF, 2);
		}
		else if (inode->index2 == NULL){
			//row - A @ 1,#
			if (sentry == NULL)
				error(ERR_UNDEC_VAR, 1);
			else if (1 != enode->rows || sentry->cols != enode->cols)
				error(ERR_SIZE_DIFF, 2);
		}
		else if (inode->index1->exp_type == 5 && inode->index1->exp.val == -1
			&& inode->index2->exp_type == 5 && inode->index2->exp.val == -1){
			//no index	
			if (sentry == NULL)
				data->symbol_table = appendSym(data->symbol_table, makeVarEntry(inode->id, enode->rows, enode->cols));
			else if (sentry->rows != enode->rows || sentry->cols != enode->cols)
				error(ERR_SIZE_DIFF, 2);
		}
		else{
			//element - A @ 1,1 || A @ B,C
			if (sentry == NULL)
				error(ERR_UNDEC_VAR, 1);
			else if (1 != enode->rows || 1 != enode->cols)
				error(ERR_SIZE_DIFF, 2);
		}
		return makeAssignStmt(inode, enode);
	}
	else if (accept(SYM_PRINT)){
		EXPNODE *enode;
		enode = expression_1();
		return makePrintStmt(enode);
	}
	else if (accept(SYM_FOR)){
		INDEXNODE *inode;
		SYMENTRY *sentry;
		int s, e;
		inode = lhs();
		sentry = findSym(data->symbol_table, inode->id);
		if (sentry == NULL)
			data->symbol_table = appendSym(data->symbol_table, makeVarEntry(inode->id, 1, 1));
		else if (sentry->rows != 1 || sentry->cols != 1)
			error(ERR_BAD_LOOP_VAR, 2);
		expect(SYM_ASSIGN);
		expect(SYM_CONSTANT);
		s = atoi(data->prevTok);
		expect(SYM_COMMA);
		expect(SYM_CONSTANT);
		e = atoi(data->prevTok);
		if (accept(SYM_BY)){
			int c;
			AST *anode = NULL;
			expect(SYM_CONSTANT);
			c = atoi(data->prevTok);
			expect(SYM_DO);
			anode = statementList(anode, 1);
			return makeLoopStmt(inode->id, s, e, c, anode);
		}
		else if (accept(SYM_DO)){
			AST *anode = NULL;
			anode = statementList(anode, 1);
			return makeLoopStmt(inode->id, s, e, 1, anode);
		}
		else{
			error(EXP_DO_BY, 3);
		}
	}
	else if (accept(SYM_IF)){
		EXPNODE *cond;
		AST *tbody = NULL, *fbody = NULL;
		cond = expression_1();
		expect(SYM_THEN);
		tbody = statementList(tbody, 1);
		if (accept(SYM_ELSE))
			fbody = statementList(fbody, 1);
		return makeCondStmt(cond, tbody, fbody);
	}
	else if (accept(SYM_RETURN)){
		EXPNODE *exp;
		exp = expression_1();
		return makeRetStmt(exp);
	}
	else if (accept(SYM_BACKSLASH)){
		SYMTAB *t = NULL;
		SYMENTRY *s = NULL;
		AST *body = NULL;
		int nparams = 0,r = 0,c = 0;
		char *name = NULL;
		
		expect(SYM_ID);
		name = (char *)malloc(strlen(data->prevTok) + 1);
		strcpy(name, data->prevTok);

		s = parameter();
		while (s != NULL){
			nparams++;
			t = appendSym(t, s);
			if (!s->rows)
				t = appendSym(t, makeVarEntry(s->rname, 1, 1));
			if (!s->cols)
				t = appendSym(t, makeVarEntry(s->cname, 1, 1));
			s = parameter();
		}
		
		if (t == NULL){
			t = (SYMTAB *)malloc(sizeof(SYMTAB));
			t->numSyms = 0;
			t->data = NULL;
		}		
		if (data->symbol_table == NULL){
			data->symbol_table = (SYMTAB *)malloc(sizeof(SYMTAB));
			data->symbol_table->numSyms = 0;
			data->symbol_table->parent = NULL;
			data->symbol_table->data = NULL;
		}

		t->parent = data->symbol_table;		
		
		data->symbol_table = t;
		
		if (accept(SYM_SUB)){
			expect(SYM_GT);
			if (accept(SYM_INT)){
				r = 1;
				c = 1;
			}
			else{
				expect(SYM_LBRACK);
				r = paramSize();
				expect(SYM_COLON);
				c = paramSize();
				expect(SYM_RBRACK);
			}
		}
		expect(SYM_ASSIGN);
		body = statementList(body,1);

		data->symbol_table = data->symbol_table->parent;
		data->symbol_table = appendSym(data->symbol_table, makeFuncEntry(name, t, r, c));
		return makeFuncStmt(name, copySymTab(t), body, r, c);
	}else{
		error(EXP_ID_FOR_PRNT,3);
		getsym();
	}
	return makePrintStmt(makeIntExp(0));
}

//returns 2 integers
EXPNODE** id_index(){
	EXPNODE **enodes = (EXPNODE **)malloc(sizeof(EXPNODE *)*2);
	enodes[0] = expression_1();
	expect(SYM_COMMA);
	enodes[1] = expression_1();
	return enodes;
}

EXPNODE *expression_1(){
	//and or
	EXPNODE *enode1;
	EXPNODE *enode2;
	Bool_op op;
	enode1 = expression_2();
	while (faccept(SYM_AND) || faccept(SYM_OR)){
		op = accept(SYM_AND) ? _AND : _OR, accept(SYM_OR);
		enode2 = expression_2();
		enode1 = makeBoolExp(op,enode1,enode2);
		if(enode1->rows == 0)
			error(ERR_BOOL_SIZE,2);
	}
	return enode1;
}

EXPNODE *expression_2(){
	EXPNODE *enode1;
	EXPNODE *enode2;
	Bool_op op;
	enode1 = expression_3();
	while (faccept(SYM_EQ) || faccept(SYM_NEQ)){
		op = accept(SYM_EQ) ? _EQ : _NEQ, accept(SYM_NEQ);
		enode2 = expression_3();
		enode1 = makeBoolExp(op,enode1,enode2);
		if(enode1->rows == 0)
			error(ERR_BOOL_SIZE,2);
	}
	return enode1;
}

EXPNODE *expression_3(){
	EXPNODE *enode1;
	EXPNODE *enode2;
	Bool_op op;
	enode1 = expression_4();
	while (faccept(SYM_GT) || faccept(SYM_LT) || faccept(SYM_GTE) || faccept(SYM_LTE)){
		op = accept(SYM_GT) ? _GT : accept(SYM_LT) ? _LT : accept(SYM_GTE) ? _GTE : _LTE, accept(SYM_LTE);
		enode2 = expression_4();
		enode1 = makeBoolExp(op,enode1,enode2);
		if(enode1->rows == 0)
			error(ERR_BOOL_SIZE,2);
	}
	return enode1;
}

EXPNODE *expression_4(){
	EXPNODE *enode1;
	EXPNODE *enode2;
	Arith_op op;
	enode1 = expression_5();
	while (faccept(SYM_ADD) || faccept(SYM_SUB)){
		op = accept(SYM_ADD) ? _ADD : _SUB, accept(SYM_SUB);
		enode2 = expression_5();
		enode1 = makeArithExp(op,enode1,enode2);
	}
	return enode1;
}

EXPNODE *expression_5(){
	EXPNODE *enode1;
	EXPNODE *enode2;
	Arith_op op;
	enode1 = expression_6();	
	while (faccept(SYM_MULT) || faccept(SYM_DIV)){
		op = accept(SYM_MULT) ? _MULT : _DIV, accept(SYM_DIV);
		enode2 = expression_6();
		if(enode2->rows == 0)
			error(ERR_ARITH_SIZE,1);
		enode1 = makeArithExp(op,enode1,enode2);
	}	
	return enode1;
}

EXPNODE *expression_6(){
	EXPNODE *enode1;
	if (accept(SYM_LPAREN)){
		enode1 = expression_1();
		expect(SYM_RPAREN);
		return enode1;
	}
	else if (accept(SYM_CONSTANT)){
		return makeIntExp(atoi(data->prevTok));
	}
	else if (accept(SYM_SUB)){
		enode1 = expression_6();
		if(enode1->rows == 1 && enode1->cols == 1)
			return makeArithExp(_SUB,makeIntExp(0),enode1);
		else
			return makeArithExp(_SUB,makeDecExp(NULL,enode1->rows,enode1->cols),enode1);
	}
	else if (accept(SYM_NOT)){
		enode1 = expression_6();
		return makeBoolExp(_NOT,enode1,NULL);
	}
	else if (faccept(SYM_ID)){
		INDEXNODE *inode = lhs();
		int r = 0,c = 0;
		SYMENTRY *sentry;
		sentry = findSym(data->symbol_table, inode->id);
		if(sentry != NULL){
			r = sentry->rows;
			c = sentry->cols;
		}else{
			error(ERR_UNDEC_VAR,1);
		}	
		if(inode->index1 == NULL)
			c = 1;
		else if(inode->index2 == NULL)
			r = 1;
		else if(inode->index1->exp_type == 5 && inode->index1->exp.val == -1 && inode->index2->exp_type == 5 && inode->index2->exp.val == -1);
		else
			r = c = 1;
		return makeLhsExp(inode,r,c);
	}
	else if (accept(SYM_LBRACK)){
		int i1,i2,num;
		int *list;
		if (accept(SYM_ID)){
			enode1 = makeDecExp(NULL, 0, 0);

			enode1->exp.emd->rname = (char *)malloc(strlen(data->prevTok) + 1);
			strcpy(enode1->exp.emd->rname, data->prevTok);
			if (findSym(data->symbol_table, data->prevTok) == NULL)
				error(ERR_UNDEC_VAR, 0);
			expect(SYM_COLON);
			if (accept(SYM_ID)){
				enode1->exp.emd->cname = (char *)malloc(strlen(data->prevTok) + 1);
				strcpy(enode1->exp.emd->cname, data->prevTok);
				if (findSym(data->symbol_table, data->prevTok) == NULL)
					error(ERR_UNDEC_VAR, 0);
			}else{
				expect(SYM_CONSTANT);
				enode1->cols = atoi(data->prevTok);
			}
			expect(SYM_RBRACK);
			return enode1;
		}
		else{
			expect(SYM_CONSTANT);
			i1 = atoi(data->prevTok);
			if (accept(SYM_COLON)){
				enode1 = makeDecExp(NULL, i1, 0);
				if (accept(SYM_ID)){
					enode1->exp.emd->cname = (char *)malloc(strlen(data->prevTok) + 1);
					strcpy(enode1->exp.emd->cname, data->prevTok);
					if (findSym(data->symbol_table, data->prevTok) == NULL)
						error(ERR_UNDEC_VAR, 0);
				}else{
					expect(SYM_CONSTANT);
					enode1->cols = atoi(data->prevTok);
				}
				expect(SYM_RBRACK);
				return enode1;
			}
			else{
				list = (int *)malloc(sizeof(int));
				list[0] = i1;
				num = 1;
				while (accept(SYM_CONSTANT)){
					i1 = atoi(data->prevTok);
					num++;
					list = (int *)realloc(list, num*sizeof(int));
					list[num - 1] = i1;
				}
				i2 = num;
				while (accept(SYM_COMMA)){
					while (accept(SYM_CONSTANT)){
						i1 = atoi(data->prevTok);
						num++;
						list = (int *)realloc(list, num*sizeof(int));
						list[num - 1] = i1;
					}
				}
				expect(SYM_RBRACK);
				return makeDecExp(list, num / i2, i2);
			}			
		}		
	}
	else if (accept(SYM_HASH)){
		return NULL;	
	}else{
		error(EXP_LP_INT_ID_LC_FUNC,3);
	}
	return makeIntExp(0);
}

void free_parse_data(PARSE_DATA *data){
	if(data != NULL){
		free(data->program_string);
		free(data->curTok);
		free(data->prevTok);		
		free_ast(data->parseTree);
		free_symtab(data->symbol_table);
	}
	free(data);
	data = NULL;
}

PARSE_DATA *parser_main(char *filename){
	free_parse_data(data);
	data = (PARSE_DATA *)malloc(sizeof(PARSE_DATA));
	data->program_string = getFileContents(filename);
	data->curTok = NULL;
	data->prevTok = NULL;
	data->curInd = 0;
	data->lineNo = 1;
	data->compile = 1;
	data->symbol_table = NULL;
	printf("creating abstract syntax tree...\n");
	data->parseTree = start();
	printAST(data->parseTree,0);
	return data;	
}