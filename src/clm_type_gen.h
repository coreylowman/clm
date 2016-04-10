#ifndef CLM_MAT_GEN_H
#define CLM_MAT_GEN_H

#include "clm_expression.h"
#include "clm_type.h"

//
// Arith operations
//

void gen_mat_arith(ArithOp op, ClmType other_type);
void gen_int_arith(ArithOp op, ClmType other_type);
void gen_float_arith(ArithOp op, ClmType other_type);
void gen_string_arith(ArithOp op, ClmType other_type);

//
// Boolean operations
//

void gen_mat_bool(BoolOp op, ClmType other_type);
void gen_int_bool(BoolOp op, ClmType other_type);
void gen_float_bool(BoolOp op, ClmType other_type);
void gen_string_bool(BoolOp op, ClmType other_type);

//
// Unary operations
//
void gen_mat_unary(UnaryOp op);
void gen_int_unary(UnaryOp op);
void gen_float_unary(UnaryOp op);
void gen_string_unary(UnaryOp op);

//
// printing
//
void gen_print_type(ClmType type, int nl);
void gen_print_mat(int nl);
void gen_print_int(int nl);
void gen_print_float(int nl);
void gen_print_string(int nl);

#endif
