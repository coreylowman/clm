#include <stdio.h>
#include <stdlib.h>
#include "util/clm_statement.h"
#include "util/clm_string.h"

ClmStmtNode *clm_stmt_new_assign(ClmExpNode *lhs,ClmExpNode *rhs){
    ClmAssignStmt *assignStmt = malloc(sizeof(*assignStmt));
    assignStmt->lhs = lhs;
    assignStmt->rhs = rhs;
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_ASSIGN;
    node->assignStmt = assignStmt;
    return node;
}

ClmStmtNode *clm_stmt_new_call(ClmExpNode *callExpr){
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_CALL;
    node->callExpr = callExpr;
    return node;
}

ClmStmtNode *clm_stmt_new_cond(ClmExpNode *condition,ClmArrayList *trueBody,ClmArrayList *falseBody){
    ClmConditionStmt *conditionStmt = malloc(sizeof(*conditionStmt));
    conditionStmt->condition = condition;
    conditionStmt->trueBody = trueBody;
    conditionStmt->falseBody = falseBody;
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_CONDITIONAL;
    node->conditionStmt = conditionStmt;
    return node;
}

ClmStmtNode *clm_stmt_new_dec(char *name,ClmArrayList *params, ClmType returnType, int returnRows,char *returnRowsVars,int returnCols,char *returnColsVar, ClmArrayList *body){
    ClmFuncDecStmt *funcDecStmt = malloc(sizeof(*funcDecStmt));
    funcDecStmt->name = clm_string_copy(name);
    funcDecStmt->parameters = params;
	funcDecStmt->returnType = returnType;
    funcDecStmt->returnRows = returnRows;
	funcDecStmt->returnRowsVar = clm_string_copy(returnRowsVars);
    funcDecStmt->returnCols = returnCols;
    funcDecStmt->returnColsVar = clm_string_copy(returnColsVar);
    funcDecStmt->body = body;
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_FUNC_DEC;
    node->funcDecStmt = funcDecStmt;
    return node;
}

ClmStmtNode *clm_stmt_new_loop(char *varId,ClmExpNode *start,ClmExpNode *end,ClmExpNode *delta,ClmArrayList *body){
    ClmLoopStmt *loopStmt = malloc(sizeof(*loopStmt));
    loopStmt->varId = clm_string_copy(varId);
    loopStmt->start = start;
    loopStmt->end = end;
    loopStmt->delta = delta;
    loopStmt->body = body;
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_LOOP;
    node->loopStmt = loopStmt;
    return node;
}

ClmStmtNode *clm_stmt_new_print(ClmExpNode *expression,int appendNewline){
    ClmPrintStmt *printStmt = malloc(sizeof(*printStmt));
    printStmt->expression = expression;
	printStmt->appendNewline = appendNewline;
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_PRINT;
    node->printStmt = printStmt;
    return node;
}

ClmStmtNode *clm_stmt_new_return(ClmExpNode *returnExpr){
    ClmStmtNode *node = malloc(sizeof(*node));
    node->type = STMT_TYPE_RET;
    node->returnExpr = returnExpr;
    return node;
}


void clm_stmt_free(void *data){
    if(data == NULL) return;

    ClmStmtNode *node = (ClmStmtNode *)data;
    switch(node->type){
        case STMT_TYPE_ASSIGN:
            clm_exp_free(node->assignStmt->lhs);
            clm_exp_free(node->assignStmt->rhs);
            free(node->assignStmt);
            break;
        case STMT_TYPE_CALL:
            clm_exp_free(node->callExpr);
            break;
        case STMT_TYPE_CONDITIONAL:
            clm_exp_free(node->conditionStmt->condition);
            clm_array_list_free(node->conditionStmt->trueBody);
            clm_array_list_free(node->conditionStmt->falseBody);
            free(node->conditionStmt);
            break;
        case STMT_TYPE_FUNC_DEC:
            free(node->funcDecStmt->name);
            clm_array_list_free(node->funcDecStmt->parameters);
            clm_array_list_free(node->funcDecStmt->body);
            free(node->funcDecStmt);
            break;
        case STMT_TYPE_LOOP:
            free(node->loopStmt->varId);
            clm_exp_free(node->loopStmt->start);
            clm_exp_free(node->loopStmt->end);
            clm_exp_free(node->loopStmt->delta);
            clm_array_list_free(node->loopStmt->body);
            free(node->loopStmt);
            break;
        case STMT_TYPE_PRINT:
            clm_exp_free(node->printStmt->expression);
            free(node->printStmt);
            break;
        case STMT_TYPE_RET:
            clm_exp_free(node->returnExpr);
            break;
    }
    free(node);
}

void clm_stmt_print(ClmStmtNode *node, int level){
    int q = level;
    printf("\n");
    while(q-- > 0) printf("  ");
    printf("Statement ");
    switch(node->type){
        case STMT_TYPE_ASSIGN:
			printf("type : assign\n");

			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("lhs:");
            clm_exp_print(node->assignStmt->lhs, level + 2);
						
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("rhs:");
            clm_exp_print(node->assignStmt->rhs, level + 2);
            break;
        case STMT_TYPE_CALL:
			printf("type : func call\n");

			int q = level + 1;
			while (q-- > 0) printf("  ");
			printf("expression: ");
            clm_exp_print(node->callExpr, level + 2);
            break;
        case STMT_TYPE_CONDITIONAL:
            printf("type : conditional\n");
			
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("condition:");
            clm_exp_print(node->conditionStmt->condition, level + 2);
			
			q = level + 1;
			printf("\n");  while (q-- > 0) printf("  ");
            printf("truebody:");
            clm_array_list_foreach_2(node->conditionStmt->trueBody,level + 2,clm_stmt_print);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("falsebody:");
            clm_array_list_foreach_2(node->conditionStmt->falseBody,level + 2,clm_stmt_print);
            break;
        case STMT_TYPE_FUNC_DEC:
            printf("type : func dec, name : %s\n",node->funcDecStmt->name);
			
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("return type : %d, ", node->funcDecStmt->returnType);
			printf("ret rows : %d | %s, ", node->funcDecStmt->returnRows, node->funcDecStmt->returnRowsVar);
			printf("ret cols : %d | %s", node->funcDecStmt->returnCols, node->funcDecStmt->returnColsVar);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("parameters:",node->funcDecStmt->name);
            clm_array_list_foreach_2(node->funcDecStmt->parameters,level + 2,clm_exp_print);
			
			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("body:");
            clm_array_list_foreach_2(node->funcDecStmt->body,level + 2,clm_stmt_print);
            break;
        case STMT_TYPE_LOOP:
            printf("type : loop, var : %s\n",node->loopStmt->varId);
			
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("start:");
            clm_exp_print(node->loopStmt->start, level + 2);

			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("delta:");
            clm_exp_print(node->loopStmt->delta, level + 2);

			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("end:");
            clm_exp_print(node->loopStmt->end, level + 2);

			q = level + 1;
			printf("\n"); while (q-- > 0) printf("  ");
            printf("body:");
            clm_array_list_foreach_2(node->loopStmt->body,level + 2,clm_stmt_print);
            break;
        case STMT_TYPE_PRINT:
			printf("type : print, newline : %d\n",node->printStmt->appendNewline);
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("expression:");
            clm_exp_print(node->printStmt->expression, level + 2);
            break;
        case STMT_TYPE_RET:
			printf("type : return\n");
			q = level + 1;
			while (q-- > 0) printf("  ");
			printf("expression: ");
            clm_exp_print(node->returnExpr, level + 2);
            break;
    }
}