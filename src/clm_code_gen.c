#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clm_asm.h"
#include "clm_code_gen.h"
#include "clm_expression.h"
#include "clm_statement.h"
#include "clm_type.h"
#include "clm_type_gen.h"
#include "clm_type_of.h"

typedef struct {
  char *code;
  ClmScope *scope;
  int labelID;
  int inFunction;
} CodeGenData;

static CodeGenData data;

void next_label(char *buffer) {
  int id = data.labelID++;
  sprintf(buffer, "label%d", id);
}

void writeLine(const char *line) {
  // TODO instead of realloc each time... keep track of capacity and double it
  // each time (make sure to append \0)
  data.code = realloc(data.code, strlen(data.code) + strlen(line) + 1);
  strcat(data.code, line);
}

/*
 *
 *  FUNCTION FORWARD DECLARATIONS
 *
 */
static void gen_arith(ClmExpNode *node);
static void gen_bool(ClmExpNode *node);
static void gen_unary(ClmExpNode *node);

static void gen_lhs(ClmExpNode *node);
static void gen_matrix_size(ClmExpNode *node);
static void gen_expression(ClmExpNode *node);
static void gen_statement(ClmStmtNode *node);

static void gen_functions(ArrayList *statements);
static void gen_statements(ArrayList *statements);

static void gen_arith(ClmExpNode *node) {
  ClmType left_type = clm_type_of_exp(node->arithExp.left, data.scope);
  ClmType right_type = clm_type_of_exp(node->arithExp.right, data.scope);

  switch (left_type) {
  case CLM_TYPE_INT:
    gen_int_arith(node->arithExp.operand, right_type);
    break;
  case CLM_TYPE_FLOAT:
    gen_float_arith(node->arithExp.operand, right_type);
    break;
  case CLM_TYPE_STRING:
    gen_string_arith(node->arithExp.operand, right_type);
    break;
  case CLM_TYPE_MATRIX:
    gen_mat_arith(node->arithExp.operand, right_type);
    break;
  default:
    // shouldn't get here
    break;
  }
}

static void gen_bool(ClmExpNode *node) {
  ClmType left_type = clm_type_of_exp(node->arithExp.left, data.scope);
  ClmType right_type = clm_type_of_exp(node->arithExp.right, data.scope);

  switch (left_type) {
  case CLM_TYPE_INT:
    gen_int_bool(node->boolExp.operand, right_type);
    break;
  case CLM_TYPE_FLOAT:
    gen_float_bool(node->boolExp.operand, right_type);
    break;
  case CLM_TYPE_STRING:
    gen_string_bool(node->boolExp.operand, right_type);
    break;
  case CLM_TYPE_MATRIX:
    gen_mat_bool(node->boolExp.operand, right_type);
    break;
  default:
    // shouldn't get here
    break;
  }
}

static void gen_unary(ClmExpNode *node) {
  ClmType type = clm_type_of_exp(node->unaryExp.node, data.scope);

  switch (type) {
  case CLM_TYPE_INT:
    gen_int_unary(node->unaryExp.operand);
    break;
  case CLM_TYPE_FLOAT:
    gen_float_unary(node->unaryExp.operand);
    break;
  case CLM_TYPE_STRING:
    gen_string_unary(node->unaryExp.operand);
    break;
  case CLM_TYPE_MATRIX:
    gen_mat_unary(node->unaryExp.operand);
    break;
  default:
    // shouldn't get here
    break;
  }
}

/* pushes the number of column and then the number of rows */
static void gen_matrix_size(ClmExpNode *node) {
  char index_str[32];
  // this is a matDecExp
  if (node->matDecExp.colVar != NULL) {
    // push dword [ebp+offset]
    ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp.colVar);
    sprintf(index_str, "dword [ebp+%d]", sym->offset + 8);
    asm_push(index_str);
  } else {
    // push $colInd
    asm_push_i(node->matDecExp.cols);
  }

  if (node->matDecExp.rowVar != NULL) {
    // push dword [ebp+offset]
    ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp.rowVar);
    sprintf(index_str, "dword [ebp+%d]", sym->offset + 4);
    asm_push(index_str);
  } else {
    // push $rowInd
    asm_push_i(node->matDecExp.rows);
  }
}

// TODO rename this pop_int_intoo_var
static void gen_lhs(ClmExpNode *node) {
  // it is an index node - otherwise it is a type check fail
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);

  switch (var->type) {
  case CLM_TYPE_INT:
    if (var->isParam)
      sprintf(index_str, "dword [ebp+%d]", var->offset + 4);
    else
      sprintf(index_str, "dword [ebp+%d]", var->offset - 4);
    pop_int_into(index_str);
    break;
  case CLM_TYPE_FLOAT:
    // TODO floats
    break;
  case CLM_TYPE_MATRIX: {
    // TODO case where matrix is actually a pointer
    if (node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL) {
      // TODO pop whole matrix off stack
      char start_label[LABEL_SIZE];
      char end_label[LABEL_SIZE];
      next_label(start_label);
      next_label(end_label);

      asm_pop(EAX);       // pop type
      asm_pop(ECX);       // pop rows
      asm_pop(EBX);       // pop cols;
      asm_imul(ECX, EBX); // ecx now contains rows * cols
      asm_dec(ECX);

      asm_label(start_label);
      asm_cmp(ECX, "0");
      asm_jmp_l(end_label);

      sprintf(index_str, "dword [ebp+%d+ecx]", var->offset);
      asm_pop(index_str);

      asm_dec(ECX);
      asm_jmp(start_label);
      asm_label(end_label);
    } else if (node->indExp.rowIndex == NULL) {
      // TODO pop a whole row off the stack
    } else if (node->indExp.colIndex == NULL) {
      // TODO pop a whole col off the stack
    } else {
      // TODO pop one val off the stack
      gen_expression(node->indExp.colIndex);
      gen_expression(node->indExp.rowIndex);
      pop_int_into(EAX);  // pop rows into eax
      pop_int_into(EBX);  // pop cols into ebx
      asm_imul(EAX, EBX); // eax = eax * ebx
      asm_imul(EAX, "4"); // now eax contains row * col * 4
      // var->offset points at type... 3 elements above is the first item in
      // the matrix
      sprintf(index_str, "dword [ebp+%d+eax]", var->offset + 12);
      asm_pop(index_str);
    }
    break;
  }
  case CLM_TYPE_STRING:
    // uhh
    break;
  case CLM_TYPE_FUNCTION:
  case CLM_TYPE_NONE:
    // shouldn't get here...
    break;
  }
}

static void gen_index(ClmExpNode *node) {
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  switch (var->type) {
  case CLM_TYPE_INT:
    if (var->isParam)
      sprintf(index_str, "dword [ebp+%d]", var->offset + 4);
    else
      sprintf(index_str, "dword [ebp+%d]", var->offset - 4);
    asm_push(index_str);
    asm_push_i((int)var->type);
    break;
  case CLM_TYPE_FLOAT:
    // TODO floats
    break;
  case CLM_TYPE_MATRIX: {
    // TODO case where matrix is actually a pointer
    if (node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL) {
      // TODO push whole matrix on stack
    } else if (node->indExp.rowIndex == NULL) {
      // TODO push a whole row onto the stack
    } else if (node->indExp.colIndex == NULL) {
      // TODO push a whole col onto the stack
    } else {
      // TODO push one val onto the stack
      gen_expression(node->indExp.colIndex);
      gen_expression(node->indExp.rowIndex);
      pop_int_into(EAX); // pop row index into eax
      pop_int_into(EBX); // pop col index into ebx
      asm_imul(EAX, EBX);
      asm_imul(EAX, "4"); // now eax contains row * col * 4
      // var->offset points at type... 3 elements above is the first item
      // in the matrix
      // sprintf(index_str, "dword [ebp+%d+eax]", var->offset + 12);
      sprintf(index_str, "[ebp+%d]", var->offset + 4);
      asm_mov(EBX, index_str);
      // asm_add(EAX, EBX); //eax now contains
      asm_push("dword [eax]");
      asm_push_i((int)CLM_TYPE_INT);
    }
    break;
  }
  case CLM_TYPE_STRING:
    // uhh
    break;
  default:
    // shouldn't get here...
    break;
  }
}

// stack should look like this:
// val
// type
static void gen_expression(ClmExpNode *node) {
  ClmType expression_type = clm_type_of_exp(node, data.scope);
  switch (node->type) {
  case EXP_TYPE_INT:
    asm_push_i(node->ival);
    asm_push_i((int)expression_type);
    break;
  case EXP_TYPE_FLOAT:
    asm_push_f(node->fval);
    asm_push_i((int)expression_type);
    break;
  case EXP_TYPE_STRING:
    break;
  case EXP_TYPE_ARITH: {
    ClmType right_type = clm_type_of_exp(node->arithExp.right, data.scope);
    ClmType left_type = clm_type_of_exp(node->arithExp.left, data.scope);
    if (left_type == CLM_TYPE_MATRIX && clm_type_is_number(right_type)) {
      // here the only ops are mul & div... we are scaling matrix
      // gen left and then right here... if we don't then we have
      // int val
      // int type
      // matrix
      // cols
      // rows
      // matrix type
      // and we have to pop the int after we generate the value... which is hard
      // and since we are multiplying the matrix in place, it would be easiest
      // to
      // gen the matrix first and then the int, so we just have to pop two
      // values
      // in total
      gen_expression(node->arithExp.left);
      asm_mov(EDX, ESP);
      gen_expression(node->arithExp.right);
      gen_arith(node);
    } else {
      gen_expression(node->arithExp.right);
      asm_mov(EDX, ESP);
      gen_expression(node->arithExp.left);
      gen_arith(node);
    }
    break;
  }
  case EXP_TYPE_BOOL:
    gen_expression(node->boolExp.right);
    asm_mov(EDX, ESP);
    gen_expression(node->boolExp.left);
    gen_bool(node);
    break;
  case EXP_TYPE_CALL: {
    int i;
    for (i = node->callExp.params->length - 1; i >= 0; i--) {
      gen_expression(node->callExp.params->data[i]);
    }
    asm_call(node->callExp.name);
    break;
  }
  case EXP_TYPE_INDEX: {
    gen_index(node);
    break;
  }
  case EXP_TYPE_MAT_DEC: {
    int i;
    if (node->matDecExp.arr != NULL) {
      for (i = node->matDecExp.length - 1; i >= 0; i--) {
        // TODO... push f or push i?
        asm_push_i(node->matDecExp.arr[i]);
      }
      asm_push_i(node->matDecExp.cols);
      asm_push_i(node->matDecExp.rows);
      asm_push_i((int)expression_type);
    } else {
      // TODO figure out where to allocate this??
      // WAIT! we don't need to allocate???
      // just have a for loop that pushes 0 onto the stack
      // d'oh
      // this is already handled by local var allocation in
      // function dec
      char index_str[32];
      if (node->matDecExp.rowVar != NULL) {
        // mov eax,[ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp.rowVar);
        sprintf(index_str, "[ebp+%d]", sym->offset);
        asm_mov(EAX, index_str);
      } else {
        // mov eax,$rowInd
        sprintf(index_str, "%d", node->matDecExp.rows);
        asm_mov(EAX, index_str);
      }

      if (node->matDecExp.colVar != NULL) {
        // mul ebx,[ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, node->matDecExp.colVar);
        sprintf(index_str, "[ebp+%d]", sym->offset);
        asm_mov(EBX, index_str);
      } else {
        // mul ebx,$colInd
        sprintf(index_str, "%d", node->matDecExp.cols);
        asm_mov(EBX, index_str);
      }
      char start_label[LABEL_SIZE];
      char end_label[LABEL_SIZE];
      next_label(start_label);
      next_label(end_label);

      asm_mov(ECX, EBX);
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
      asm_push_i((int)expression_type);
    }
    break;
  }
  case EXP_TYPE_PARAM:
    // TODO ?
    break;
  case EXP_TYPE_UNARY:
    gen_expression(node->unaryExp.node);
    gen_unary(node);
    break;
  }
}

static void gen_print(ClmExpNode *node, int newline) {
  switch (node->type) {
  case EXP_TYPE_INT:
    asm_print_const_int(node->ival, newline);
    break;
  case EXP_TYPE_FLOAT:
    asm_print_const_float(node->fval, newline);
    break;
  case EXP_TYPE_STRING:
    // TODO print string
    break;
  case EXP_TYPE_ARITH:
    gen_expression(node);
    // TODO print result
    break;
  case EXP_TYPE_BOOL:
    gen_expression(node);
    // TODO print result
    break;
  case EXP_TYPE_CALL:
    gen_expression(node);
    // TODO print result
    ClmSymbol *function = clm_scope_find(data.scope, node->callExp.name);
    switch (function->type) {
    case CLM_TYPE_INT:
      break;
    case CLM_TYPE_MATRIX:
      break;
    case CLM_TYPE_STRING:
      break;
    case CLM_TYPE_FLOAT:
      break;
    default:
      // shouldn't get here
      break;
    }
    break;
  case EXP_TYPE_INDEX: {
    char index_str[32];
    ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
    switch (var->type) {
    case CLM_TYPE_INT:
      gen_expression(node);
      pop_int_into(EAX);
      asm_print_int(EAX, newline);
      break;
    case CLM_TYPE_FLOAT:
      gen_expression(node);
      asm_pop(EAX); // pop type
      asm_pop(EAX); // pop val into eax
      asm_print_float(EAX, newline);
      break;
    case CLM_TYPE_MATRIX: {
      // these two types should be the same...
      // they are generated differently...
      // but gen_expression will push all the vals onto the stack
      gen_expression(node);
      if (node->indExp.rowIndex != NULL && node->indExp.colIndex != NULL) {
        pop_int_into(EAX);
        asm_print_int(EAX, newline);
      } else {
        // for all other cases we wil have to pop the rows and cols
        // off the stack
        // TODO print matrix
      }
      break;
    }
    case CLM_TYPE_STRING:
      // this will be similar to printing a matrix, but will always be
      // one dimensional, so will just pop off length and then do a for
      // loop to print each char
      // TODO print string
      break;
    case CLM_TYPE_FUNCTION:
    case CLM_TYPE_NONE:
      // can't print these :)
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

static void gen_statement(ClmStmtNode *node) {
  switch (node->type) {
  case STMT_TYPE_ASSIGN:
    gen_expression(node->assignStmt.rhs);
    gen_lhs(node->assignStmt.lhs);
    break;
  case STMT_TYPE_CALL:
    gen_expression(node->callExpr);
    break;
  case STMT_TYPE_CONDITIONAL: {
    gen_expression(node->conditionStmt.condition);

    if (node->conditionStmt.falseBody == NULL) {
      char end_label[LABEL_SIZE];
      next_label(end_label);
      ClmScope *trueScope =
          clm_scope_find_child(data.scope, node->conditionStmt.trueBody);

      asm_pop(EAX);
      asm_cmp(EAX, "1");
      asm_jmp_neq(end_label);
      data.scope = trueScope;
      gen_statements(node->conditionStmt.trueBody);
      asm_label(end_label);

      data.scope = trueScope->parent;
    } else {
      char end_label[LABEL_SIZE];
      char false_label[LABEL_SIZE];
      next_label(end_label);
      next_label(false_label);
      ClmScope *trueScope =
          clm_scope_find_child(data.scope, node->conditionStmt.trueBody);
      ClmScope *falseScope =
          clm_scope_find_child(data.scope, node->conditionStmt.falseBody);

      asm_pop(EAX);
      asm_cmp(EAX, "1");
      asm_jmp_neq(false_label);
      data.scope = trueScope;
      gen_statements(node->conditionStmt.trueBody);
      asm_jmp(end_label);
      asm_label(false_label);
      data.scope = falseScope;
      gen_statements(node->conditionStmt.falseBody);
      asm_label(end_label);

      data.scope = falseScope->parent;
    }
    break;
  }
  case STMT_TYPE_FUNC_DEC: {
    int i;
    char func_label[LABEL_SIZE];

    sprintf(func_label, "_%s", node->funcDecStmt.name);
    ClmScope *funcScope = clm_scope_find_child(data.scope, node);

    asm_label(func_label);
    asm_push(EBP);
    asm_mov(EBP, ESP);

    int local_var_size = 2 * 4 * (funcScope->symbols->length -
                                  node->funcDecStmt.parameters->length);
    char local_var_size_str[32];
    sprintf(local_var_size_str, "%d", local_var_size);
    asm_sub(ESP, local_var_size_str);

    // each local var has 2 slots on the stack, their type and the value
    // for matrices, the value is a pointer to the location on the stack
    // these are all declared below the local variables
    ClmSymbol *sym;
    ClmStmtNode *dec;
    char start_label[LABEL_SIZE];
    char end_label[LABEL_SIZE];
    char index_str[32];
    for (i = 0; i < funcScope->symbols->length; i++) {
      sym = funcScope->symbols->data[i];
      dec = sym->declaration;

      if (sym->isParam)
        continue;
      // setting the type of the local var
      sprintf(index_str, "dword [ebp+%d]", sym->offset);
      asm_mov_i(index_str, (int)sym->type);

      // setting the value of the local var
      sprintf(index_str, "dword [ebp+%d]", sym->offset - 4);
      asm_mov_i(index_str, 0);

      if (sym->type == CLM_TYPE_MATRIX) {
        next_label(start_label);
        next_label(end_label);

        gen_matrix_size(dec->assignStmt.rhs);
        asm_pop(EAX); // eax contains num of rows
        asm_pop(EBX); // ebx contains num of cols
        asm_mov(ECX, EAX);
        asm_imul(ECX, EBX);
        asm_label(start_label);
        asm_dec(ECX);
        asm_cmp(ECX, "0");
        asm_jmp_eq(end_label);
        asm_push_i(0);
        asm_jmp(start_label);
        asm_label(end_label);
        asm_push(EBX); // cols

        // setting the pointer to point at the rows
        // note: push changes the value at esp and THEN
        // decrements it
        sprintf(index_str, "dword [ebp+%d]", sym->offset - 4);
        asm_mov(index_str, ESP);

        asm_push(EAX); // rows
      }
    }
    // TODO figure out strings though!

    data.inFunction = 1;
    data.scope = funcScope;
    gen_statements(node->funcDecStmt.body);
    data.scope = funcScope->parent;
    data.inFunction = 0;

    if (node->funcDecStmt.returnRows == -2) {
      // no return value!
      asm_mov(ESP, EBP);
      asm_pop(EBP);
    }
    asm_ret();
    break;
  }
  case STMT_TYPE_LOOP: {
    char start_label[LABEL_SIZE];
    char end_label[LABEL_SIZE];
    next_label(start_label);
    next_label(end_label);

    ClmSymbol *var = clm_scope_find(data.scope, node->loopStmt.varId);
    char loop_var[32];
    sprintf(loop_var, "dword [ebp+%d]", var->offset + (var->isParam ? 4 : -4));

    // don't need to store this - just evaluate and put into loop var
    gen_expression(node->loopStmt.start);

    if (!node->loopStmt.startInclusive) {
      gen_expression(node->loopStmt.delta);
      pop_int_into(EAX); // eax contains delta
      pop_int_into(EBX); // ebx contains start
      asm_add(EBX, EAX);

      asm_push(EBX); // push start back on stack
      asm_push_i((int)CLM_TYPE_INT);
    }
    pop_int_into(loop_var);
    asm_label(start_label);

    // don't need to store this - just evaulate every loop
    gen_expression(node->loopStmt.end);

    pop_int_into(EAX);

    asm_cmp(loop_var, EAX);
    if (node->loopStmt.endInclusive)
      asm_jmp_g(end_label);
    else
      asm_jmp_ge(end_label);

    ClmScope *loopScope = clm_scope_find_child(data.scope, node);
    data.scope = loopScope;
    gen_statements(node->loopStmt.body);
    data.scope = loopScope->parent;

    if (node->loopStmt.delta->type == EXP_TYPE_INT &&
        node->loopStmt.delta->ival == 1) {
      asm_inc(loop_var);
    } else if (node->loopStmt.delta->type == EXP_TYPE_INT &&
               node->loopStmt.delta->ival == -1) {
      asm_dec(loop_var);
    } else if (node->loopStmt.delta->type == EXP_TYPE_INT) {
      asm_add_i(loop_var, node->loopStmt.delta->ival);
    } else {
      gen_expression(node->loopStmt.delta);
      asm_pop(EAX);
      asm_add(loop_var, EAX);
    }

    asm_jmp(start_label);
    asm_label(end_label);
    break;
  }
  case STMT_TYPE_PRINT:
    gen_print(node->printStmt.expression, node->printStmt.appendNewline);
    break;
  case STMT_TYPE_RET:
    // reset the stack pointer,
    // save the frame pointer & the stack address
    // push the return expression onto the stack so the stack looks like this
    // on return:
    //
    // return val
    // return type
    // <- esp

    // note: T_EAX and T_EBX are globals defined in clm_asm_headers.h
    asm_mov(ESP, EBP); // reset stack pointer to above the function
    asm_pop(T_EBX);    // pop the old frame pointer into t_ebx
    asm_pop(T_EAX);    // pop the stack address of the next instruction to
    // execute after call finishes
    gen_expression(node->returnExpr);
    asm_mov(EBP, T_EBX); // move the old frame pointer into ebp
    asm_push(T_EAX);     // push the stack address of the next instruction to
    // execute after call finishes
    asm_ret(); // return
    break;
  }
}

static void gen_functions(ArrayList *statements) {
  int i;
  for (i = 0; i < statements->length; i++) {
    ClmStmtNode *node = statements->data[i];
    if (node->type == STMT_TYPE_FUNC_DEC) {
      gen_statement(node);
    }
  }
}

static void gen_statements(ArrayList *statements) {
  int i;
  for (i = 0; i < statements->length; i++) {
    ClmStmtNode *node = statements->data[i];
    if (node->type != STMT_TYPE_FUNC_DEC) {
      gen_statement(node);
    }
  }
}

static void gen_macros() {}

const char *clm_code_gen_main(ArrayList *statements, ClmScope *globalScope) {
  data.scope = globalScope;
  data.labelID = 0;
  data.inFunction = 0;
  data.code = malloc(strlen(ASM_HEADER) + 1);
  strcpy(data.code, ASM_HEADER);

  gen_functions(statements);

  // TODO make it start in the main function!

  writeLine(ASM_START);
  gen_statements(statements);
  writeLine(ASM_EXIT_PROCESS);

  return data.code;
}
