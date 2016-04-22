#include <stdarg.h>
#include <stdio.h>

#include "clm_asm.h"

#define ASM_WRITE(format, args...) \
  char buffer[32]; \
  sprintf(buffer, format, args); \
  writeLine(buffer);

extern void writeLine(const char *line);

void pop_int_into(const char *dest) {
  // pop type
  asm_pop(dest);

  // overwrite type with int value
  asm_pop(dest);
}

void pop_float_into(const char *dest) {
  // pop type off of general stack
  asm_pop(dest);

  // pop value off of fpu stack
  asm_pop_f(dest);
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
  ASM_WRITE("pop %s\n", dest);
}

void asm_push(const char *src) {
  ASM_WRITE("push %s\n", src);
}

/*
  from fasm docs:

  "fst copies the value of ST0 register to the destination operand,
  which can be 32-bit or 64-bit memory location or another FPU register.
  fstp performs the same operation as fst and then pops the register stack,
  getting rid of ST0. fstp accepts the same operands as the fst instruction and
  can also store value in the 80-bit memory.""
*/
void asm_pop_f(const char *dest) {
  ASM_WRITE("fstp %s\n", dest);
}

void asm_push_f(const char *src) {
  ASM_WRITE("fld %s\n", src);
}

void asm_push_const_i(int val) {
  ASM_WRITE("push %d\n", val);
}

void asm_push_const_f(float val) {
  ASM_WRITE("fld %f\n", val);
}

void asm_push_const_c(char val) {
  ASM_WRITE("push %c\n", val);
}

void asm_add(const char *dest, const char *other) {
  ASM_WRITE("add %s,%s\n", dest, other);
}

void asm_add_i(const char *dest, int i) {
  ASM_WRITE("add %s,%d\n", dest, i);
}

void asm_sub(const char *dest, const char *other) {
  ASM_WRITE("sub %s,%s\n", dest, other);
}

void asm_imul(const char *dest, const char *other) {
  ASM_WRITE("imul %s,%s\n", dest, other);
}

void asm_div(const char *denom) {
  ASM_WRITE("div %s\n", denom);
}

void asm_fxch(const char *arg1, const char *arg2){
  ASM_WRITE("fxch %s,%s\n", arg1, arg2);
}

void asm_fild(const char *src){
  ASM_WRITE("fild %s\n", src);
}

void asm_fiadd(const char *dest, const char *other){
  ASM_WRITE("fiadd %s,%s\n", dest, other);
}

void asm_fisub(const char *dest, const char *other){
  ASM_WRITE("fisub %s,%s\n", dest, other);
}

void asm_fimul(const char *dest, const char *other){
  ASM_WRITE("fimul %s,%s\n", dest, other);
}

void asm_fidiv(const char *dest, const char *other){
  ASM_WRITE("fidiv %s,%s\n", dest, other);
}

/*
  from fasm doc:

  "fadd adds the destination and source operand and stores the sum in the
  destination location. The destination operand is always an FPU register, if
  the source is a memory
  location, the destination is ST0 register and only source operand should be
  specified.
  If both operands are FPU registers, at least one of them should be ST0
  register. An operand in memory can be a 32-bit or 64-bit value."
*/
void asm_fadd(const char *dest, const char *other) {
  ASM_WRITE("fadd %s,%s\n", dest, other);
}

void asm_fsub(const char *dest, const char *other) {
  ASM_WRITE("fsub %s,%s\n", dest, other);
}

void asm_fmul(const char *dest, const char *other) {
  ASM_WRITE("fmul %s,%s\n", dest, other);
}

void asm_fdiv(const char *dest, const char *other) {
  ASM_WRITE("fdiv %s,%s\n", dest, other);
}

void asm_faddp(const char *dest, const char *other){
  ASM_WRITE("faddp %s,%s\n", dest, other);
}

void asm_fsubp(const char *dest, const char *other){
  ASM_WRITE("fsubp %s,%s\n", dest, other);
}

void asm_fmulp(const char *dest, const char *other){
  ASM_WRITE("fmulp %s,%s\n", dest, other);
}

void asm_fdivp(const char *dest, const char *other){
  ASM_WRITE("fdivp %s,%s\n", dest, other);
}

void asm_faddr(const char *dest, const char *other){
  ASM_WRITE("faddr %s,%s\n", dest, other);
}

void asm_fsubr(const char *dest, const char *other){
  ASM_WRITE("fsubr %s,%s\n", dest, other);
}

void asm_fmulr(const char *dest, const char *other){
  ASM_WRITE("fmulr %s,%s\n", dest, other);
}

void asm_fdivr(const char *dest, const char *other){
  ASM_WRITE("fdivr %s,%s\n", dest, other);
}

void asm_faddrp(const char *dest, const char *other){
  ASM_WRITE("faddpr %s,%s\n", dest, other);
}

void asm_fsubrp(const char *dest, const char *other){
  ASM_WRITE("fsubpr %s,%s\n", dest, other);
}

void asm_fmulrp(const char *dest, const char *other){
  ASM_WRITE("fmulpr %s,%s\n", dest, other);
}

void asm_fdivrp(const char *dest, const char *other){
  ASM_WRITE("fdivpr %s,%s\n", dest, other);
}


void asm_inc(const char *arg) {
  ASM_WRITE("inc %s\n", arg);
}

void asm_dec(const char *arg) {
  ASM_WRITE("dec %s\n", arg);
}

void asm_neg(const char *arg) {
  ASM_WRITE("dec %s\n", arg);
}

void asm_mov(const char *dest, const char *src) {
  ASM_WRITE("mov %s,%s\n", dest, src);
}

void asm_mov_i(const char *dest, int i) {
  ASM_WRITE("mov %s,%d\n", dest, i);
}

void asm_lea(const char *dest, const char *src) {
  ASM_WRITE("lea %s,%s\n", dest, src);
}

void asm_xchg(const char *arg1, const char *arg2) {
  ASM_WRITE("xchg %s,%s\n", arg1, arg2);
}

void asm_and(const char *arg1, const char *arg2) {
  ASM_WRITE("and %s,%s\n", arg1, arg2);
}

void asm_or(const char *arg1, const char *arg2) {
  ASM_WRITE("or %s,%s\n", arg1, arg2);
}

void asm_xor(const char *arg1, const char *arg2) {
  ASM_WRITE("xor %s,%s\n", arg1, arg2);
}

void asm_cmp(const char *arg1, const char *arg2) {
  ASM_WRITE("cmp %s,%s\n", arg1, arg2);
}

void asm_jmp(const char *label) {
  ASM_WRITE("jmp %s\n", label);
}

void asm_jmp_g(const char *label) {
  ASM_WRITE("jmp %s\n", label);
}

void asm_jmp_ge(const char *label) {
  ASM_WRITE("jge %s\n", label);
}

void asm_jmp_l(const char *label) {
  ASM_WRITE("jl %s\n", label);
}

void asm_jmp_le(const char *label) {
  ASM_WRITE("jle %s\n", label);
}

void asm_jmp_eq(const char *label) {
  ASM_WRITE("je %s\n", label);
}

void asm_jmp_neq(const char *label) {
  ASM_WRITE("jne %s\n", label);
}

void asm_label(const char *name) {
  ASM_WRITE("%s:\n", name);
}

void asm_call(const char *name) {
  ASM_WRITE("call _%s\n", name);
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
