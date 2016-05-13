#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clm.h"
#include "clm_asm.h"
#include "clm_ast.h"
#include "clm_scope.h"
#include "clm_type.h"
#include "clm_type_gen.h"

typedef struct {
  char *code;
  int size;
  int capacity;

  ClmScope *scope;
  int labelID;
  int inFunction;

  int temporaryID;
} CodeGenData;

static CodeGenData data;

void next_label(char *buffer) {
  int id = data.labelID++;
  sprintf(buffer, "label%d", id);
}

void next_temporary(char *buffer) {
  int id = ++data.temporaryID;
  sprintf(buffer, "temporary%d", id);
}

void writeLine(const char *line) {
  if(data.size == data.capacity){
    data.capacity *= 2;
    data.code = realloc(data.code, data.capacity * sizeof(*data.code));
  }
  data.size += strlen(line);
  strcat(data.code, line);
}

#define LOAD_ROWS(sym, out_buffer) load_var_location(sym, out_buffer, 4, NULL)

#define LOAD_COLS(sym, out_buffer) load_var_location(sym, out_buffer, 8, NULL)

static void load_var_location(ClmSymbol *sym, char *out_buffer, int offset,
                             const char *offset_loc) {
  char temp_buffer[64];

  sprintf(out_buffer, "dword [");

  switch(sym->location){
    case LOCATION_GLOBAL:
      if (offset != 0) {
        sprintf(temp_buffer, "_%s+%d", sym->name, offset);
      } else {
        sprintf(temp_buffer, "_%s", sym->name);
      }
      break;
    case LOCATION_PARAMETER:
      sprintf(temp_buffer, "ebp+%d", sym->offset + offset);
      break;
    case LOCATION_LOCAL:
      sprintf(temp_buffer, "ebp+%d", sym->offset + offset);
      break;
    case LOCATION_STACK: //fallthrough
    default:
      //shouldn't get here
      break;
  }
  strcat(out_buffer, temp_buffer);

  if (offset_loc != NULL) {
    sprintf(temp_buffer, "+%s", offset_loc);
    strcat(out_buffer, temp_buffer);
  }

  strcat(out_buffer, "]");
}

/*
 *
 *  FUNCTION FORWARD DECLARATIONS
 *
 */
static void gen_arith(ClmExpNode *node);
static void gen_bool(ClmExpNode *node);
static void gen_unary(ClmExpNode *node);

static void pop_into_lhs(ClmExpNode *node);
static void gen_exp_size(ClmExpNode *node);
static void push_expression(ClmExpNode *node);
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
static void gen_exp_size(ClmExpNode *node) {
  char index_str[32];

  switch(node->type){
    case EXP_TYPE_INT: //falthrough
    case EXP_TYPE_FLOAT: //fallthrough
    case EXP_TYPE_BOOL:
      asm_push_const_i(1);
      asm_push_const_i(1);
      break;
    case EXP_TYPE_STRING:
      // TODO
      break;
    case EXP_TYPE_ARITH:
      // TODO
      break;
    case EXP_TYPE_INDEX:
      // TODO
      break;
    case EXP_TYPE_CALL: //fallthrough
    case EXP_TYPE_MAT_DEC: //fallthrough
    case EXP_TYPE_PARAM:
    {
      MatrixSize size;
      if (node->type == EXP_TYPE_MAT_DEC) {
        size = node->matDecExp.size;
      } else if (node->type == EXP_TYPE_PARAM) {
        size = node->paramExp.size;
      } else if(node->type == EXP_TYPE_CALL) {
        ClmSymbol *sym = clm_scope_find(data.scope, node->callExp.name);
        ClmStmtNode *decl = sym->declaration;
        size = decl->funcDecStmt.returnSize;
      }

      if (size.colVar != NULL) {
        // push dword [ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, size.colVar);
        load_var_location(sym, index_str, 8, NULL);
        asm_push(index_str);
      } else {
        // push $colInd
        asm_push_const_i(size.cols);
      }

      if (size.rowVar != NULL) {
        // push dword [ebp+offset]
        ClmSymbol *sym = clm_scope_find(data.scope, size.rowVar);
        load_var_location(sym, index_str, 4, NULL);
        asm_push(index_str);
      } else {
        // push $rowInd
        asm_push_const_i(size.rows);
      }
      break;
    }
    case EXP_TYPE_UNARY:
      gen_exp_size(node->unaryExp.node);
      break;
    default:
      break;
  }
}

// pops a matrix that is on the stack, into the variable contained in node
static void pop_into_whole_matrix(ClmExpNode *node){
  // TODO case where matrix on stack is actually a pointer
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  asm_pop(EAX);       // pop type
  asm_pop(ECX);       // pop rows
  asm_pop(EBX);       // pop cols;
  // todo assert rows == A.rows && cols == A.cols
  asm_imul(ECX, EBX); // ecx now contains rows * cols
  asm_dec(ECX);

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);

  asm_mov(EAX, ECX);
  asm_imul(EAX, "4");

  load_var_location(var, index_str, 12, EAX);
  asm_pop(index_str);

  asm_dec(ECX);
  asm_jmp(cmp_label);
  asm_label(end_label);
}

// pushes a matrix identified by the node onto the stack
static void push_whole_matrix(ClmExpNode *node){
  // TODO case where matrix on stack is actually a pointer
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  LOAD_COLS(var, index_str);
  asm_mov(EDX, index_str); // edx = cols
  LOAD_ROWS(var, index_str);
  asm_mov(EBX, index_str); // ebx = rows
  asm_mov(ECX, EBX);
  asm_imul(ECX, EDX);
  asm_dec(ECX); // ecx = rows * cols - 1

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);

  asm_mov(EAX, ECX);
  asm_imul(EAX, "4");

  load_var_location(var, index_str, 12, EAX);
  asm_push(index_str);

  asm_dec(ECX);
  asm_jmp(cmp_label);
  asm_label(end_label);

  // push type info
  asm_push(EDX); // cols
  asm_push(EBX); // rows
  asm_push((int)CLM_TYPE_MATRIX);
}

// pops a matrix from the stack into a row or column of a matrix identified by node
static void pop_into_row_or_col(ClmExpNode *node){
  // TODO case where matrix on stack is actually a pointer
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  push_expression(node->indExp.rowIndex);
  push_expression(node->indExp.colIndex);
  asm_pop(EDX); // edx now contains the index value

  asm_pop(EAX);       // pop type
  asm_pop(EAX);       // pop rows
  asm_pop(EBX);       // pop cols
  //todo assert rows == A.rows or cols == A.cols
  
  asm_mov(ECX, "0");

  if(node->indExp.rowIndex != NULL){
    /*
      A[x,]
      for i in 1..A.cols do
        A[x * A.cols + i] = pop
      end
    */
    LOAD_COLS(var, index_str);
    asm_imul(EDX, index_str);
    
    asm_label(cmp_label);
    asm_cmp(ECX, index_str);
    asm_jmp_eq(end_label);

    asm_mov(EAX, EDX);
    asm_add(EAX, ECX); // eax now contains rowIndex * A.cols + i
    asm_imul(EAX, "4");
    load_var_location(var, index_str, 12, EAX);
    asm_pop(index_str);
  }else{
    /*
      A[,y]
      for i in 1..A.rows do
        A[i * A.cols + y] = pop
      end
    */
    LOAD_ROWS(var, index_str);

    asm_label(cmp_label);
    asm_cmp(ECX, index_str);
    asm_jmp_eq(end_label);

    asm_mov(EAX, ECX);
    asm_imul(EAX, index_str);
    asm_add(EAX, EDX);
    asm_imul(EAX, "4");
    load_var_location(var, index_str, 12, EAX);
    asm_pop(index_str);
  }

  asm_inc(ECX);
  asm_jmp(cmp_label);
  asm_label(end_label);
}

// pushes a row or col of the matrix identified by node onto the stack
static void push_row_or_col(ClmExpNode *node){
  // TODO case where matrix on stack is actually a pointer
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  push_expression(node->indExp.rowIndex);
  push_expression(node->indExp.colIndex);
  asm_pop(EDX); // edx now contains the index value

  if(node->indExp.rowIndex != NULL){
    /*
      push A[x,]
      for i in A.cols,-1..1 do
        push A[x * A.cols + i]
      end
    */
    LOAD_COLS(var, index_str);
    asm_mov(ECX, index_str);
    asm_imul(EDX, ECX);
    asm_dec(ECX);
    
    asm_label(cmp_label);
    asm_cmp(ECX, "0");
    asm_jmp_l(end_label);

    asm_mov(EAX, EDX);
    asm_add(EAX, ECX); // eax = rowIndex * A.cols + i
    asm_imul(EAX, "4");
    load_var_location(var, index_str, 12, EAX);
    asm_push(index_str);
  }else{
    /*
      push A[,y]
      for i in A.rows,-1..1 do
        push A[i * A.cols + y]
      end
    */
    LOAD_ROWS(var, index_str);
    asm_mov(ECX, index_str);
    asm_dec(ECX);

    asm_label(cmp_label);
    asm_cmp(ECX, "0");
    asm_jmp_l(end_label);

    asm_mov(EAX, ECX);
    asm_imul(EAX, index_str);
    asm_add(EAX, EDX); // eax = i * A.cols + y
    asm_imul(EAX, "4");
    load_var_location(var, index_str, 12, EAX);
    asm_push(index_str);
  }

  asm_inc(ECX);
  asm_jmp(cmp_label);
  asm_label(end_label);
}

static void pop_matrix(ClmExpNode *node) {
  if (node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL) {
    pop_into_whole_matrix(node);
  } else if (node->indExp.rowIndex == NULL || node->indExp.colIndex == NULL) {
    pop_into_row_or_col(node);
  } else {
    char index_str[64];
    ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
    push_expression(node->indExp.colIndex);
    push_expression(node->indExp.rowIndex);
    pop_int_into(EAX); // pop rows into eax
    pop_int_into(EBX); // pop cols into ebx

    LOAD_COLS(var, index_str);
    asm_mov(ECX, index_str); // mov number of columns into ecx
    asm_imul(EAX, ECX);      // eax = nc * row
    asm_add(EAX, EBX);       // eax = nc * row + col
    asm_imul(EAX, "4");      // eax = 4 * (row * nc + col)
    load_var_location(var, index_str, 12, EAX);
    pop_int_into(index_str);
  }
}

static void push_matrix(ClmExpNode *node){
  if (node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL) {
    push_whole_matrix(node);
  } else if (node->indExp.rowIndex == NULL || node->indExp.colIndex == NULL) {
    push_row_or_col(node);
  } else {
    char index_str[64];
    ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
    // A[x * nc + y]
    push_expression(node->indExp.colIndex);
    push_expression(node->indExp.rowIndex);
    pop_int_into(EAX); // pop row index into eax
    pop_int_into(EBX); // pop col index into ebx

    LOAD_COLS(var, index_str);
    asm_imul(EAX, index_str); // EAX = rowIndex * nc
    asm_add(EAX, EBX); // EAX = rowIndex * nc + colIndex
    asm_imul(EAX, "4"); // EAX = 4 * rowIndex * nc + colIndex
    load_var_location(var, index_str, 12, EAX);
    asm_push(index_str);
    asm_push_const_i((int)CLM_TYPE_INT);
  }
}

static void pop_into_lhs(ClmExpNode *node) {
  // it is an index node - otherwise it is a type check fail
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);

  switch (var->type) {
  case CLM_TYPE_INT:
    load_var_location(var, index_str, 4, NULL);
    pop_int_into(index_str);
    break;
  case CLM_TYPE_FLOAT:
    load_var_location(var, index_str, 4, NULL);
    pop_float_into(index_str);
    break;
  case CLM_TYPE_MATRIX:
    pop_matrix(node);
    break;
  case CLM_TYPE_STRING:
    // uhh
    break;
  case CLM_TYPE_FUNCTION: //fallthrough
  case CLM_TYPE_NONE:
    // shouldn't get here...
    break;
  }
}

static void push_index(ClmExpNode *node) {
  char index_str[64];
  ClmSymbol *var = clm_scope_find(data.scope, node->indExp.id);
  switch (var->type) {
  case CLM_TYPE_INT:
    load_var_location(var, index_str, 4, NULL);
    asm_push(index_str);
    asm_push_const_i((int)var->type);
    break;
  case CLM_TYPE_FLOAT:
    load_var_location(var, index_str, 4, NULL);
    asm_push_f(index_str);
    asm_push_const_i((int)var->type);
    break;
  case CLM_TYPE_MATRIX:
    push_matrix(node);
    break;
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
static void push_expression(ClmExpNode *node) {
  if(node == NULL) return;

  ClmType expression_type = clm_type_of_exp(node, data.scope);
  switch (node->type) {
  case EXP_TYPE_INT:
    asm_push_const_i(node->ival);
    asm_push_const_i((int)expression_type);
    break;
  case EXP_TYPE_FLOAT:
    asm_push_const_f(node->fval);
    asm_push_const_i((int)expression_type);
    break;
  case EXP_TYPE_STRING:
    // TODO
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
      push_expression(node->arithExp.left);
      asm_mov(EDX, ESP);
      push_expression(node->arithExp.right);
      gen_arith(node);
    } else {
      push_expression(node->arithExp.right);
      asm_mov(EDX, ESP);
      push_expression(node->arithExp.left);
      gen_arith(node);
    }
    break;
  }
  case EXP_TYPE_BOOL:
    push_expression(node->boolExp.right);
    asm_mov(EDX, ESP);
    push_expression(node->boolExp.left);
    gen_bool(node);
    break;
  case EXP_TYPE_CALL: {
    // first push everything thats not a matrix... and for matrices push a pointer
    int tempStartID = data.temporaryID;
    int i;
    ClmExpNode *param;
    char temporary[256];

    // first for any matrices that are parameters that will be pushed through
    // the stack, push them on the stack and save their location into a temporary
    // global
    for (i = node->callExp.params->length - 1; i >= 0; i--) {
      param = node->callExp.params->data[i];
      if(param->type == CLM_TYPE_MATRIX){
        ClmLocation location = clm_location_of_exp(param, data.scope);
        switch(location){
            case LOCATION_STACK:
              push_expression(param);
              next_temporary(temporary);
              asm_mov(temporary, ESP);
              break;
            default:
              break;
        }
      }
    }

    // then push every expression.. when we get to a matrix, push the pointer
    // to its location
    int tempOffset = 1;
    char index_str[256];
    for (i = node->callExp.params->length - 1; i >= 0; i--) {
      param = node->callExp.params->data[i];
      if(param->type == CLM_TYPE_MATRIX){
        ClmLocation location = clm_location_of_exp(param, data.scope);
        switch(location){
            case LOCATION_STACK:
              sprintf(temporary, "dword [temporary%d]", tempStartID + tempOffset);
              asm_push(temporary);
              tempOffset++;
              break;
            default:
            {
              // the only way its a matrix and not on the stack is if its an
              // ind exp with no indices
              ClmSymbol *symbol = clm_scope_find(data.scope, param->indExp.id);
              load_var_location(symbol, index_str, 0, NULL);
              asm_push(index_str);
              break;
            }
        }
        asm_push_const_i((int) CLM_TYPE_MATRIX);
      }else{
        push_expression(param);
      }
    }

    asm_call(node->callExp.name);
    break;
  }
  case EXP_TYPE_INDEX:
    push_index(node);
    break;
  case EXP_TYPE_MAT_DEC: {
    int i;
    if (node->matDecExp.arr != NULL) {
      for (i = node->matDecExp.length - 1; i >= 0; i--) {
        // TODO... push f or push i?
        asm_push_const_i((int) node->matDecExp.arr[i]);
      }
      asm_push_const_i(node->matDecExp.size.cols);
      asm_push_const_i(node->matDecExp.size.rows);
      asm_push_const_i((int)CLM_TYPE_MATRIX);
    } else {
      // push a matrix onto the stack with all 0s
      char cmp_label[LABEL_SIZE];
      char end_label[LABEL_SIZE];
      next_label(cmp_label);
      next_label(end_label);

      gen_exp_size(node);
      asm_pop(EAX); // # rows
      asm_pop(EBX); // # cols

      asm_mov(ECX, EAX);
      asm_imul(ECX, EBX);
      asm_dec(ECX);
      asm_label(cmp_label);
      asm_cmp(ECX, "0");
      asm_jmp_l(end_label);
      asm_push_const_i(0);
      asm_dec(ECX);
      asm_jmp(cmp_label);
      asm_label(end_label);
      asm_push(EBX);
      asm_push(EAX);
      asm_push_const_i((int)CLM_TYPE_MATRIX);
    }
    break;
  }
  case EXP_TYPE_PARAM:
    // TODO ?
    break;
  case EXP_TYPE_UNARY:
    push_expression(node->unaryExp.node);
    gen_unary(node);
    break;
  }
}

static void gen_conditional(ClmStmtNode *node) {
  push_expression(node->conditionStmt.condition);

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
}

static void gen_func_dec(ClmStmtNode *node) {
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
  ClmExpNode *dec;
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  char index_str[32];
  for (i = 0; i < funcScope->symbols->length; i++) {
    sym = funcScope->symbols->data[i];
    dec = sym->declaration;

    if (sym->location = LOCATION_PARAMETER)
      continue;
    
    // setting the type of the local var
    load_var_location(sym, index_str, 0, NULL);
    asm_mov_i(index_str, (int)sym->type);

    // setting the value of the local var
    load_var_location(sym, index_str, 4, NULL);
    asm_mov_i(index_str, 0);

    if (sym->type == CLM_TYPE_MATRIX) {
      // TODO what does this do? does it work?
      next_label(cmp_label);
      next_label(end_label);

      gen_exp_size(dec);
      asm_pop(EAX); // eax contains num of rows
      asm_pop(EBX); // ebx contains num of cols
      asm_mov(ECX, EAX);
      asm_imul(ECX, EBX);
      asm_label(cmp_label);
      asm_dec(ECX);
      asm_cmp(ECX, "0");
      asm_jmp_eq(end_label);
      asm_push_const_i(0);
      asm_jmp(cmp_label);
      asm_label(end_label);
      asm_push(EBX); // cols

      // setting the pointer to point at the rows
      // note: push changes the value at esp and THEN
      // decrements it
      load_var_location(sym, index_str, 4, NULL);
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

  if (node->funcDecStmt.returnSize.rows == -1) {
    // no return value!
    asm_mov(ESP, EBP);
    asm_pop(EBP);
  }
  asm_ret();
}

static void gen_loop(ClmStmtNode *node) {
  char cmp_label[LABEL_SIZE];
  char end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  ClmSymbol *var = clm_scope_find(data.scope, node->loopStmt.varId);
  char loop_var[32];
  load_var_location(var, loop_var, 4, NULL);

  // don't need to store this - just evaluate and put into loop var
  push_expression(node->loopStmt.start);

  if (!node->loopStmt.startInclusive) {
    push_expression(node->loopStmt.delta);
    pop_int_into(EAX); // eax contains delta
    pop_int_into(EBX); // ebx contains start
    asm_add(EBX, EAX);

    asm_push(EBX); // push start back on stack
    asm_push_const_i((int)CLM_TYPE_INT);
  }
  pop_int_into(loop_var);
  asm_label(cmp_label);

  // don't need to store this - just evaulate every loop
  push_expression(node->loopStmt.end);

  pop_int_into(EAX);

  asm_cmp(loop_var, EAX);
  if (node->loopStmt.endInclusive)
    asm_jmp_g(end_label);
  else
    asm_jmp_ge(end_label);

  gen_statements(node->loopStmt.body);

  if (node->loopStmt.delta->type == EXP_TYPE_INT &&
      node->loopStmt.delta->ival == 1) {
    asm_inc(loop_var);
  } else if (node->loopStmt.delta->type == EXP_TYPE_INT &&
             node->loopStmt.delta->ival == -1) {
    asm_dec(loop_var);
  } else if (node->loopStmt.delta->type == EXP_TYPE_INT) {
    asm_add_i(loop_var, node->loopStmt.delta->ival);
  } else {
    push_expression(node->loopStmt.delta);
    asm_pop(EAX);
    asm_add(loop_var, EAX);
  }

  asm_jmp(cmp_label);
  asm_label(end_label);
}

static void gen_statement(ClmStmtNode *node) {
  switch (node->type) {
  case STMT_TYPE_ASSIGN:
    push_expression(node->assignStmt.rhs);
    pop_into_lhs(node->assignStmt.lhs);
    break;
  case STMT_TYPE_CALL:
    push_expression(node->callExpr);
    break;
  case STMT_TYPE_CONDITIONAL:
    gen_conditional(node);
    break;
  case STMT_TYPE_FUNC_DEC:
    gen_func_dec(node);
    break;
  case STMT_TYPE_LOOP:
    gen_loop(node);
    break;
  case STMT_TYPE_PRINT:
    push_expression(node->printStmt.expression);
    gen_print_type(clm_type_of_exp(node->printStmt.expression, data.scope),
                   node->printStmt.appendNewline);
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

    // note: T_EAX and T_EBX are globals defined in clm_asm.h
    asm_mov(ESP, EBP);      // reset stack pointer to above the function
    asm_pop("[" T_EBX "]"); // pop the old frame pointer into t_ebx
    asm_pop("[" T_EAX "]"); // pop the stack address of the next instruction to
    // execute after call finishes
    push_expression(node->returnExpr);
    asm_mov(EBP, "[" T_EBX "]"); // move the old frame pointer into ebp
    asm_push("[" T_EAX
             "]"); // push the stack address of the next instruction to
    // execute after call finishes
    asm_ret(); // return
    break;
  }
}

static void gen_globals(ClmScope *globalScope) {
  int i;
  ClmSymbol *symbol;
  char buffer[256];
  for (i = 0; i < globalScope->symbols->length; i++) {
    symbol = globalScope->symbols->data[i];
    switch (symbol->type) {
    case CLM_TYPE_INT:
      sprintf(buffer, "_%s dd %d, 0\n", symbol->name, (int)CLM_TYPE_INT);
      writeLine(buffer);
      break;
    case CLM_TYPE_FLOAT:
      sprintf(buffer, "_%s dd %d, 0\n", symbol->name, (int)CLM_TYPE_FLOAT);
      writeLine(buffer);
      break;
    case CLM_TYPE_STRING:
      // TODO
      break;
    case CLM_TYPE_MATRIX: {
      ClmStmtNode *node = symbol->declaration;
      ClmExpNode *val = node->assignStmt.rhs;
      int rows, cols;
      if (clm_size_of_exp(val, globalScope, &rows, &cols)) {
        int i;
        sprintf(buffer, "_%s dd %d, %d, %d", symbol->name, (int)CLM_TYPE_MATRIX,
                rows, cols);
        for (i = 0; i < rows * cols; i++) {
          strcat(buffer, ", 0");
        }
        strcat(buffer, "\n");
        writeLine(buffer);
      }
      break;
    }
    default:
      break;
    }
  }

  // temporaries are integers used for pushing matrices into functions
  while(data.temporaryID > 0){
    sprintf(buffer, "temporary%d dd %d, 0\n", data.temporaryID, (int)CLM_TYPE_INT);
    writeLine(buffer);
    data.temporaryID--;
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
  data.temporaryID = 0;
  data.inFunction = 0;
  data.size = strlen(ASM_HEADER) + 1;
  data.capacity = data.size;
  data.code = malloc(strlen(ASM_HEADER) + 1);
  strcpy(data.code, ASM_HEADER);

  gen_functions(statements);

  writeLine(ASM_START);
  gen_statements(statements);

  writeLine(ASM_EXIT_PROCESS);
  writeLine(ASM_DATA);

  gen_globals(globalScope);

  return data.code;
}
