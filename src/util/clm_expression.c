#include <stdlib.h>
#include <stdio.h>
#include "clm_string.h"
#include "clm_expression.h"

const char *clm_arith_op_to_string(ClmArithOp op){
    const char *strings[] = { "ADD" , "SUB" , "MULT" , "DIV" };
    return strings[(int)op];
}

const char *clm_bool_op_to_string(ClmBoolOp op){
    const char *strings[] = { "AND" , "OR" , "EQ" , "NEQ" , "GT" , "LT" , "GTE" , "LTE" };
    return strings[(int)op];
}

const char *clm_unary_op_to_string(ClmUnaryOp op){
    const char *strings[] = { "MINUS", "TRANSPOSE" , "NOT" };
    return strings[(int)op];
}

ClmExpNode *clm_exp_new_int(int val){
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_INT;
    node->ival = val;
    return node;
}

ClmExpNode *clm_exp_new_float(float fval){
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_FLOAT;
    node->fval = fval;
    return node;
}

ClmExpNode *clm_exp_new_string(const char *str){
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_STRING;
    node->str = clm_string_copy(str);
    return node;
}

ClmExpNode *clm_exp_new_arith(ClmArithOp operand, ClmExpNode *right, ClmExpNode *left){
    ClmArithExp *arithExp = malloc(sizeof(*arithExp));
    arithExp->operand = operand;
    arithExp->right = right;
    arithExp->left = left;
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_ARITH;
    node->arithExp = arithExp;
    return node;
}

ClmExpNode *clm_exp_new_bool(ClmBoolOp operand, ClmExpNode *right, ClmExpNode *left){
    ClmBoolExp *boolExp = malloc(sizeof(*boolExp));
    boolExp->operand = operand;
    boolExp->right = right;
    boolExp->left = left;
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_BOOL;
    node->boolExp = boolExp;
    return node;
}

ClmExpNode *clm_exp_new_call(char *name,ClmArrayList *params){
    ClmCallExp *callExp = malloc(sizeof(*callExp));
    callExp->name = clm_string_copy(name);
    callExp->params = params; 
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_CALL;
    node->callExp = callExp;
    return node;
}

ClmExpNode *clm_exp_new_index(const char *id,ClmExpNode *rowIndex,ClmExpNode *colIndex){
    ClmIndexExp *indExp = malloc(sizeof(*indExp));
    indExp->id = clm_string_copy(id);
    indExp->rowIndex = rowIndex;
    indExp->colIndex = colIndex;
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_INDEX;
    node->indExp = indExp;
    return node;
}

ClmExpNode *clm_exp_new_mat_dec(float *arr,int length,int cols){
    ClmMatDecExp *matDecExp = malloc(sizeof(*matDecExp));
    matDecExp->arr = arr;
    matDecExp->length = length;
    matDecExp->rows = length / cols;
    matDecExp->cols = cols;
    matDecExp->rowVar = NULL;
    matDecExp->colVar = NULL;
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_MAT_DEC;
    node->matDecExp = matDecExp;
    return node;
}

ClmExpNode *clm_exp_new_empty_mat_dec(int rows,int cols,const char *rowVar,const char *colVar){
    ClmMatDecExp *matDecExp = malloc(sizeof(*matDecExp));
    matDecExp->arr = NULL;
    matDecExp->length = 0;
    matDecExp->rows = rows;
    matDecExp->cols = cols;
    matDecExp->rowVar = clm_string_copy(rowVar);
    matDecExp->colVar = clm_string_copy(colVar);
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_MAT_DEC;
    node->matDecExp = matDecExp;
    return node;
}

ClmExpNode *clm_exp_new_param(const char *name,ClmType type, int rows,int cols,const char *rowVar,const char *colVar){
    ClmParamExp *paramExp = malloc(sizeof(*paramExp));
    paramExp->name = clm_string_copy(name);
	paramExp->type = type;
    paramExp->rows = rows;
    paramExp->cols = cols;
    paramExp->rowVar = clm_string_copy(rowVar);
    paramExp->colVar = clm_string_copy(colVar);
    ClmExpNode *node = malloc(sizeof(*node));
    node->type = EXP_TYPE_PARAM;
    node->paramExp = paramExp;
    return node;
}

ClmExpNode *clm_exp_new_unary(ClmUnaryOp operand, ClmExpNode *node){
    ClmUnaryExp *unaryExp = malloc(sizeof(*unaryExp));
    unaryExp->operand = operand;
    unaryExp->node = node;
	ClmExpNode *unaryNode = malloc(sizeof(*unaryNode));
	unaryNode->type = EXP_TYPE_UNARY;
	unaryNode->unaryExp = unaryExp;
	return unaryNode;
}


void clm_exp_free(void *data){
    if(data == NULL) return;

    ClmExpNode *node = (ClmExpNode *)data;
    switch(node->type){
        case EXP_TYPE_INT:
        case EXP_TYPE_FLOAT:            
            break;
        case EXP_TYPE_STRING:
            free(node->str);
            break;
        case EXP_TYPE_ARITH:
            clm_exp_free(node->arithExp->left);
            clm_exp_free(node->arithExp->right);
            free(node->arithExp);
            break;
        case EXP_TYPE_BOOL:
            clm_exp_free(node->boolExp->left);
            clm_exp_free(node->boolExp->right);
            free(node->boolExp);
            break;
        case EXP_TYPE_CALL:
            free(node->callExp->name);
            clm_array_list_free(node->callExp->params);
            free(node->callExp);
            break;
        case EXP_TYPE_INDEX:
            free(node->indExp->id);
            clm_exp_free(node->indExp->rowIndex);
            clm_exp_free(node->indExp->colIndex);
            free(node->indExp);
            break;
        case EXP_TYPE_MAT_DEC:
            free(node->matDecExp->arr);
            free(node->matDecExp->rowVar);
            free(node->matDecExp->colVar);
            free(node->matDecExp);
            break;
        case EXP_TYPE_PARAM:
            free(node->paramExp->name);
            free(node->paramExp->rowVar);
            free(node->paramExp->colVar);
            free(node->paramExp);
            break;
        case EXP_TYPE_UNARY:
            clm_exp_free(node->unaryExp->node);
            free(node->unaryExp);
            break;
    }
    free(node);
}

void clm_exp_unbox_right(ClmExpNode *node){
    switch(node->type){
        case EXP_TYPE_ARITH:
        {
            ClmExpNode *unboxed;
            unboxed = node->arithExp->right;

            clm_exp_free(node->arithExp->left);
            node->arithExp->right = NULL;
            free(node->arithExp);

            node->type = unboxed->type;
            //no matter what type unboxed is, the data will be at id's offset in the struct
            node->str = unboxed->str;

            free(unboxed);
            break;
        }
        case EXP_TYPE_BOOL:
        {
            ClmExpNode *unboxed;
            unboxed = node->boolExp->right;

            clm_exp_free(node->boolExp->left);
            node->boolExp->right = NULL;
            free(node->boolExp);

            node->type = unboxed->type;
            //no matter what type unboxed is, the data will be at id's offset in the struct
            node->str = unboxed->str;

            free(unboxed);
            break;
        }
        default:
            break;
    }
}

void clm_exp_unbox_left(ClmExpNode *node){
    switch(node->type){
        case EXP_TYPE_ARITH:
        {
            ClmExpNode *unboxed;
            unboxed = node->arithExp->left;

            clm_exp_free(node->arithExp->right);
            node->arithExp->left = NULL;
            free(node->arithExp);

            node->type = unboxed->type;
            //no matter what type unboxed is, the data will be at id's offset in the struct
            node->str = unboxed->str;

            free(unboxed);
            break;
        }
        case EXP_TYPE_BOOL:
        {
            ClmExpNode *unboxed;
            unboxed = node->boolExp->left;

            clm_exp_free(node->boolExp->right);
            node->boolExp->left = NULL;
            free(node->boolExp);

            node->type = unboxed->type;
            //no matter what type unboxed is, the data will be at id's offset in the struct
			node->str = unboxed->str;

            free(unboxed);
            break;
        }
        default:
            break;
    }
}

void clm_exp_unbox_unary(ClmExpNode *node){
    ClmExpNode *unboxed;
    unboxed = node->unaryExp->node;
    node->unaryExp->node = NULL;
    free(node->unaryExp);

    node->type = unboxed->type;
    //no matter what type unboxed is, the data will be at id's offset in the struct
    node->str = unboxed->str;

    free(unboxed);
}

void clm_exp_print(ClmExpNode *node,int level){
    int q = level;    
	printf("\n"); while (q-- > 0) printf("  ");
    printf("Expression ");
    switch(node->type){
        case EXP_TYPE_INT:
            printf("type : int, val : %d",node->ival);
            break;
        case EXP_TYPE_FLOAT:
            printf("type : float, val : %f",node->fval);
            break;
        case EXP_TYPE_STRING:
            printf("type : string, val : %s",node->str);
            break;
        case EXP_TYPE_ARITH:
            printf("type : arith, op : %d\n",node->arithExp->operand);
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("right:");
            clm_exp_print(node->arithExp->right, level + 2);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("left:");
            clm_exp_print(node->arithExp->left, level + 2);
            break;
        case EXP_TYPE_BOOL:
            printf("type : bool, op : %d\n",node->boolExp->operand);
			
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("right:");
            clm_exp_print(node->boolExp->right, level + 2);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("left:");
            clm_exp_print(node->boolExp->left, level + 2);
            break;
        case EXP_TYPE_CALL:
            printf("type : func call, name : %s",node->callExp->name);
            printf(", params : ");
            clm_array_list_foreach_2(node->callExp->params,level + 2, clm_exp_print);
            break;
        case EXP_TYPE_INDEX:
            printf("type : index, id : %s\n",node->indExp->id);
			
			q = level + 1;
			while (q-- > 0) printf("  ");
            printf("rowIndex:");
            clm_exp_print(node->indExp->rowIndex, level + 2);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("colIndex:");
            clm_exp_print(node->indExp->colIndex, level + 2);
            break;
        case EXP_TYPE_MAT_DEC:
            if(node->matDecExp->arr == NULL){
                printf("type : mat dec, rows : %d",node->matDecExp->rows);
                printf(", cols : %d",node->matDecExp->cols);
                printf(", rowVar : %s",node->matDecExp->rowVar);
                printf(", colVar : %s",node->matDecExp->colVar);
            }else{
                int i;
                printf("type : mat dec, data : ");
                for(i = 0;i < node->matDecExp->rows * node->matDecExp->cols - 1;i++){
                    printf("%f ",node->matDecExp->arr[i]);
                }
                printf("%f",node->matDecExp->arr[i]);
                printf(", rows : %d, cols : %d",node->matDecExp->rows,node->matDecExp->cols);
            }
            break;
        case EXP_TYPE_PARAM:
            printf("type : param, name : %s",node->paramExp->name);
            printf(", rows : %d",node->paramExp->rows);
            printf(", cols : %d",node->paramExp->cols);
            printf(", rowVar : %s",node->paramExp->rowVar);
            printf(", colVar : %s",node->paramExp->colVar);
            break;
        case EXP_TYPE_UNARY:
            printf("type : unary, op : %d\n",node->unaryExp->operand);
			q = level + 1;
			while (q-- > 0) printf("  ");
            printf("expression:");
            clm_exp_print(node->unaryExp->node, level + 2);
            break;
    }
}

int clm_exp_has_no_inds(ClmExpNode *node){
    if(node->type == EXP_TYPE_INDEX){
		return node->indExp->rowIndex == NULL && node->indExp->colIndex == NULL;
    }
    return 0;
}