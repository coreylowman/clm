#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "clm_code_gen.h"
#include "clm_asm_headers.h"
#include "util/clm_expression.h"
#include "util/clm_statement.h"
#include "util/clm_type.h"
#include "util/clm_type_of.h"

typedef struct{
    char *code;
    ClmScope *scope;
    int labelID;
    int inFunction;
} CodeGenData;

static CodeGenData data;

#define EAX "eax"
#define EBX "ebx"
#define ECX "ecx"
#define EDX "edx"
#define ESP "esp"
#define EBP "ebp"
#define T_EAX "T_EAX"
#define T_EBX "T_EBX"

static void writeLine(const char *line){
    data.code = realloc(data.code, strlen(data.code) + strlen(line) + 1);
    strcat(data.code, line);
}

static void asm_pop(const char *dest){
    char buffer[32];
    sprintf(buffer, "pop %s\n", dest);
    writeLine(buffer);
}

static void asm_push(const char *src){
    char buffer[32];
    sprintf(buffer, "push %s\n", src);
    writeLine(buffer);
}

static void asm_push_i(int val){
    char buffer[32];
    sprintf(buffer, "push %d\n", val);
    writeLine(buffer);
}

static void asm_push_f(float val){
    char buffer[32];
    sprintf(buffer, "push %f\n", val);
    writeLine(buffer);   
}

static void asm_push_c(char val){
    char buffer[32];
    sprintf(buffer, "push %c\n", val);
    writeLine(buffer);
}

static void asm_add(const char *dest, const char *other){
    char buffer[32];
    sprintf(buffer, "add %s,%s\n", dest, other);
    writeLine(buffer);
}

static void asm_add_i(const char *dest, int i){
    char buffer[32];
    sprintf(buffer, "add %s,%d\n", dest, i);
    writeLine(buffer);
}

static void asm_sub(const char *dest, const char *other){
    char buffer[32];
    sprintf(buffer, "sub %s,%s\n", dest, other);
    writeLine(buffer);
}

static void asm_imul(const char *dest, const char *other){
    char buffer[32];
    sprintf(buffer, "imul %s,%s\n", dest, other);
    writeLine(buffer);
}

static void asm_fadd(const char *dest, const char *other){

}

static void asm_fsub(const char *dest, const char *other){

}

static void asm_fmul(const char *dest, const char *other){

}

static void asm_inc(const char *arg){
    char buffer[32];
    sprintf(buffer, "inc %s\n", arg);
    writeLine(buffer);
}

static void asm_dec(const char *arg){
    char buffer[32];
    sprintf(buffer, "dec %s\n", arg);
    writeLine(buffer);
}

static void asm_neg(const char *arg){
    char buffer[32];
    sprintf(buffer, "dec %s\n", arg);
    writeLine(buffer);
}

static void asm_mov(const char *dest, const char *src){
    char buffer[32];
    sprintf(buffer, "mov %s,%s\n", dest, src);
    writeLine(buffer);
}

static void asm_mov_i(const char *dest, int i){
	char buffer[32];
	sprintf(buffer, "mov %s,%d\n", dest, i);
	writeLine(buffer);
}

static void asm_lea(const char *dest, const char *src){
	char buffer[32];
	sprintf(buffer, "lea %s,%s\n", dest, src);
	writeLine(buffer);
}

static void asm_xchg(const char *arg1, const char *arg2){

}

static void asm_and(const char *arg1, const char *arg2){

}

static void asm_or(const char *arg1, const char *arg2){

}

static void asm_xor(const char *arg1, const char *arg2){

}

static void asm_cmp(const char *arg1, const char *arg2){
    char buffer[32];
    sprintf(buffer, "cmp %s,%s\n", arg1,arg2);
    writeLine(buffer);
}

static void asm_jmp(const char *label){
    char buffer[32];
    sprintf(buffer, "jmp %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_g(const char *label){
    char buffer[32];
    sprintf(buffer, "jmp %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_ge(const char *label){
    char buffer[32];
    sprintf(buffer, "jge %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_l(const char *label){
    char buffer[32];
    sprintf(buffer, "jl %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_le(const char *label){
    char buffer[32];
    sprintf(buffer, "jle %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_eq(const char *label){
    char buffer[32];
    sprintf(buffer, "je %s\n", label);
    writeLine(buffer);
}

static void asm_jmp_neq(const char *label){
    char buffer[32];
    sprintf(buffer, "jne %s\n", label);
    writeLine(buffer);
}

static void asm_label(const char *name){
    char buffer[32];
    sprintf(buffer, "%s:\n", name);
    writeLine(buffer);
}

static void asm_call(const char *name){
    char buffer[32];
    sprintf(buffer, "call _%s\n", name);
    writeLine(buffer);
}

static void asm_ret(){
    char buffer[32];
    sprintf(buffer, "ret\n");
    writeLine(buffer);
}

static void asm_print_mat(const char *src, int nl){

}

static void asm_print_const_mat(float *arr, int num_elements, int nl){

}

static  void asm_print_float(const char *src, int nl){
	char buffer[64];
	if (nl)
		sprintf(buffer, "cinvoke printf, print_float_nl, %s\n", src);
	else
		sprintf(buffer, "cinvoke printf, print_float, %s\n", src);
	writeLine(buffer);
}

static void asm_print_const_float(float f, int nl){

}

static void asm_print_int(const char *src, int nl){
	char buffer[64];
	if (nl)
		sprintf(buffer, "cinvoke printf, print_int_nl, %s\n", src);
	else
		sprintf(buffer, "cinvoke printf, print_int, %s\n", src);
	writeLine(buffer);
}

static void asm_print_const_int(int i, int nl){

}

static void asm_print_char(const char *src, int nl){
	char buffer[64];
	if (nl)
		sprintf(buffer, "cinvoke printf, print_char_nl, %s\n", src);
	else
		sprintf(buffer, "cinvoke printf, print_char, %s\n", src);
	writeLine(buffer);
}

static void asm_print_const_char(char c, int nl){

}

static void next_label(char *buffer){
    int id = data.labelID++;
    sprintf(buffer, "label%d", id);
}

/*
 *
 *  FUNCTION FORWARD DECLARATIONS
 *
 */
static void gen_arith(ClmArithExp *node);
static void gen_bool(ClmBoolExp *node);
static void gen_unary(ClmUnaryExp *node);

static void gen_lhs(ClmExpNode *node);
static void gen_matrix_size(ClmExpNode *node);
static void gen_expression(ClmExpNode *node);
static void gen_statement(ClmStmtNode *node);

static void gen_functions(ClmArrayList *statements);
static void gen_statements(ClmArrayList *statements);

// stack will look like this
// right
// right type
// left <- edx
// left type
// <- esp
// so [esp-4] is the left type, and [edx-4] is the right type
static void gen_arith(ClmArithExp *node){
    //TODO all of these
    switch(node->operand){
    case ARITH_OP_ADD:
        break;
    case ARITH_OP_SUB:
        break;
    case ARITH_OP_MULT:
        break;
    case ARITH_OP_DIV:
        break;
    }
}

static void gen_bool(ClmBoolExp *node){
    switch(node->operand){
    case BOOL_OP_AND:
        asm_pop(EAX); //pop type into eax
        asm_pop(EAX); //ovewrite type with val
        asm_pop(EBX); //pop type into ebx
        asm_pop(EBX); //overwrite type iwth val
        asm_and(EAX, EBX);
        asm_push(EAX);
        break;
    case BOOL_OP_OR:
        asm_pop(EAX);
        asm_pop(EAX);
        asm_pop(EBX);
        asm_pop(EBX);
        asm_or(EAX, EBX);
        asm_push(EAX);
        break;
    case BOOL_OP_EQ:
    //TODO equals!
        break;
    case BOOL_OP_NEQ:
    //TODO not equals! should be the same as eq but with neg
        break;
    case BOOL_OP_GT:
    {
        char end_label[256];
        char false_label[256];
        next_label(false_label);
        next_label(end_label);

        asm_pop(EAX);
        asm_pop(EAX);
        asm_pop(EBX);
        asm_pop(EBX);
        asm_cmp(EAX, EBX);
        asm_jmp_le(false_label);
        asm_push_i(1);
        asm_jmp(end_label);
        asm_label(false_label);
        asm_push_i(0);
        asm_label(end_label);
        break;
    }
    case BOOL_OP_LT:
    {
        char end_label[256];
        char false_label[256];
        next_label(false_label);
        next_label(end_label);

        asm_pop(EAX);
        asm_pop(EAX);
        asm_pop(EBX);
        asm_pop(EBX);
        asm_cmp(EAX, EBX);
        asm_jmp_ge(false_label);
        asm_push_i(1);
        asm_jmp(end_label);
        asm_label(false_label);
        asm_push_i(0);
        asm_label(end_label);
        break;
    }
    case BOOL_OP_GTE:
    {
        char end_label[256];
        char false_label[256];
        next_label(false_label);
        next_label(end_label);

        asm_pop(EAX);
        asm_pop(EAX);
        asm_pop(EBX);
        asm_pop(EBX);
        asm_cmp(EAX, EBX);
        asm_jmp_l(false_label);
        asm_push_i(1);
        asm_jmp(end_label);
        asm_label(false_label);
        asm_push_i(0);
        asm_label(end_label);
        break;
    }
    case BOOL_OP_LTE:
    {
        char end_label[256];
        char false_label[256];
        next_label(false_label);
        next_label(end_label);

        asm_pop(EAX);
        asm_pop(EAX);
        asm_pop(EBX);
        asm_pop(EBX);
        asm_cmp(EAX, EBX);
        asm_jmp_g(false_label);
        asm_push_i(1);
        asm_jmp(end_label);
        asm_label(false_label);
        asm_push_i(0);
        asm_label(end_label);
        break;
    }
    }
}

static void gen_unary(ClmUnaryExp *node){
    switch(node->operand){
    case UNARY_OP_MINUS:
    //TODO foreach element... neg it
    //cols
    //rows
    //type 
    // <- esp
    //mov ecx, [esp + 8]
    //mul ecx, [esp + 12] ; now ecx contains rows * cols
    //loop0:
    //dec ecx
    //cmp ecx, -1
    //je loop1
    //neg dword [esp + ecx]
    //jmp loop0
    //loop1:
        break;
    case UNARY_OP_TRANSPOSE:
    //TODO transpose it!
        break;
    case UNARY_OP_NOT:
        asm_xor("dword [" ESP "]", "1");
        break;
    }
}

/* pushes the number of column and then the number of rows */
static void gen_matrix_size(ClmExpNode *node){
	char index_str[32];
    //this is a matDecExp
    if(node->matDecExp->colVar != NULL){
        //push dword [ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp->colVar);
		sprintf(index_str, "dword [ebp+%d]", sym->offset + 8);
        asm_push(index_str);
    }else{
        //push $colInd
        asm_push_i(node->matDecExp->cols);
    }

    if(node->matDecExp->rowVar != NULL){
        //push dword [ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp->rowVar);
        sprintf(index_str, "dword [ebp+%d]", sym->offset + 4);
        asm_push(index_str);
    }else{
        //push $rowInd
        asm_push_i(node->matDecExp->rows);
    }

}

static void gen_lhs(ClmExpNode *node){
    //it is an index node - otherwise it is a type check fail
    char index_str[64];
    ClmSymbol *var = clm_scope_find(data.scope, node->indExp->id);

    switch(var->type){
    case CLM_TYPE_INT:
        asm_pop(EAX); //pop type
        if(var->isParam)
            sprintf(index_str, "dword [ebp+%d]",var->offset + 4);
        else
            sprintf(index_str, "dword [ebp+%d]",var->offset - 4);
        asm_pop(index_str);
        break;
    case CLM_TYPE_FLOAT:
		//TODO floats
        break;
    case CLM_TYPE_MATRIX:
    {
        ClmExpNode *rowIndex = node->indExp->rowIndex;
        ClmExpNode *colIndex = node->indExp->colIndex;
        if(rowIndex == NULL && colIndex == NULL){
            //TODO pop whole matrix off stack
			char start_label[32];
			char end_label[32];
			next_label(start_label);
			next_label(end_label);

			asm_pop(EAX); //pop type
			asm_pop(ECX); //pop rows
			asm_pop(EBX); //pop cols;
			asm_imul(ECX, EBX); //eax now contains rows * cols
			asm_label(start_label);
			asm_dec(ECX);
			asm_cmp(ECX, "0");
			asm_jmp_eq(end_label);

			sprintf(index_str, "dword [ebp+%d+ecx]", var->offset);
			asm_pop(index_str);

			asm_jmp(start_label);
			asm_label(end_label);
        }else if(rowIndex == NULL){
            //TODO pop a whole row off the stack
        }else if(colIndex == NULL){
            //TODO pop a whole col off the stack
        }else{
            //TODO pop one val off the stack
            gen_expression(colIndex);			
            gen_expression(rowIndex);
			asm_pop(EAX); //pop type
            asm_pop(EAX);
			asm_pop(EBX); //pop type
			asm_pop(EBX);
            asm_imul(EAX, EBX);
            asm_imul(EAX, "4"); //now eax contains row * col * 4
            //var->offset points at type... 3 elements above is the first item in the matrix
            sprintf(index_str, "dword [ebp+%d+eax]", var->offset + 12);
            asm_pop(index_str);
        }
        break;
    }
    case CLM_TYPE_MATRIX_POINTER:
    {
        ClmExpNode *rowIndex = node->indExp->rowIndex;
        ClmExpNode *colIndex = node->indExp->colIndex;
        if(rowIndex == NULL && colIndex == NULL){
            //TODO push whole matrix on stack
        }else if(rowIndex == NULL){
            //TODO push a whole row onto the stack
        }else if(colIndex == NULL){
            //TODO push a whole col onto the stack
        }else{
            //TODO push one val onto the stack
        }
        break;
    }
    case CLM_TYPE_STRING:
        //uhh
        break;
    case CLM_TYPE_FUNCTION:
    case CLM_TYPE_NONE:
        //shouldn't get here...
        break;
    }
}

//stack should look like this:
// val
// type
static void gen_expression(ClmExpNode *node){
    ClmType expression_type = clm_type_of_exp(node, data.scope);
    switch(node->type){
    case EXP_TYPE_INT:
        asm_push_i(node->ival);
        asm_push_i((int) expression_type);
        break;
    case EXP_TYPE_FLOAT:
        asm_push_f(node->fval);
        asm_push_i((int) expression_type);
        break;
    case EXP_TYPE_STRING:
        break;
    case EXP_TYPE_ARITH:
        gen_expression(node->arithExp->right);
        asm_mov(EDX, ESP);
        gen_expression(node->arithExp->left);
        gen_arith(node->arithExp);
        break;
    case EXP_TYPE_BOOL:
        gen_expression(node->boolExp->right);
        asm_mov(EDX, ESP);
        gen_expression(node->boolExp->left);
        gen_bool(node->boolExp);
        break;
    case EXP_TYPE_CALL:
    {
        int i;
        for(i = node->callExp->params->length - 1;i >= 0;i--){
            gen_expression(node->callExp->params->data[i]);
        }
        asm_call(node->callExp->name);
        break;
    }
    case EXP_TYPE_INDEX:
    {
        char index_str[64];
        ClmSymbol *var = clm_scope_find(data.scope, node->indExp->id);
        switch(var->type){
        case CLM_TYPE_INT:
            if(var->isParam)
                sprintf(index_str, "dword [ebp+%d]",var->offset + 4);
            else
                sprintf(index_str, "dword [ebp+%d]",var->offset - 4);
            asm_push(index_str);
            asm_push_i((int) var->type);
            break;
        case CLM_TYPE_FLOAT:
            //TODO floats
            break;
        case CLM_TYPE_MATRIX:
        {
            if(node->indExp->rowIndex == NULL && node->indExp->colIndex == NULL){
                //TODO push whole matrix on stack
            }else if(node->indExp->rowIndex == NULL){
                //TODO push a whole row onto the stack
            }else if(node->indExp->colIndex == NULL){
                //TODO push a whole col onto the stack
            }else{
                //TODO push one val onto the stack
				gen_expression(node->indExp->colIndex);
				gen_expression(node->indExp->rowIndex);
				asm_pop(EAX); //pop type
				asm_pop(EAX); //pop row index into eax
				asm_pop(EBX); //pop type
				asm_pop(EBX); //pop col index into ebx
				asm_imul(EAX, EBX);
				asm_imul(EAX, "4"); //now eax contains row * col * 4
				//var->offset points at type... 3 elements above is the first item in the matrix
				//sprintf(index_str, "dword [ebp+%d+eax]", var->offset + 12);
				sprintf(index_str, "[ebp+%d]", var->offset + 4);
				asm_mov(EBX, index_str);
                //asm_add(EAX, EBX); //eax now contains              
				asm_push("dword [eax]");
				asm_push_i((int)CLM_TYPE_INT);
            }
            break;
        }
        case CLM_TYPE_MATRIX_POINTER:
        {
            if(node->indExp->rowIndex == NULL && node->indExp->colIndex == NULL){
                //TODO push whole matrix on stack
            }else if(node->indExp->rowIndex == NULL){
                //TODO push a whole row onto the stack
            }else if(node->indExp->colIndex == NULL){
                //TODO push a whole col onto the stack
            }else{
                //TODO push one val onto the stack
            }
            break;
        }
        case CLM_TYPE_STRING:
            //uhh
            break;
        case CLM_TYPE_FUNCTION:
        case CLM_TYPE_NONE:
            //shouldn't get here...
            break;
        }
        break;
    }
    case EXP_TYPE_MAT_DEC:
    {
        int i;        
        if(node->matDecExp->arr != NULL){
            for(i = node->matDecExp->length - 1; i >= 0;i--){
                //TODO... push f or push i?
                asm_push_f(node->matDecExp->arr[i]);
            }
            asm_push_i(node->matDecExp->cols);
            asm_push_i(node->matDecExp->rows);
            asm_push_i((int) expression_type);
        }else{
            //TODO figure out where to allocate this??
            //WAIT! we don't need to allocate???
            //just have a for loop that pushes 0 onto the stack
            //d'oh
            //this is already handled by local var allocation in
            //function dec
            char index_str[32];            
            if(node->matDecExp->rowVar != NULL){
                //mov eax,[ebp+offset]
                ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp->rowVar);
                sprintf(index_str, "[ebp+%d]", sym->offset);
                asm_mov(EAX, index_str);
            }else{
                //mov eax,$rowInd
                sprintf(index_str, "%d", node->matDecExp->rows);
                asm_mov(EAX, index_str);
            }

            if(node->matDecExp->colVar != NULL){
                //mul ebx,[ebp+offset]
                ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp->colVar);
                sprintf(index_str, "[ebp+%d]", sym->offset);
                asm_mov(EBX, index_str);
            }else{
                //mul ebx,$colInd
                sprintf(index_str, "%d", node->matDecExp->cols);
                asm_mov(EBX, index_str);
            }
            char start_label[32];
            next_label(start_label);
            char end_label[32];
            next_label(end_label);

            asm_mov(ECX,EBX);
            asm_imul(ECX, EBX);
            asm_label(start_label);
            asm_dec(ECX);
            asm_cmp(ECX, "0");
            asm_jmp_eq(end_label);
            asm_push_i(0);
            asm_jmp(start_label);
            asm_label(end_label);
            asm_push(EBX);
            asm_push(EAX);
            asm_push_i((int) expression_type);
        }
        break;
    }
    case EXP_TYPE_PARAM:
    //TODO ?
        break;
    case EXP_TYPE_UNARY:
        gen_expression(node->unaryExp->node);
        gen_unary(node->unaryExp);
        break;
    }
}

static void gen_print(ClmExpNode *node, int newline){
    switch(node->type){
    case EXP_TYPE_INT:
        asm_print_const_int(node->ival,newline);
        break;
    case EXP_TYPE_FLOAT:
        asm_print_const_float(node->fval, newline);
        break;
    case EXP_TYPE_STRING:
		//TODO print string
        break;
    case EXP_TYPE_ARITH:
		gen_expression(node);
		//TODO print result
        break;
    case EXP_TYPE_BOOL:
		gen_expression(node);
		//TODO print result
        break;
    case EXP_TYPE_CALL:
		gen_expression(node);
		//TODO print result
        ClmSymbol *function = clm_scope_find(data.scope, node->callExp->name);
        switch(function->type){
        case CLM_TYPE_INT:
            break;
        case CLM_TYPE_MATRIX:
            break;
        case CLM_TYPE_MATRIX_POINTER:
            break;
        case CLM_TYPE_STRING:
            break;
        case CLM_TYPE_FLOAT:
            break;
        }
        break;
	case EXP_TYPE_INDEX:
	{
		char index_str[32];
		ClmSymbol *var = clm_scope_find(data.scope, node->indExp->id);		
		switch (var->type){
		case CLM_TYPE_INT:
            gen_expression(node);
			asm_pop(EAX); //pop type
            asm_pop(EAX); //pop val into eax
			asm_print_int(EAX, newline);
            break;
		case CLM_TYPE_FLOAT:
            gen_expression(node);
			asm_pop(EAX); //pop type
            asm_pop(EAX); //pop val into eax
			asm_print_float(EAX, newline);
			break;
		case CLM_TYPE_MATRIX:
		case CLM_TYPE_MATRIX_POINTER:
		{
            //these two types should be the same...
            //they are generated differently...
            //but gen_expression will push all the vals onto the stack
            gen_expression(node);
            if (node->indExp->rowIndex != NULL && node->indExp->colIndex != NULL){
                asm_pop(EAX); //pop type
                asm_pop(EAX); //pop val
                asm_print_int(EAX, newline);
            }else{
                //for all other cases we wil have to pop the rows and cols
                //off the stack
                //TODO print matrix
            }
			break;
		}
		case CLM_TYPE_STRING:
			//this will be similar to printing a matrix, but will always be
            //one dimensional, so will just pop off length and then do a for
            //loop to print each char
            //TODO print string
			break;
		case CLM_TYPE_FUNCTION:
		case CLM_TYPE_NONE:
			//can't print these :)
			break;
		}
		break;
	}
    case EXP_TYPE_MAT_DEC:
        break;
    case EXP_TYPE_PARAM:
        break;
    case EXP_TYPE_UNARY:
        break;
    }
}

static void gen_statement(ClmStmtNode *node){
    switch(node->type){
    case STMT_TYPE_ASSIGN:
        gen_expression(node->assignStmt->rhs);
        gen_lhs(node->assignStmt->lhs);
        break;
    case STMT_TYPE_CALL:
        gen_expression(node->callExpr);
        break;
    case STMT_TYPE_CONDITIONAL:
    {
        if(node->conditionStmt->falseBody != NULL){
            char end_label[32];
            next_label(end_label);
            ClmScope *trueScope = clm_scope_find_child(data.scope, node->conditionStmt->trueBody);
            
            asm_pop(EAX);
            asm_cmp(EAX, "1");
            asm_jmp_neq(end_label);
            data.scope = trueScope;
            gen_statements(node->conditionStmt->trueBody);
            asm_label(end_label);

            data.scope = trueScope->parent;
        }else{
            char end_label[32];
            char false_label[32];
            next_label(end_label);
            next_label(false_label);
            ClmScope *trueScope = clm_scope_find_child(data.scope, node->conditionStmt->trueBody);
            ClmScope *falseScope = clm_scope_find_child(data.scope, node->conditionStmt->falseBody);
            
            asm_pop(EAX);
            asm_cmp(EAX, "1");
            asm_jmp_neq(false_label);
            data.scope = trueScope;
            gen_statements(node->conditionStmt->trueBody);            
            asm_jmp(end_label);
            asm_label(false_label);            
            data.scope = falseScope;
            gen_statements(node->conditionStmt->falseBody);
            asm_label(end_label);

            data.scope = falseScope->parent;
        }
        break;
    }
    case STMT_TYPE_FUNC_DEC:
    {
		int i;
        char func_label[32];
		
        sprintf(func_label, "_%s", node->funcDecStmt->name);
        ClmScope *funcScope = clm_scope_find_child(data.scope, node);

        asm_label(func_label);
        asm_push(EBP);
        asm_mov(EBP, ESP);

        int local_var_size = 2 * 4 * (funcScope->symbols->length - node->funcDecStmt->parameters->length);
        char local_var_size_str[32];
        sprintf(local_var_size_str, "%d", local_var_size);
        asm_sub(ESP, local_var_size_str);
		
		//each local var has 2 slots on the stack, their type and the value
		//for matrices, the value is a pointer to the location on the stack
		//these are all declared below the local variables
		ClmSymbol *sym;
		ClmStmtNode *dec;
		char start_label[32];
		char end_label[32];
		char index_str[32];
		for (i = 0; i < funcScope->symbols->length; i++){
			sym = funcScope->symbols->data[i];
			dec = sym->declaration;

			if (sym->isParam) continue;
			writeLine("; setting type on stack\n");
			//setting the type of the local var
			sprintf(index_str, "dword [ebp+%d]", sym->offset);
			asm_mov_i(index_str, (int)sym->type);

			writeLine("; setting val on stack\n");
			//setting the value of the local var
			sprintf(index_str, "dword [ebp+%d]", sym->offset - 4);
			asm_mov_i(index_str, 0);

			if (clm_type_is_matrix(sym->type)){
				writeLine("; local var is matrix\n");
				next_label(start_label);
				next_label(end_label);

				gen_matrix_size(dec->assignStmt->rhs);
				asm_pop(EAX); //eax contains num of rows
				asm_pop(EBX); //ebx contains num of cols
				asm_mov(ECX, EAX);
				asm_imul(ECX, EBX);				
				asm_label(start_label);
				asm_dec(ECX);
				asm_cmp(ECX, "0");
				asm_jmp_eq(end_label);
				asm_push_i(0);
				asm_jmp(start_label);
				asm_label(end_label);
				asm_push(EBX); //cols
				
				//setting the pointer to point at the rows
				//note: push changes the value at esp and THEN
				// decrements it
				sprintf(index_str, "dword [ebp+%d]", sym->offset - 4);
				asm_mov(index_str, ESP);

				asm_push(EAX); //rows
			}			
		}
        //TODO figure out strings though!

        data.inFunction = 1;
        data.scope = funcScope;
        gen_statements(node->funcDecStmt->body);
        data.scope = funcScope->parent;
        data.inFunction = 0;

        if(node->funcDecStmt->returnRows == -2){
            //no return value!
            asm_mov(ESP,EBP);
            asm_pop(EBP);
        }
        asm_ret();
        break;
    }
    case STMT_TYPE_LOOP:
    {
        char start_label[32];
        char end_label[32];
        next_label(start_label);
        next_label(end_label);

        ClmSymbol *var = clm_scope_find(data.scope, node->loopStmt->varId);
        char loop_var[32];
        sprintf(loop_var, "dword [ebp+%d]", var->offset);

        gen_expression(node->loopStmt->start);// don't need to store this - just evaluate and put into loop var
        asm_pop(loop_var);
        //TODO start/end inclusive
        asm_label(start_label);
        gen_expression(node->loopStmt->end); // don't need to store this - just evaulate every loop

        asm_pop(EAX);
        asm_cmp(loop_var, EAX);
        asm_jmp_eq(end_label);

        ClmScope *loopScope = clm_scope_find_child(data.scope, node);
        data.scope = loopScope;
        gen_statements(node->loopStmt->body);
        data.scope = loopScope->parent;

        if(node->loopStmt->delta->type == EXP_TYPE_INT
            && node->loopStmt->delta->ival == 1){
            asm_inc(loop_var);
        }else if(node->loopStmt->delta->type == EXP_TYPE_INT
            && node->loopStmt->delta->ival == -1){
            asm_dec(loop_var);
        }else if(node->loopStmt->delta->type == EXP_TYPE_INT){
            asm_add_i(loop_var, node->loopStmt->delta->ival);
        }else{
            gen_expression(node->loopStmt->delta);
            asm_pop(EAX);
            asm_add(loop_var, EAX);
        }

        asm_jmp(start_label);
        asm_label(end_label);        
        break;
    }
    case STMT_TYPE_PRINT:
        gen_print(node->printStmt->expression, node->printStmt->appendNewline);
        break;
    case STMT_TYPE_RET:
        //reset the stack pointer,
        //save the frame pointer & the stack address 
        //push the return expression onto the stack so the stack looks like this
        // on return:
        //
        // return val
        // return type
        // <- esp

        //note: T_EAX and T_EBX are globals defined in clm_asm_headers.h
        asm_mov(ESP, EBP); //reset stack pointer to above the function
        asm_pop(T_EBX); //pop the old frame pointer into t_ebx
        asm_pop(T_EAX); //pop the stack address of the next instruction to execute after call finishes
        gen_expression(node->returnExpr);
        asm_mov(EBP,T_EBX); //move the old frame pointer into ebp
        asm_push(T_EAX); //push the stack address of the next instruction to execute after call finishes
        asm_ret(); //return
        break;
    }
}

static void gen_functions(ClmArrayList *statements){
    int i;
    for(i = 0;i < statements->length;i++){
        ClmStmtNode *node = statements->data[i];
        if(node->type == STMT_TYPE_FUNC_DEC){
            gen_statement(node);
        }
    }
}

static void gen_statements(ClmArrayList *statements){
    int i;
    for(i = 0;i < statements->length;i++){
        ClmStmtNode *node = statements->data[i];
        if(node->type != STMT_TYPE_FUNC_DEC){
            gen_statement(node);
        }
    }
}

static void gen_macros(){

}

const char *clm_code_gen_main(ClmArrayList *statements, ClmScope *globalScope){
    data.scope = globalScope;
    data.labelID = 0;
    data.inFunction = 0;
    data.code = malloc(strlen(ASM_HEADER) + 1);
    strcpy(data.code, ASM_HEADER);

    gen_functions(statements);

    //TODO make it start in the main function!

    writeLine(ASM_START);
    gen_statements(statements);
    writeLine(ASM_EXIT_PROCESS);

    return data.code;
}