#include <stdio.h>
#include <windows.h>
#include <shellapi.h>
#include <string.h>
#include "symtab.h"
#include "files.h"
#include "gen.h"

static int loopID = 0;
static int condID = 0;
static int stack_temp_size = 0;

int ilength(int a){
	int l = 0;
	if(a == 0) return 1;
	if(a < 0){ a *= -1; l++;}
	while(a){ a /= 10; l++;}
	return l;
}

char *appendLine(char * str,char *line){
	if(str == NULL){
		str = (char *)malloc(strlen(line) + 1);
		strcpy(str,line);
	}else{
		str = (char *)realloc(str,strlen(str) + strlen(line) + 1);
		strcat(str,line);
	}
	return str;
}

char *genINDEXNODE(char *code,INDEXNODE *node,SYMTAB *syms){
	int length,i,ind;
	char *line = NULL;
	SYMENTRY *sentry = findSym(syms, node->id);
	if(node->index1 == NULL){
		//A @ #,n - nth column		
		//pop eax - will be the col ind
		//[A + 8 + eax + 0*rows]
		//[A + 8 + eax + 4*rows]
		//[A + 8 + eax + 8*rows]
		//...
		code = genEXPNODE(code,node->index2,syms);
		length = strlen("pop eax\nimul eax,4\n") + 1;
		line = (char *)malloc(length * sizeof(char));
		_snprintf(line,length,"pop eax\nimul eax,4\n");
		code = appendLine(code,line);
		for(i = sentry->cols - 1;i >= 0;i--){
			free(line);
			length = strlen("push dword [+8+eax+*]\n") + strlen(sentry->name) + ilength(4*i) + ilength(sentry->rows) + 1;
			line = (char *)malloc(length * sizeof(char));
			_snprintf(line,length,"push dword [%s+8+eax+%d*%d]\n",sentry->name,4*i,sentry->rows);
			code = appendLine(code,line);
		}
	}else if(node->index2 == NULL){
		//A @ n,# - nth row
		//pop eax - will be the row ind
		//imul eax,#rows
		//[A + 8 + 0 + eax]
		//[A + 8 + 4 + eax]
		//[A + 8 + 8 + eax]
		//...
		code = genEXPNODE(code,node->index1,syms);
		length = strlen("pop eax\nimul eax,\nimul eax,4\n") + ilength(sentry->rows) + 1;
		line = (char *)malloc(length * sizeof(char));
		_snprintf(line,length,"pop eax\nimul eax,%d\nimul eax,4\n",sentry->rows);
		code = appendLine(code,line);		
		for(i = sentry->rows - 1;i >= 0;i--){
			free(line);
			length = strlen("push dword [+8++eax]\n") + strlen(sentry->name) + ilength(4*i) + 1;
			line = (char *)malloc(length * sizeof(char));			
			_snprintf(line,length,"push dword [%s+8+%d+eax]\n",sentry->name,4*i);
			code = appendLine(code,line);
		}		
	}else if(node->index1->exp_type == _int && node->index1->exp.val == -1 && node->index2->exp_type == _int && node->index2->exp.val == -1){
		for(i = sentry->rows * sentry->cols - 1;i >= 0;i--){
			ind = i*4 + 8;
			length = strlen("push dword [+]\n") + strlen(node->id) + ilength(ind) + 1;
			free(line);
			line = (char *)malloc(length*sizeof(char));
			_snprintf(line,length,"push dword [%s+%d]\n",node->id,ind);
			code = appendLine(code,line);
		}
	}else{
		code = genEXPNODE(code,node->index1,syms);
		code = genEXPNODE(code,node->index2,syms);
		length = strlen("pop eax\npop ebx\nimul eax,[]\nimul eax,4\nimul ebx,4\npush dword [+8+eax+ebx]\n") + 2*strlen(node->id) + 1;
		line = (char *)malloc(length*sizeof(char));
		_snprintf(line,length,"pop eax\npop ebx\nimul eax,[%s]\nimul eax,4\nimul ebx,4\npush dword [%s+8+eax+ebx]\n",node->id,node->id);
		code = appendLine(code,line);
	}
	free(line);
	return code;
}

char *genEXPNODE(char *code,EXPNODE *node,SYMTAB *syms){
	char *line = NULL;
	int length,i,j,lr,lc,rc,li,ri;	
	if(node == NULL) return;
	switch(node->exp_type){
		case arith:
			code = genEXPNODE(code,node->exp.a->left,syms);
			code = appendLine(code,"mov ecx,esp\n");
			code = genEXPNODE(code,node->exp.a->right,syms);
			code = appendLine(code,"mov edx,esp\n");			
			switch(node->exp.a->operand){
				case _ADD:
					for(i = node->exp.a->right->rows * node->exp.a->right->cols - 1;i >= 0;i--){
						free(line);
						length = strlen("mov eax,[ecx+]\nadd eax,[edx+]\npush eax\n") + 2*ilength(4*i) + 1;
						line = (char *)malloc(length*sizeof(char));
						_snprintf(line,length,"mov eax,[ecx+%d]\nadd eax,[edx+%d]\npush eax\n",4*i,4*i);
						code = appendLine(code,line);
					}
					break;
				case _SUB:
					for(i = node->exp.a->right->rows * node->exp.a->right->cols - 1;i >= 0;i--){
						free(line);
						length = strlen("mov eax,[ecx+]\nsub eax,[edx+]\npush eax\n") + 2*ilength(4*i) + 1;
						line = (char *)malloc(length*sizeof(char));
						_snprintf(line,length,"mov eax,[ecx+%d]\nsub eax,[edx+%d]\npush eax\n",4*i,4*i);
						code = appendLine(code,line);
					}
					break;
				case _MULT:
					lr = node->exp.a->left->rows;
					lc = node->exp.a->left->cols;
					rc = node->exp.a->right->cols;
					ri = rc - 1;
					li = lr * lc - lc;
					for(i = lr * rc - 1;i >= 0;i--){
						if(ri < 0){
							ri = rc - 1;
							li = li - lc;
						}
						free(line);
						length = strlen("mov eax,[ecx+]\nimul eax,[edx+]\n") + ilength(4*li) + ilength(4*ri) + 1;
						line = (char *)malloc(length*sizeof(char));
						_snprintf(line,length,"mov eax,[ecx+%d]\nimul eax,[edx+%d]\n",4*li,4*ri);
						code = appendLine(code,line);
						for(j = lc - 1;j >= 1;j--){
							free(line);
							length = strlen("mov ebx,[ecx+]\nimul ebx,[edx+]\nadd eax,ebx\n") + ilength(4*li + 4*j) + ilength(4*ri + 4*j*rc) + 1;
							line = (char *)malloc(length*sizeof(char));
							_snprintf(line,length,"mov ebx,[ecx+%d]\nimul ebx,[edx+%d]\nadd eax,ebx\n",4*li + 4*j,4*ri + 4*j*rc);
							code = appendLine(code,line);
						}
						ri--;
						code = appendLine(code,"push eax\n");						
					}
					break;
				case _DIV:				
					code = appendLine(code,"mov edx,0\npop eax\npop ebx\ndiv ebx\npush eax\n");
					break;
				default:
					break;			
			}
			break;
		case mat_dec:
			for(i = node->exp.md->ncols * node->exp.md->nrows - 1;i >= 0;i--){
				length = strlen("push \n") + ilength(node->exp.md->arr[i]) + 1;
				free(line);
				line = (char *)malloc(length*sizeof(char));
				_snprintf(line,length,"push %d\n",node->exp.md->arr[i]);
				code = appendLine(code,line);
			}
			break;
		case emp_mat_dec:
			if (node->rows && node->cols){ //2 reals
				for (i = node->rows * node->cols - 1; i >= 0; i--)
					code = appendLine(code, "push 0\n");
			} else if (node->rows){ //real rows
				asdf
			}
			else if (node->cols){ //real cols

			}
			else{ //2 variables

			}
			break;
		case _lhs:
			code = genINDEXNODE(code,node->exp.lhs_exp,syms);
			break;
		case _int:
			length = strlen("push \n") + ilength(node->exp.val) + 1;
			line = (char *)malloc(length*sizeof(char));
			_snprintf(line,length,"push %d\n",node->exp.val);
			code = appendLine(code,line);
			break;
		case _bool:
			code = genEXPNODE(code,node->exp.b->left,syms);
			if(node->exp.b->operand == _NOT)
				code = appendLine(code,"pop eax\n");		
			else{
				code = genEXPNODE(code,node->exp.b->right,syms);
				code = appendLine(code,"pop eax\npop ebx\n");
			}
			switch(node->exp.b->operand){
				case _AND:
					code = appendLine(code,"and eax,ebx\n");
					break;
				case _OR:
					code = appendLine(code,"or eax,ebx\n");
					break;
				case _EQ:

					break;
				case _NEQ:
					break;
				case _GT:
					break;
				case _LT:
					break;
				case _GTE:
					break;
				case _LTE:
					break;
				case _NOT:
					code = appendLine(code,"not eax\n");
					break;
				default:
					break;
			}
			break;
		default:
			break;	
	}
	free(line);
	return code;
}

char *genASTNODE(char *code,ASTNODE *node,SYMTAB *syms){	
	char *line = (char *)malloc(1);
	char *id;
	int length,i,j;
	SYMENTRY *sentry;
	assign_stmt *as;
	switch(node->stmt_type){
		case assign: //assign
			code = genEXPNODE(code,node->stmt.a->exp,syms);
			as = node->stmt.a;
			sentry = findSym(syms, as->var->id);
			if(as->var->index1 == NULL){
				//A @ #,n - nth column		
				//pop eax - will be the col ind
				//[A + 8 + eax + 0*rows]
				//[A + 8 + eax + 4*rows]
				//[A + 8 + eax + 8*rows]
				//...
				code = genEXPNODE(code,as->var->index2,syms);
				length = strlen("pop eax\nimul eax,4\n") + 1;
				line = (char *)malloc(length * sizeof(char));
				_snprintf(line,length,"pop eax\nimul eax,4\n");
				code = appendLine(code,line);
				for(i = 0;i < sentry->rows;i++){
					free(line);
					length = strlen("pop dword [+8+eax+*]\n") + strlen(sentry->name) + ilength(4*i) + ilength(sentry->cols) + 1;
					line = (char *)malloc(length * sizeof(char));
					_snprintf(line,length,"pop dword [%s+8+eax+%d*%d]\n",sentry->name,4*i,sentry->cols);
					code = appendLine(code,line);
				}
			}else if(as->var->index2 == NULL){
				//A @ n,# - nth row
				//pop eax - will be the row ind
				//imul eax,#rows
				//[A + 8 + 0 + eax]
				//[A + 8 + 4 + eax]
				//[A + 8 + 8 + eax]
				//...
				code = genEXPNODE(code,as->var->index1,syms);
				length = strlen("pop eax\nimul eax,\nimul eax,4\n") + ilength(sentry->cols) + 1;
				line = (char *)malloc(length * sizeof(char));
				_snprintf(line,length,"pop eax\nimul eax,%d\nimul eax,4\n",sentry->cols);
				code = appendLine(code,line);		
				for(i = 0;i < sentry->cols;i++){
					free(line);
					length = strlen("pop dword [+8++eax]\n") + strlen(sentry->name) + ilength(4*i) + 1;
					line = (char *)malloc(length * sizeof(char));			
					_snprintf(line,length,"pop dword [%s+8+%d+eax]\n",sentry->name,4*i);
					code = appendLine(code,line);
				}
			}else if(as->var->index1->exp_type == _int && as->var->index1->exp.val == -1 
				&& as->var->index2->exp_type == _int && as->var->index2->exp.val == -1){
				for(i = 0;i < as->exp->rows * as->exp->cols;i++){
					length = strlen("pop dword [+]\n") + strlen(as->var->id) + ilength(4*i + 8) + 1;
					free(line);
					line = (char *)malloc(length*sizeof(char));
					_snprintf(line,length,"pop dword [%s+%d]\n",as->var->id,4*i + 8);
					code = appendLine(code,line);
				}
			}else{
				//A @ x,y = exp
				//A[8 + index1*rows + index2] = exp
				//pop dword [A + 8 + index1*rows + index2]
				code = genEXPNODE(code,as->var->index1,syms);
				code = genEXPNODE(code,as->var->index2,syms);
				length = strlen("pop eax\npop ebx\nimul eax,[]\nimul eax,4\nimul ebx,4\npop dword [+8+eax+ebx]\n") + 2*strlen(as->var->id) + 1;
				line = (char *)malloc(length*sizeof(char));
				_snprintf(line,length,"pop eax\npop ebx\nimul eax,[%s]\nimul eax,4\nimul ebx,4\npop dword [%s+8+eax+ebx]\n",as->var->id,as->var->id);
				code = appendLine(code,line);
			}
			free(line);
			break;
		case loop: //loop
			i = loopID++;
			id = node->stmt.l->var;
			length = strlen("mov dword [+8],\n.loop:\nmov ecx,\nmov ebx,[+8]\ncmp ebx,ecx\njg .end\n") + 2*strlen(id) + ilength(node->stmt.l->start) + ilength(node->stmt.l->end) + 2*ilength(i) + 1;
			line = (char *)malloc(length*sizeof(char));
			_snprintf(line,length,"mov dword [%s+8],%d\n.loop%d:\nmov ecx,%d\nmov ebx,[%s+8]\ncmp ebx,ecx\njg .end%d\n",id,node->stmt.l->start,i,node->stmt.l->end,id,i);
			code = appendLine(code,line);
			
			code = genAST(code,node->stmt.l->body,syms);
			
			free(line);
			length = strlen("add dword [+8],\njmp .loop\n.end:\n") + strlen(id) + 2*ilength(i) + 1 + ilength(node->stmt.l->change);
			line = (char *)malloc(length*sizeof(char));
			_snprintf(line,length,"add dword [%s+8],%d\njmp .loop%d\n.end%d:\n",id,node->stmt.l->change,i,i);
			code = appendLine(code,line);
			free(line);
			break;
		case print_exp: //print exp
			code = genEXPNODE(code,node->stmt.p,syms);
			for(i = 0;i < node->stmt.p->rows;i++){
				for(j = 0;j < node->stmt.p->cols;j++){
					code = appendLine(code,"pop eax\ncinvoke printf,print_int,eax\n");
				}
				code = appendLine(code,"cinvoke printf,print_char,''\n");
			}
			code = appendLine(code,"cinvoke printf,print_char,''\n");
			break;
		case func_dec:
			break;
		case conditional:
			i = condID++;
			code = genEXPNODE(code,node->stmt.c->condition,syms);
			length = strlen("pop eax\npop ebx\ncmp eax,ebx\njne .condfalse\n") + ilength(i) + 1;
			line = (char *)malloc(length*sizeof(char));
			_snprintf(line,length,"pop eax\npop ebx\ncmp eax,ebx\njne .condfalse%d\n",i);
			code = appendLine(code,line);
			code = genAST(code,node->stmt.c->true_body,syms);
			length = strlen("jmp .condend\n.condfalse:\n") + 2*ilength(i) + 1;
			free(line);
			line = (char *)malloc(length * sizeof(char));
			_snprintf(line,length,"jmp .condend%d\n.condfalse%d:\n",i,i);
			code = appendLine(code,line);
			code = genAST(code,node->stmt.c->false_body,syms);
			length = strlen(".condend:\n") + ilength(i) + 1;
			free(line);
			line = (char *)malloc(length * sizeof(char));
			_snprintf(line,length,".condend%d:\n",i);
			code = appendLine(code,line);
			break;
		case ret:
			break;
		default:
			break;
	}
	return code;
}

char *genAST(char * code,AST *ast,SYMTAB *syms){
	AST *curr = ast;
	while(curr != NULL){
		code = genASTNODE(code,curr->ele,syms);
		curr = curr->next;
	}
	return code;
}

//variables has largest size ever used in program
char *genVars(char *decs,SYMTAB *syms){
	if (syms == NULL) return NULL;
	char *line,*name;
	int length;
	int i,j,r,c;
	SYMENTRY *curr;
	for (i = 0; i < syms->numSyms; i++){
		curr = syms->data[i];
		if (curr->isFunc) continue;
		name = curr->name;
		r = curr->rows;
		c = curr->cols;
		length = 0;
		length = strlen(name) + strlen(" MATRIX ") + ilength(r) + strlen(",") + ilength(c) + strlen(",<") + 1;
		line = (char *)malloc(length*sizeof(char));
		_snprintf(line, length, "%s MATRIX %d,%d,<", name, r, c);
		for (j = 0; j < r * c - 1; j++)
			line = appendLine(line, "0,");
		line = appendLine(line, "0>\n");
		decs = appendLine(decs, line);
	}
	return decs;
}

void gen_main(char *filename,AST *ast,SYMTAB *syms){	
	char *code = NULL;
	char *decs = NULL;
	char *asm_filename = NULL;
	char *ptr = (char *)strchr(filename,'.');
	int ind = ptr - filename;
	asm_filename = (char *)malloc(ind + 5);
	strncpy(asm_filename,filename,ind);
	strcpy(asm_filename + ind,".asm");
	
	printf("generating assembly...\n");
	code = genAST(code,ast,syms);
	decs = genVars(decs,syms);
	
	printf("creating %s...\n",asm_filename);
	clearFile(asm_filename);
	
	printf("writing assembly to %s...\n",asm_filename);
	writeLine(asm_filename,ASM_HEADER);
	writeLine(asm_filename,code);
	writeLine(asm_filename,ASM_FOOTER);
	if(decs != NULL) writeLine(asm_filename,decs);
	
	printf("assembling %s...\n",asm_filename);
	ShellExecute(NULL,"open","fasm.exe",asm_filename,NULL,SW_SHOWNORMAL);
		
	printf("done\n");
	
	free(code);
	free(decs);
	free(asm_filename);
}