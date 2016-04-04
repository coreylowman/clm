#include "clm_type_gen.h"
#include "clm_asm.h"

// stack will look like this
// where left will always be the matrix
// right
// right type
// left <- edx
// left type
// <- esp

extern void next_label(char *buffer);

void gen_mat_arith(ArithOp op, ClmType other_type) {
  switch (other_type) {
  case CLM_TYPE_INT:
    if (op == ARITH_OP_MULT)
      gen_mat_mul_int();
    else if (op == ARITH_OP_DIV)
      gen_mat_div_int();
    break;
  case CLM_TYPE_FLOAT:
    if (op == ARITH_OP_MULT)
      gen_mat_mul_float();
    else if (op == ARITH_OP_DIV)
      gen_mat_div_float();
    break;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT)
      gen_mat_mul_int();
    else if (op == ARITH_OP_ADD)
      gen_mat_add_mat();
    else if (op == ARITH_OP_SUB)
      gen_mat_sub_mat();
    break;
  }
}

/*
        lrows = [edx + 8]
        lele = lrows * [edx + 12]

        for(ecx = lele - 1, ecx >= 0, ecx--)
                ebx = 16 + ecx * 4
                add [esp - ebx], [edx - ebx]

        pop matrix
*/
void gen_mat_add_mat() {
  char cmp_label[LABEL_SIZE], end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  asm_mov(ECX, "[edx - 8]");
  asm_imul(ECX, "[edx - 12]");
  asm_mov(EAX, ECX);
  asm_dec(ECX);

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);

  asm_mov(EBX, ECX);
  asm_imul(EBX, "4");
  asm_add(EBX, "16");
  asm_add("dword [esp - ebx]", "dword [edx - ebx]");

  asm_dec(ECX);
  asm_jmp(cmp_label);

  asm_label(end_label);

  pop_matrix_of_size(EAX);
}

/*
        lrows = [edx + 8]
        lele = lrows * [edx + 12]

        for(ecx = lele - 1, ecx >= 0, ecx--)
                ebx = 16 + ecx * 4
                sub [esp - ebx], [edx - ebx]

        pop matrix
*/
void gen_mat_sub_mat() {
  char cmp_label[LABEL_SIZE], end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  asm_mov(ECX, "[edx - 8]");
  asm_imul(ECX, "[edx - 12]");
  asm_mov(EAX, ECX);
  asm_dec(ECX);

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);

  asm_mov(EBX, ECX);
  asm_imul(EBX, "4");
  asm_add(EBX, "16");
  asm_sub("dword [esp - ebx]", "dword [edx - ebx]");

  asm_dec(ECX);
  asm_jmp(cmp_label);

  asm_label(end_label);

  pop_matrix_of_size(EAX);
}

void gen_mat_mul_mat() {
  // TODO
}

void gen_mat_div_mat() {
  // TODO
}

void gen_mat_mul_int() {
  // TODO
}

void gen_mat_div_int() {
  // TODO
}

void gen_mat_mul_float() {
  // TODO
}

void gen_mat_div_float() {
  // TODO
}

// int arith
void gen_int_arith(ArithOp op, ClmType other_type) {
  switch (other_type) {
  case CLM_TYPE_INT:
    if (op == ARITH_OP_ADD)
      gen_int_add_int();
    else if (op == ARITH_OP_SUB)
      gen_int_sub_int();
    else if (op == ARITH_OP_MULT)
      gen_int_mul_int();
    else if (op == ARITH_OP_DIV)
      gen_int_div_int();
    break;
  case CLM_TYPE_FLOAT:
    if (op == ARITH_OP_ADD)
      gen_int_add_float();
    else if (op == ARITH_OP_SUB)
      gen_int_sub_float();
    else if (op == ARITH_OP_MULT)
      gen_int_mul_float();
    else if (op == ARITH_OP_DIV)
      gen_int_div_float();
    break;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT)
      gen_int_mul_mat();
    break;
  }
}

void gen_int_add_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_add(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

void gen_int_sub_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_sub(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

void gen_int_mul_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_imul(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

void gen_int_div_int() {
  // TODO
}

void gen_int_add_float() {
  // TODO
}

void gen_int_sub_float() {
  // TODO
}

void gen_int_mul_float() {
  // TODO
}

void gen_int_div_float() {
  // TODO
}

void gen_int_mul_mat() {
  // TODO
}

// float arith
void gen_float_arith(ArithOp op, ClmType other_type) {
  switch (other_type) {
  case CLM_TYPE_INT:
    if (op == ARITH_OP_ADD)
      gen_float_add_int();
    else if (op == ARITH_OP_SUB)
      gen_float_sub_int();
    else if (op == ARITH_OP_MULT)
      gen_float_mul_int();
    else if (op == ARITH_OP_DIV)
      gen_float_div_int();
    break;
  case CLM_TYPE_FLOAT:
    if (op == ARITH_OP_ADD)
      gen_float_add_float();
    else if (op == ARITH_OP_SUB)
      gen_float_sub_float();
    else if (op == ARITH_OP_MULT)
      gen_float_mul_float();
    else if (op == ARITH_OP_DIV)
      gen_float_div_float();
    break;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT)
      gen_float_mul_mat();
    break;
  }
}

void gen_float_add_int() {
  // TODO
}

void gen_float_sub_int() {
  // TODO
}

void gen_float_mul_int() {
  // TODO
}

void gen_float_div_int() {
  // TODO
}

void gen_float_add_float() {
  // TODO
}

void gen_float_sub_float() {
  // TODO
}

void gen_float_mul_float() {
  // TODO
}

void gen_float_div_float() {
  // TODO
}

void gen_float_mul_mat() {
  // TODO
}

// string arith
void gen_string_arith(ArithOp op, ClmType other_type) {
  if (op == ARITH_OP_ADD && other_type == CLM_TYPE_STRING)
    gen_string_add_string();
}

void gen_string_add_string() {
  // TODO
}
