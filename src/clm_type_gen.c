#include "clm_type_gen.h"
#include "clm_asm.h"

// arith
static void gen_mat_add_mat();
static void gen_mat_sub_mat();
static void gen_mat_mul_mat();
static void gen_mat_div_mat();
static void gen_mat_mul_int();
static void gen_mat_div_int();
static void gen_mat_mul_float();
static void gen_mat_div_float();

static void gen_int_add_int();
static void gen_int_sub_int();
static void gen_int_mul_int();
static void gen_int_div_int();
static void gen_int_add_float();
static void gen_int_sub_float();
static void gen_int_mul_float();
static void gen_int_div_float();
static void gen_int_mul_mat();

static void gen_float_add_int();
static void gen_float_sub_int();
static void gen_float_mul_int();
static void gen_float_div_int();
static void gen_float_add_float();
static void gen_float_sub_float();
static void gen_float_mul_float();
static void gen_float_div_float();
static void gen_float_mul_mat();

static void gen_string_add_string();

// bool
static void gen_mat_and_mat();
static void gen_mat_or_mat();
static void gen_mat_cmp_mat(BoolOp op);

static void gen_int_and_int();
static void gen_int_and_float();
static void gen_int_or_int();
static void gen_int_or_float();
static void gen_int_cmp_int(BoolOp op);
static void gen_int_cmp_float(BoolOp op);

static void gen_float_and_int();
static void gen_float_and_float();
static void gen_float_or_int();
static void gen_float_or_float();
static void gen_float_cmp_int(BoolOp op);
static void gen_float_cmp_float(BoolOp op);

static void gen_string_cmp_string(BoolOp op);

// unary
static void gen_mat_minus();
static void gen_mat_transpose();

static void gen_int_minus();
static void gen_int_not();

static void gen_float_minus();

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
  default:
    // shouldn't get here
    break;
  }
}

/*
        lrows = [edx - 8]
        lele = lrows * [edx - 12]

        for(ecx = lele - 1, ecx >= 0, ecx--)
                ebx = 16 + ecx * 4
                add [esp - ebx], [edx - ebx]

        pop matrix
*/
static void gen_mat_add_mat() {
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
        lrows = [edx - 8]
        lele = lrows * [edx - 12]

        for(ecx = lele - 1, ecx >= 0, ecx--)
                ebx = 16 + ecx * 4
                sub [esp - ebx], [edx - ebx]

        pop matrix
*/
static void gen_mat_sub_mat() {
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

static void gen_mat_mul_mat() {
  // TODO
}

static void gen_mat_div_mat() {
  // TODO
}

/*
        matrix
        cols
        rows
        matrix type
        int val <-edx
        int type
        <- esp
*/
static void gen_mat_mul_int() {
  // note this funcs is genned differently... see code_gen gen_arith comment
  gen_int_mul_mat();
}

static void gen_mat_div_int() {
  // note this funcs is genned differently... see code_gen gen_arith comment
  // TODO
}

static void gen_mat_mul_float() {
  // note this funcs is genned differently... see code_gen gen_arith comment
  // TODO
}

static void gen_mat_div_float() {
  // note this funcs is genned differently... see code_gen gen_arith comment
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
  default:
    // shouldn't get here
    break;
  }
}

static void gen_int_add_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_add(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

static void gen_int_sub_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_sub(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

static void gen_int_mul_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_imul(EAX, EBX);
  asm_push(EAX);
  asm_push_i((int)CLM_TYPE_INT);
}

static void gen_int_div_int() {
  // TODO
}

static void gen_int_add_float() {
  // TODO
}

static void gen_int_sub_float() {
  // TODO
}

static void gen_int_mul_float() {
  // TODO
}

static void gen_int_div_float() {
  // TODO
}

/*
        lrows = [edx - 8]
        lele = lrows * [edx - 12]
        eax = [esp - 4]
        for(ecx = lele - 1, ecx >= 0, ecx--)
                ebx = 16 + ecx * 4
                imul [edx - ebx], eax

        pop matrix
*/
static void gen_int_mul_mat() {
  char cmp_label[LABEL_SIZE], end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  asm_mov(ECX, "[edx - 8]");
  asm_imul(ECX, "[edx - 12]");
  asm_dec(ECX);

  asm_mov(EAX, "[esp - 8]");

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);

  asm_mov(EBX, ECX);
  asm_imul(EBX, "4");
  asm_add(EBX, "16");
  asm_imul("dword [edx - ebx]", "eax");

  asm_dec(ECX);
  asm_jmp(cmp_label);

  asm_label(end_label);

  asm_mov(ECX, "[edx - 8]");
  asm_imul(ECX, "[edx - 12]");
  pop_matrix_of_size(ECX);
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
  default:
    // shouldn't get here
    break;
  }
}

static void gen_float_add_int() {
  // TODO
}

static void gen_float_sub_int() {
  // TODO
}

static void gen_float_mul_int() {
  // TODO
}

static void gen_float_div_int() {
  // TODO
}

static void gen_float_add_float() {
  // TODO
}

static void gen_float_sub_float() {
  // TODO
}

static void gen_float_mul_float() {
  // TODO
}

static void gen_float_div_float() {
  // TODO
}

static void gen_float_mul_mat() {
  // TODO
}

// string arith
void gen_string_arith(ArithOp op, ClmType other_type) {
  if (op == ARITH_OP_ADD && other_type == CLM_TYPE_STRING)
    gen_string_add_string();
}

static void gen_string_add_string() {
  // TODO
}

void gen_mat_bool(BoolOp op, ClmType other_type) {
  // other type here can only be CLM_TYPE_MATRIX
  switch (op) {
  case BOOL_OP_AND:
    gen_mat_and_mat();
    break;
  case BOOL_OP_OR:
    gen_mat_or_mat();
    break;
  default:
    gen_mat_cmp_mat(op);
    break;
  }
}

static void gen_mat_and_mat() {
  // TODO
}

static void gen_mat_or_mat() {
  // TODO
}

typedef void (*asm_func1)(const char *);
/*
        lrows = [esp - 8]
        rrows = [edx - 8]
        cmp lrows, rrows
        jneq false_label

        lele = lrows * [esp - 12]
        rele = rrows * [edx - 12]
        cmp lele, rele
        jneq false_label

        for(ecx = lele - 1, ecx >= 0, ecx--)
                eax = 16 + ecx * 4
                cmp [esp - eax], [edx - eax]
                cmp_func false_label

        pop matrices
        push 1
        jmp end_label
false_label
        pop matrices
        push 0
end_label
*/
static void gen_mat_cmp_mat(BoolOp op) {
  asm_func1 jmp_func;
  switch (op) {
  case BOOL_OP_GT:
    jmp_func = asm_jmp_le;
    break;
  case BOOL_OP_LT:
    jmp_func = asm_jmp_ge;
    break;
  case BOOL_OP_GTE:
    jmp_func = asm_jmp_l;
    break;
  case BOOL_OP_LTE:
    jmp_func = asm_jmp_g;
    break;
  case BOOL_OP_EQ:
    jmp_func = asm_jmp_neq;
    break;
  case BOOL_OP_NEQ:
    jmp_func = asm_jmp_eq;
    break;
  default:
    return;
  }

  char cmp_label[256], true_label[256], false_label[256], end_label[256];
  next_label(cmp_label);
  next_label(true_label);
  next_label(false_label);
  next_label(end_label);

  // compare rows
  asm_mov(EAX, "[esp - 8]");
  asm_mov(EBX, "[edx - 8]");
  asm_cmp(EAX, EBX);
  asm_jmp_neq(false_label);

  // compare num elements
  asm_imul(EAX, "[esp - 12]");
  asm_imul(EBX, "[edx - 12]");
  asm_cmp(EAX, EBX);
  asm_jmp_neq(false_label);

  // ecx = num elements - 1
  asm_mov(ECX, EAX);
  asm_dec(ECX);

  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(true_label);

  // eax = 16 + 4 * ecx
  asm_mov(EAX, ECX);
  asm_imul(EAX, "4");
  asm_add(EAX, "16");
  asm_cmp("[esp - eax]", "[edx - eax]");
  jmp_func(false_label);

  asm_dec(EAX);
  asm_jmp(cmp_label);

  asm_label(true_label);
  pop_matrix_of_size(EBX); // ebx still contains number of elements
  asm_push_i(1);
  asm_jmp(end_label);

  asm_label(false_label);
  pop_matrix_of_size(EBX); // ebx still contains number of elements
  asm_push_i(0);

  asm_label(end_label);
}

void gen_int_bool(BoolOp op, ClmType other_type) {
  switch (op) {
  case BOOL_OP_AND:
    if (other_type == CLM_TYPE_INT) {
      gen_int_and_int();
    } else {
      // other_type == CLM_TYPE_FLOAT
      gen_int_and_float();
    }
    break;
  case BOOL_OP_OR:
    if (other_type == CLM_TYPE_INT) {
      gen_int_or_int();
    } else {
      // other_type == CLM_TYPE_FLOAT
      gen_int_or_float();
    }
    break;
  default:
    if (other_type == CLM_TYPE_INT) {
      gen_int_cmp_int(op);
    } else {
      gen_int_cmp_float(op);
    }
    break;
  }
}

static void gen_int_and_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_and(EAX, EBX);
  asm_push(EAX);
}

static void gen_int_and_float() {
  // TODO
}

static void gen_int_or_int() {
  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_or(EAX, EBX);
  asm_push(EAX);
}

static void gen_int_or_float() {
  // TODO
}

static void gen_int_cmp_int(BoolOp op) {
  asm_func1 jmp_func;
  switch (op) {
  case BOOL_OP_GT:
    jmp_func = asm_jmp_le;
    break;
  case BOOL_OP_LT:
    jmp_func = asm_jmp_ge;
    break;
  case BOOL_OP_GTE:
    jmp_func = asm_jmp_l;
    break;
  case BOOL_OP_LTE:
    jmp_func = asm_jmp_g;
    break;
  case BOOL_OP_EQ:
    jmp_func = asm_jmp_neq;
    break;
  case BOOL_OP_NEQ:
    jmp_func = asm_jmp_eq;
    break;
  default:
    return;
  }

  char end_label[256];
  char false_label[256];
  next_label(false_label);
  next_label(end_label);

  pop_int_into(EAX);
  pop_int_into(EBX);
  asm_cmp(EAX, EBX);

  jmp_func(false_label);

  asm_push_i(1);
  asm_jmp(end_label);
  asm_label(false_label);
  asm_push_i(0);
  asm_label(end_label);
}

static void gen_int_cmp_float(BoolOp op) {
  // TODO
}

void gen_float_bool(BoolOp op, ClmType other_type) {
  switch (op) {
  case BOOL_OP_AND:
    if (other_type == CLM_TYPE_INT) {
      gen_float_and_int();
    } else {
      // other_type == CLM_TYPE_FLOAT
      gen_float_and_float();
    }
    break;
  case BOOL_OP_OR:
    if (other_type == CLM_TYPE_INT) {
      gen_float_or_int();
    } else {
      // other_type == CLM_TYPE_FLOAT
      gen_float_or_float();
    }
    break;
  default:
    if (other_type == CLM_TYPE_INT) {
      gen_float_cmp_int(op);
    } else {
      gen_float_cmp_float(op);
    }
    break;
  }
}

static void gen_float_and_int() {
  // TODO
}

static void gen_float_and_float() {
  // TODO
}

static void gen_float_or_int() {
  // TODO
}

static void gen_float_or_float() {
  // TODO
}

static void gen_float_cmp_int(BoolOp op) {
  // TODO
}

static void gen_float_cmp_float(BoolOp op) {
  // TODO
}

void gen_string_bool(BoolOp op, ClmType other_type) {
  // other type here can only be CLM_TYPE_STRING
  switch (op) {
  case BOOL_OP_EQ:
  case BOOL_OP_NEQ:
    gen_string_cmp_string(op);
    break;
  default:
    return;
  }
}

static void gen_string_cmp_string(BoolOp op) {
  // TODO
}

void gen_mat_unary(UnaryOp op) {
  switch (op) {
  case UNARY_OP_TRANSPOSE:
    gen_mat_transpose();
    break;
  case UNARY_OP_MINUS:
    gen_mat_minus();
    break;
  }
}

static void gen_mat_minus() {
  char cmp_label[LABEL_SIZE], end_label[LABEL_SIZE];
  next_label(cmp_label);
  next_label(end_label);

  asm_mov(ECX, "[esp - 8]");
  asm_imul(ECX, "[esp - 12]");
  asm_dec(ECX);
  asm_label(cmp_label);
  asm_cmp(ECX, "0");
  asm_jmp_l(end_label);
  asm_neg("dword [esp - ecx]");
  asm_dec(ECX);
  asm_jmp(cmp_label);
  asm_label(end_label);
}

static void gen_mat_transpose() {
  // TODO
  // https://en.wikipedia.org/wiki/In-place_matrix_transposition
}

void gen_int_unary(UnaryOp op) {
  switch (op) {
  case UNARY_OP_MINUS:
    gen_int_minus();
    break;
  case UNARY_OP_NOT:
    gen_int_not();
    break;
  }
}

static void gen_int_minus() { asm_neg("dword [esp - 8]"); }

static void gen_int_not() { asm_xor("dword [esp - 8]", "1"); }

void gen_float_unary(UnaryOp op) {
  // only op can be minus
  gen_float_minus();
}

static void gen_float_minus() {
  // TODO
}

void gen_string_unary(UnaryOp op) {
  // shouldn't get called
}
