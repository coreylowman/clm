#include <stdio.h>
#include "clm_asm.h"

extern void writeLine(const char *line);

void asm_comment(const char *line) {
  char buffer[128];
  sprintf(buffer, "; %s\n", line);
  writeLine(buffer);
}

void asm_pop(const char *dest) {
  char buffer[32];
  sprintf(buffer, "pop %s\n", dest);
  writeLine(buffer);
}

void asm_push(const char *src) {
  char buffer[32];
  sprintf(buffer, "push %s\n", src);
  writeLine(buffer);
}

void asm_push_i(int val) {
  char buffer[32];
  sprintf(buffer, "push %d\n", val);
  writeLine(buffer);
}

void asm_push_f(float val) {
  char buffer[32];
  sprintf(buffer, "push %f\n", val);
  writeLine(buffer);
}

void asm_push_c(char val) {
  char buffer[32];
  sprintf(buffer, "push %c\n", val);
  writeLine(buffer);
}

void asm_add(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "add %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_add_i(const char *dest, int i) {
  char buffer[32];
  sprintf(buffer, "add %s,%d\n", dest, i);
  writeLine(buffer);
}

void asm_sub(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "sub %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_imul(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "imul %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_fadd(const char *dest, const char *other) {}

void asm_fsub(const char *dest, const char *other) {}

void asm_fmul(const char *dest, const char *other) {}

void asm_inc(const char *arg) {
  char buffer[32];
  sprintf(buffer, "inc %s\n", arg);
  writeLine(buffer);
}

void asm_dec(const char *arg) {
  char buffer[32];
  sprintf(buffer, "dec %s\n", arg);
  writeLine(buffer);
}

void asm_neg(const char *arg) {
  char buffer[32];
  sprintf(buffer, "dec %s\n", arg);
  writeLine(buffer);
}

void asm_mov(const char *dest, const char *src) {
  char buffer[32];
  sprintf(buffer, "mov %s,%s\n", dest, src);
  writeLine(buffer);
}

void asm_mov_i(const char *dest, int i) {
  char buffer[32];
  sprintf(buffer, "mov %s,%d\n", dest, i);
  writeLine(buffer);
}

void asm_lea(const char *dest, const char *src) {
  char buffer[32];
  sprintf(buffer, "lea %s,%s\n", dest, src);
  writeLine(buffer);
}

void asm_xchg(const char *arg1, const char *arg2) {}

void asm_and(const char *arg1, const char *arg2) {}

void asm_or(const char *arg1, const char *arg2) {}

void asm_xor(const char *arg1, const char *arg2) {}

void asm_cmp(const char *arg1, const char *arg2) {
  char buffer[32];
  sprintf(buffer, "cmp %s,%s\n", arg1, arg2);
  writeLine(buffer);
}

void asm_jmp(const char *label) {
  char buffer[32];
  sprintf(buffer, "jmp %s\n", label);
  writeLine(buffer);
}

void asm_jmp_g(const char *label) {
  char buffer[32];
  sprintf(buffer, "jmp %s\n", label);
  writeLine(buffer);
}

void asm_jmp_ge(const char *label) {
  char buffer[32];
  sprintf(buffer, "jge %s\n", label);
  writeLine(buffer);
}

void asm_jmp_l(const char *label) {
  char buffer[32];
  sprintf(buffer, "jl %s\n", label);
  writeLine(buffer);
}

void asm_jmp_le(const char *label) {
  char buffer[32];
  sprintf(buffer, "jle %s\n", label);
  writeLine(buffer);
}

void asm_jmp_eq(const char *label) {
  char buffer[32];
  sprintf(buffer, "je %s\n", label);
  writeLine(buffer);
}

void asm_jmp_neq(const char *label) {
  char buffer[32];
  sprintf(buffer, "jne %s\n", label);
  writeLine(buffer);
}

void asm_label(const char *name) {
  char buffer[32];
  sprintf(buffer, "%s:\n", name);
  writeLine(buffer);
}

void asm_call(const char *name) {
  char buffer[32];
  sprintf(buffer, "call _%s\n", name);
  writeLine(buffer);
}

void asm_ret() {
  char buffer[32];
  sprintf(buffer, "ret\n");
  writeLine(buffer);
}

void asm_print_mat(const char *src, int nl) {}

void asm_print_const_mat(float *arr, int num_elements, int nl) {}

void asm_print_float(const char *src, int nl) {
  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_float_nl, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_float, %s\n", src);
  writeLine(buffer);
}

void asm_print_const_float(float f, int nl) {}

void asm_print_int(const char *src, int nl) {
  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_int_nl, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_int, %s\n", src);
  writeLine(buffer);
}

void asm_print_const_int(int i, int nl) {}

void asm_print_char(const char *src, int nl) {
  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_char_nl, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_char, %s\n", src);
  writeLine(buffer);
}

void asm_print_const_char(char c, int nl) {}
