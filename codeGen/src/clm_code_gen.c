#include <stdargs.h>
#include <stdio.h>
#include <string.h>
#include "codeGen/clm_code_gen.h"
#include "util/clm_expression.h"
#include "util/clm_statement.h"
#include "util/clm_type.h"

typedef struct{
    char *code;
    ClmScope *scope;
    int labelID;
    int inFunction;
} CodeGenData;

static CodeGenData data;

static const char ASM_HEADER[] = "format PE console\n\
entry start\n\
\n\
include 'win32a.inc'\n\
include 'macro/import32.inc'\n\
\n\
section '.rdata' data readable\n\
        print_int db '%d',32,0\n\
        print_int_nl db '%d',32,10,0\n\
        print_char db '%c',13,10,0\n\
\n\
section '.idata' data readable import\n\
        library kernel32, 'kernel32.dll', \\\n\
                msvcrt,   'msvcrt.dll'\n\
        import kernel32, ExitProcess, 'ExitProcess'\n\
        import msvcrt, printf, 'printf'\n\
\n\
section '.code' code executable\n";

static const char ASM_EXIT_PROCESS[] = "invoke ExitProcess, 0\n";
static const char ASM_START[] = "start:\n";
static const char ASM_DATA[] = "section '.data' data readable writable\n";

#define EAX "eax"
#define EBX "ebx"
#define ECX "ecx"
#define EDX "edx"
#define ESP "esp"
#define EBP "ebp"

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

static void asm_add(const char *dest, const char *other);
static void asm_sub(const char *dest, const char *other);
static void asm_imul(const char *dest, const char *other);

static void asm_fadd(const char *dest, const char *other);
static void asm_fsub(const char *dest, const char *other);
static void asm_fmul(const char *dest, const char *other);

static void asm_inc(const char *arg);
static void asm_dec(const char *arg);
static void asm_neg(const char *arg);

static void asm_mov(const char *dest, const char *src);

static void asm_and(const char *arg1, const char *arg2);
static void asm_or(const char *arg1, const char *arg2);
static void asm_xor(const char *arg1, const char *arg2);

static void asm_cmp(const char *arg2, const char *arg2);

static void asm_jmp(const char *label);
static void asm_jmp_g(const char *label);
static void asm_jmp_ge(const char *label);
static void asm_jmp_l(const char *label);
static void asm_jmp_le(const char *label);
static void asm_jmp_eq(const char *label);
static void asm_jmp_neq(const char *label);

static void asm_label(const char *name);

static void asm_call(const char *name);
static void asm_ret();

static void asm_print_int(const char *arg);
static void asm_print_int_nl(const char *arg);
static void asm_print_char(const char *arg);
static void asm_print_char_nl(const char *arg);

static void gen_arith(ClmArithExp *node);
static void gen_bool(ClmBoolExp *node);
static void gen_unary(ClmUnaryExp *node);

static void gen_expression(ClmExpNode *node);
static void gen_statement(ClmStmtNode *node);

static void gen_functions(ClmArrayList *statements);
static void gen_statements(ClmArrayList *statements);

static void next_label(char *buffer){
    int id = data.labelID++;
    sprintf(buffer, "label%d", id);
}

// stack will look like this
// right
// right type
// left <- edx
// left type
// <- esp
// so [esp-4] is the left type, and [edx-4] is the right type
static void gen_arith(ClmArithExp *node){
    switch(node->op){
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
    switch(node->op){
    case BOOL_OP_AND:
        asm_pop(EAX);
        asm_pop(EBX);
        asm_and(EAX, EBX);
        asm_push(EAX);
        break;
    case BOOL_OP_OR:
        asm_pop(EAX);
        asm_pop(EBX);
        asm_or(EAX, EBX);
        asm_push(EAX);
        break;
    case BOOL_OP_EQ:
        break;
    case BOOL_OP_NEQ:
        break;
    case BOOL_OP_GT:
    {
        char end_label[256];
        char false_label[256];
        next_label(false_label);
        next_label(end_label);

        asm_pop(EAX);
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
    switch(node->op){
    case UNARY_OP_MINUS:
    //TODO foreach element... neg it
        break;
    case UNARY_OP_TRANSPOSE:
        break;
    case UNARY_OP_NOT:
        asm_xor("dword [" ESP "]", "1");
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
        break;
    case EXP_TYPE_MAT_DEC:
        break;
    case EXP_TYPE_PARAM:
        break;
    case EXP_TYPE_UNARY:
        gen_expression(node->unaryExp->node);
        gen_unary(node->unaryExp);
        break;
    }
}

static void gen_statement(ClmStmtNode *node){
    switch(node->type){
    case STMT_TYPE_ASSIGN:
    //TODO need to write a pop_expression function for this...
    // maybe for gen_expression, add a variable to tell it where to put the stuff?
    // so for assign we would pass in the address of the lhs variable
    // and for regular gen_expression we can just pass in NULL
        gen_expression(node->assignStmt->rhs);
        //pop_into(node->assignStmt->lhs);
        break;
    case STMT_TYPE_CALL:
        gen_expression(node->callExp);
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
        char func_label[32];
        sprintf(func_label, "_%s:\n", node->funcDecStmt->name);
        ClmScope *funcScope = clm_scope_find_child(data.scope, node);

        asm_label(func_label);
        asm_push(EBP);
        asm_mov(EBP, ESP);

        //TODO subtract the local variable sizes from esp
        //also... should matrices be passed as an address?

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
        //TODO where to store these????
        gen_expression(node->loopStmt->start);
        gen_expression(node->loopStmt->end);
        gen_expression(node->loopStmt->delta);

        ClmScope *loopScope = clm_scope_find_child(scope, node);
        data.scope = loopScope;
        gen_statements(node->loopStmt->body);
        data.scope = loopScope->parent;
        break;
    }
    case STMT_TYPE_PRINT:
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

        //TODO does saving them in ebx/eax work? what if we create a global to
        //hold them?
        asm_mov(ESP, EBP); //reset stack pointer to above the function
        asm_pop(EBX); //pop the old frame pointer into ebx
        asm_pop(EAX); //pop the stack address of the next instruction to execute after call finishes
        gen_expression(node->returnExp);
        asm_mov(EBP,EBX); //move the old frame pointer into ebp
        asm_push(EAX); //push the stack address of the next instruction to execute after call finishes
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