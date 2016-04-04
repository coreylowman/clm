#ifndef CLM_MAT_GEN_H
#define CLM_MAT_GEN_H

#include "clm_expression.h"
#include "clm_type.h"

// mat arith
void gen_mat_arith(ArithOp op, ClmType other_type);

void gen_mat_add_mat();
void gen_mat_sub_mat();
void gen_mat_mul_mat();
void gen_mat_div_mat();

void gen_mat_mul_int();
void gen_mat_div_int();

void gen_mat_mul_float();
void gen_mat_div_float();

// int arith
void gen_int_arith(ArithOp op, ClmType other_type);

void gen_int_add_int();
void gen_int_sub_int();
void gen_int_mul_int();
void gen_int_div_int();

void gen_int_add_float();
void gen_int_sub_float();
void gen_int_mul_float();
void gen_int_div_float();

void gen_int_mul_mat();

// float arith
void gen_float_arith(ArithOp op, ClmType other_type);

void gen_float_add_int();
void gen_float_sub_int();
void gen_float_mul_int();
void gen_float_div_int();

void gen_float_add_float();
void gen_float_sub_float();
void gen_float_mul_float();
void gen_float_div_float();

void gen_float_mul_mat();

// string arith
void gen_string_arith(ArithOp op, ClmType other_type);

void gen_string_add_string();

#endif
