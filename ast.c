#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

INDEXNODE *makeIndex(char * name,EXPNODE *i1,EXPNODE *i2){
	INDEXNODE *node = (INDEXNODE *)malloc(sizeof(INDEXNODE));
	node->id = (char *)malloc(strlen(name) + 1);
	strcpy(node->id,name);
	node->index1 = i1;
	node->index2 = i2;
	return node;
}

EXPNODE *makeArithExp(Arith_op op, EXPNODE *l, EXPNODE *r){
	EXPNODE *node;
	arith_exp *f;
	
	f = (arith_exp *)malloc(sizeof(arith_exp));
	f->operand = op;
	f->right = r;
	f->left = l;
	
	node = (EXPNODE *)malloc(sizeof(EXPNODE));
	node->exp_type = arith;
	node->exp.a = f;
	switch(op){
		//add
		case 0:
			if(l->rows == r->rows && l->cols == r->cols){
				node->rows = l->rows;
				node->cols = l->cols;
			}else{
				node->rows = 0;
				node->cols = 0;
			}
			break;
		//sub
		case 1:
			if(l->rows == r->rows && l->cols == r->cols){
				node->rows = l->rows;
				node->cols = l->cols;
			}else{
				node->rows = 0;
				node->cols = 0;
			}
			break;
		//mult
		case 2:
			if(l->cols == r->rows){
				node->rows = l->rows;
				node->cols = r->cols;
			}else{
				node->rows = 0;
				node->cols = 0;
			}
			break;
		//div
		case 3:
			if(l->rows == r->rows && r->rows == 1 && l->cols == r->cols && r->cols == 1){
				node->rows = l->rows;
				node->cols = r->cols;
			}else{
				node->rows = 0;
				node->cols = 0;
			}
			break;
		default:
			break;
	}
	return node;
}

EXPNODE *makeBoolExp(Bool_op op,EXPNODE *left,EXPNODE *right){
	EXPNODE *node;
	bool_exp *b;
	
	b = (bool_exp *)malloc(sizeof(bool_exp));
	b->operand = op;
	b->left = left;
	b->right = right;

	node = (EXPNODE *)malloc(sizeof(EXPNODE));
	node->exp_type = _bool;
	node->exp.b = b;

	if(right == NULL && b->operand != _NOT){
		node->rows = 0;
		node->cols = 0;
		return node;
	}else if(right == NULL && b->operand == _NOT){
		node->rows = 1;
		node->cols = 1;
		return node;
	}

	if(left->rows == right->rows && left->cols == right->cols && left->rows == 1 && left->cols == 1){
		node->rows = 1;
		node->cols = 1;
	}else{
		node->rows = 0;
		node->cols = 0;
	}
	return node;
}

EXPNODE *makeDecExp(int *arr, int nrows,int ncols){
	EXPNODE *node;
	node = (EXPNODE *)malloc(sizeof(EXPNODE));
	if(arr == NULL){
		emp_mat_dec_exp *f;
		
		f = (emp_mat_dec_exp *)malloc(sizeof(emp_mat_dec_exp));				
		
		node->exp_type = emp_mat_dec;
		node->exp.emd = f;
		node->rows = nrows;
		node->cols = ncols;
	}else{
		mat_dec_exp *g;
		
		g = (mat_dec_exp *)malloc(sizeof(mat_dec_exp));
		g->arr = arr;
		g->nrows = nrows;
		g->ncols = ncols;
		
		node->exp_type = mat_dec;		
		node->exp.md = g;
		node->rows = nrows;
		node->cols = ncols;
	}
	return node;
}

EXPNODE *makeLhsExp(INDEXNODE *var,int r,int c){
	EXPNODE *node;
	node = (EXPNODE *)malloc(sizeof(EXPNODE));
	node->exp_type = _lhs;
	node->exp.lhs_exp = var;
	node->rows = r;
	node->cols = c;
	return node;
}

EXPNODE *makeIntExp(int val){	
	EXPNODE *node;
	node = (EXPNODE *)malloc(sizeof(EXPNODE));	
	node->exp_type = _int;
	node->exp.val = val;
	node->rows = 1;
	node->cols = 1;	
	return node;
}

ASTNODE *makeAssignStmt(INDEXNODE *var, EXPNODE *exp){
	ASTNODE *node;
	assign_stmt *a;

	a = (assign_stmt *)malloc(sizeof(assign_stmt));
	a->var = var;
	a->exp = exp;
	
	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = assign;
	node->stmt.a = a;
	return node;
}

ASTNODE *makePrintStmt(EXPNODE *exp){
	ASTNODE *node;	
	
	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = print_exp;
	node->stmt.p = exp;
	return node;
}

ASTNODE *makeLoopStmt(char * name,int start,int end, int change, AST *body){
	ASTNODE *node;
	loop_stmt *l;
	
	l = (loop_stmt *)malloc(sizeof(loop_stmt));
	l->var = (char *)malloc(strlen(name) + 1);
	strcpy(l->var,name);
	l->start = start;
	l->end = end;
	l->change = change;
	l->body = body;
	
	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = loop;
	node->stmt.l = l;
	return node;
}

ASTNODE *makeFuncStmt(char *name,SYMTAB *symbol_table,AST *body,int ret_rows,int ret_cols){
	ASTNODE *node;
	func_dec_stmt *fd;

	fd = (func_dec_stmt *)malloc(sizeof(func_dec_stmt));
	fd->name = (char *)malloc(strlen(name) + 1);
	strcpy(fd->name, name);
	fd->sym_tab = symbol_table;
	fd->body = body;
	fd->rrows = ret_rows;
	fd->rcols = ret_cols;

	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = func_dec;
	node->stmt.fd = fd;
	return node;
}

ASTNODE *makeCondStmt(EXPNODE *cond,AST *tbody,AST *fbody){
	ASTNODE *node;
	cond_stmt *c;
	c = (cond_stmt *)malloc(sizeof(cond_stmt));
	c->condition = cond;
	c->true_body = tbody;
	c->false_body = fbody;

	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = conditional;
	node->stmt.c = c;
	return node;
}

ASTNODE *makeRetStmt(EXPNODE *exp){
	ASTNODE *node;

	node = (ASTNODE *)malloc(sizeof(ASTNODE));
	node->stmt_type = ret;
	node->stmt.r = exp;
	return node;
}

AST *append(AST *list,ASTNODE *ele){	
	AST * newnode,*prev,*curr;
	newnode = (AST *)malloc(sizeof(AST));
	newnode->ele = ele;
	newnode->next = NULL;
	
	if(list == NULL){	
		list = newnode;
		return list;
	}
	
	prev = NULL;
	curr = list;	
	while (curr != NULL){
		prev = curr;
		curr = curr->next;
	}	

	prev->next = newnode;
	return list;
}

void printAST(AST *ast,int level){
	AST *curr = ast;
	int i = 0;
	if(ast == NULL) return;
	while(i++ < level) printf("\t");
	printf("AST:\n");
	while(curr != NULL){
		printASTNODE(curr->ele,level + 1);
		curr = curr->next;
	}
}

void printASTNODE(ASTNODE *node,int level){
	loop_stmt *lsp;
	int i = 0;
	if(node == NULL) return;
	while(i++ < level) printf("\t");
	switch(node->stmt_type){
		case assign:
			printf("ASTNODE: assign\n");
			printINDEXNODE(node->stmt.a->var,level + 1);
			printEXPNODE(node->stmt.a->exp,level + 1);
			break;
		case loop:
			lsp = node->stmt.l;
			printf("ASTNODE: loop %s %d to %d by %d\n",lsp->var,lsp->start,lsp->end,lsp->change);
			printAST(lsp->body,level + 1);
			break;
		case print_exp: 
			printf("ASTNODE: print\n");
			printEXPNODE(node->stmt.p,level + 1);
			break;
		case conditional:
			printf("ASTNODE: conditional\n");
			printEXPNODE(node->stmt.c->condition,level + 1);
			i = 0;
			while(i++ < level) printf("\t");
			printf("TRUE BRANCH ");
			printAST(node->stmt.c->true_body,level + 1);
			i = 0;
			while(i++ < level) printf("\t");
			printf("FALSE BRANCH ");
			printAST(node->stmt.c->false_body,level + 1);
			break;
		case func_dec:
			printf("ASTNODE: function %s -> [%d:%d]\n",node->stmt.fd->name,node->stmt.fd->rrows,node->stmt.fd->rcols);
			printAST(node->stmt.fd->body, level + 1);
			break;
		case ret:
			printf("ASTNODE: return\n");
			printEXPNODE(node->stmt.r, level + 1);
			break;
		default:
			break;
	}
}

void printINDEXNODE(INDEXNODE *node,int level){
	int i = 0;
	while(i++ < level) printf("\t");
	printf("INDEXNODE: %s ",node->id);
	if(node->index1 == NULL)
		printf("#,");
	else
		printf("exp,");

	if(node->index2 == NULL)
		printf("#\n");
	else
		printf("exp\n");
}

void printEXPNODE(EXPNODE *node,int level){	
	int i = 0;
	if(node == NULL) return;
	while(i++ < level) printf("\t");
	printf("EXPNODE(%d,%d):",node->rows,node->cols);
	switch(node->exp_type){
		case arith:
			switch(node->exp.a->operand){
				case _ADD:
					printf(" add\n");
					break;
				case _SUB: //sub
					printf(" sub\n");
					break;
				case _MULT: //mult
					printf(" mult\n");
					break;
				case _DIV: //div			
					printf(" div\n");
					break;
				default:
					break;
			}
			printEXPNODE(node->exp.a->left,level + 1);
			printEXPNODE(node->exp.a->right,level + 1);
			break;
		case mat_dec:
			printf(" full matrix %d rows %d cols\n",node->exp.md->nrows,node->exp.md->ncols);
			i = 0;
			while(i++ < level)printf("\t");
			i = 0;
			while(i < node->exp.md->nrows * node->exp.md->ncols){
				printf("%d ",node->exp.md->arr[i]);
				i++;
			}
			printf("\n");
			break;
		case emp_mat_dec:
			printf(" empty matrix ");
			if (node->rows == 0)
				printf("%s rows ", node->exp.emd->rname);
			else
				printf("%d rows ", node->rows);
			if (node->cols == 0)
				printf("%s cols\n", node->exp.emd->cname);
			else
				printf("%d cols\n", node->cols);
			break;
		case _lhs:
			printf(" lhs\n");
			printINDEXNODE(node->exp.lhs_exp,level + 1);
			break;
		case _int:
			printf(" int %d\n",node->exp.val);
			break;
		case _bool:
			switch(node->exp.b->operand){
				case _AND:
					printf(" and\n");
					break;
				case _OR:
					printf(" or\n");
					break;
				case _EQ:
					printf(" eq\n");
					break;
				case _NEQ:
					printf(" not eq\n");
					break;
				case _GT:
					printf(" greater\n");
					break;
				case _LT:
					printf(" less\n");
					break;
				case _GTE:
					printf(" greater/equal\n");
					break;
				case _LTE:
					printf(" less/equal\n");
					break;				
				case _NOT:
					printf(" not\n");
					break;
			}
			printEXPNODE(node->exp.b->left,level + 1);
			printEXPNODE(node->exp.b->right,level + 1);
			break;
		default:
			break;
	}
}


void free_indexnode(INDEXNODE *node){	
	if (node == NULL) return;
	free(node->id);
	free(node);
}

void free_expnode(EXPNODE *node){
	if (node == NULL) return;
	int i = 0;
	switch(node->exp_type){
		case arith:
			free_expnode(node->exp.a->right);
			free_expnode(node->exp.a->left);
			free(node->exp.a);
			break;
		case mat_dec:
			free(node->exp.md->arr);
			free(node->exp.md);
			break;
		case emp_mat_dec:
			break;
			break;
		case _lhs:
			free_indexnode(node->exp.lhs_exp);
			break;
		case _int:
			break;
		case _bool:
			free_expnode(node->exp.b->left);
			free_expnode(node->exp.b->right);
			break;
		default:
			break;
	}
	free(node);
}

void free_astnode(ASTNODE *node){
	if (node == NULL) return;
	switch(node->stmt_type){
		case assign:
			free_indexnode(node->stmt.a->var);								
			free_expnode(node->stmt.a->exp);
			free(node->stmt.a);
			break;
		case loop:
			free(node->stmt.l->var);
			free_ast(node->stmt.l->body);
			free(node->stmt.l);
			break;
		case print_exp:
			free_expnode(node->stmt.p);
			break;
		case func_dec:
			free(node->stmt.fd->name);
			free_symtab(node->stmt.fd->sym_tab);
			free_ast(node->stmt.fd->body);
			break;
		case conditional:
			free_expnode(node->stmt.c->condition);
			free_ast(node->stmt.c->true_body);
			free_ast(node->stmt.c->false_body);
			break;
		case ret:
			free_expnode(node->stmt.r);
			break;
		default:
			break;
	}
	free(node);
}

void free_ast(AST *ast){
	AST *curr;
	AST *temp;
	curr = ast;
	while(curr != NULL){
		temp = curr;
		curr = curr->next;	
		free_astnode(temp->ele);
		free(temp);
	}
	ast = NULL;
}


