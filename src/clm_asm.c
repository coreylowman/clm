#include <stdio.h>

#include "clm_asm.h"

extern void writeLine(const char *line);

void pop_int_into(const char *dest) {
  // pop type
  asm_pop(dest);

  // overwrite type with int value
  asm_pop(dest);
}

// size_location is the location of the numbber of elements the matrix has
// this assumes esp is pointing like so
//
// vals
// ...
// cols
// rows
// type
// <- esp
void pop_matrix_of_size(const char *size_location) {
  asm_mov(EAX, size_location); // eax contains number of elements
  asm_imul(EAX, "4");
  asm_add(EAX, "16");
  asm_sub(ESP, EAX); // esp -= (num elements * 4 + 16)
}

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

void asm_pop_f(const char *dest){
  char buffer[32];
  sprintf(buffer, "fstp %s\n", dest);
  writeLine(buffer);
}

void asm_push_f(const char *src){
  char buffer[32];
  sprintf(buffer, "fld %s\n", src);
  writeLine(buffer);
}

void asm_push_const_i(int val) {
  char buffer[32];
  sprintf(buffer, "push %d\n", val);
  writeLine(buffer);
}

void asm_push_const_f(float val) {
  char buffer[32];
  sprintf(buffer, "fld %f\n", val);
  writeLine(buffer);
}

void asm_push_const_c(char val) {
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

void asm_div(const char *denom) {
  char buffer[32];
  sprintf(buffer, "div %s\n", denom);
  writeLine(buffer);
}

/*
  from fasm doc:

  "fadd adds the destination and source operand and stores the sum in the destination location.
  The destination operand is always an FPU register, if the source is a memory location,
  the destination is ST0 register and only source operand should be specified.
  If both operands are FPU registers, at least one of them should be ST0 register.
  An operand in memory can be a 32-bit or 64-bit value."
*/
void asm_fadd(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "fadd %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_fsub(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "fsub %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_fmul(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "fmul %s,%s\n", dest, other);
  writeLine(buffer);
}

void asm_fdiv(const char *dest, const char *other) {
  char buffer[32];
  sprintf(buffer, "fdiv %s,%s\n", dest, other);
  writeLine(buffer);
}

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

void asm_xchg(const char *arg1, const char *arg2) {
  char buffer[32];
  sprintf(buffer, "xchg %s,%s\n", arg1, arg2);
  writeLine(buffer);
}

void asm_and(const char *arg1, const char *arg2) {
  char buffer[32];
  sprintf(buffer, "and %s,%s\n", arg1, arg2);
  writeLine(buffer);
}

void asm_or(const char *arg1, const char *arg2) {
  char buffer[32];
  sprintf(buffer, "or %s,%s\n", arg1, arg2);
  writeLine(buffer);
}

void asm_xor(const char *arg1, const char *arg2) {
  char buffer[32];
  sprintf(buffer, "xor %s,%s\n", arg1, arg2);
  writeLine(buffer);
}

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

void asm_ret() { writeLine("ret\n"); }

void asm_print_float(const char *src, int spc, int nl) {
  asm_push_regs();

  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_float_nl, %s\n", src);
  else if (spc)
    sprintf(buffer, "cinvoke printf, print_float_spc, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_float, %s\n", src);
  writeLine(buffer);

  asm_pop_regs();
}

void asm_print_int(const char *src, int spc, int nl) {
  asm_push_regs();

  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_int_nl, %s\n", src);
  else if (spc)
    sprintf(buffer, "cinvoke printf, print_int_spc, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_int, %s\n", src);
  writeLine(buffer);

  asm_pop_regs();
}

void asm_print_char(const char *src, int spc, int nl) {
  asm_push_regs();

  char buffer[64];
  if (nl)
    sprintf(buffer, "cinvoke printf, print_char_nl, %s\n", src);
  else if (spc)
    sprintf(buffer, "cinvoke printf, print_char_spc, %s\n", src);
  else
    sprintf(buffer, "cinvoke printf, print_char, %s\n", src);
  writeLine(buffer);

  asm_pop_regs();
}

void asm_push_regs() { writeLine("pushad\n"); }

void asm_pop_regs() { writeLine("popad\n"); }
