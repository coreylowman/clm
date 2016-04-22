#ifndef CLM_ASM_H_
#define CLM_ASM_H_

static const char ASM_HEADER[] =
    "format PE console\n"
    "entry start\n"
    "\n"
    "include 'win32a.inc'\n"
    "include 'macro/import32.inc'\n"
    "\n"
    "section '.rdata' data readable\n"
    "        print_int db '%%d',0\n"
    "        print_int_spc db '%%d',32,0\n"
    "        print_int_nl db '%%d',10,0\n"
    "        print_float db '%%f',0\n"
    "        print_float_spc db '%%f',32,0\n"
    "        print_float_nl db '%%f',10,0\n"
    "        print_char db '%%c',13,0\n"
    "        print_char_spc db '%%c',13,32,0\n"
    "        print_char_nl db '%%c',13,10,0\n"
    "\n"
    "section '.idata' data readable import\n"
    "        library kernel32, 'kernel32.dll', \\\n"
    "                msvcrt,   'msvcrt.dll'\n"
    "        import kernel32, ExitProcess, 'ExitProcess'\n"
    "        import msvcrt, printf, 'printf'\n"
    "\n"
    "section '.code' code executable\n";

static const char ASM_EXIT_PROCESS[] = "invoke ExitProcess, 0\n";
static const char ASM_START[] = "start:\n";
static const char ASM_DATA[] = "section '.data' data readable writable\n"
                               "__T_EAX__ dd 0\n"
                               "__T_EBX__ dd 0\n"
                               "__T_END__ dd 0\n"
                               "__T_ROW_END__ dd 0\n"
                               "__T_ESP__ dd 0\n";

// general 32 bit registers
#define EAX "eax"
#define EBX "ebx"
#define ECX "ecx"
#define EDX "edx"
#define ESP "esp"
#define EBP "ebp"

// FPU registers
#define ST0 "st0"
#define ST1 "st1"
#define ST2 "st2"
#define ST3 "st3"
#define ST4 "st4"
#define ST5 "st5"
#define ST6 "st6"
#define ST7 "st7"

// compiler only globals to give more temporary
#define T_EAX "__T_EAX__"
#define T_EBX "__T_EBX__"
#define T_END "__T_END__"
#define T_ROW_END "__T_ROW_END__"
#define T_ESP "__T_ESP__"

#define LABEL_SIZE 32

void pop_int_into(const char *dest);
void pop_matrix_of_size(const char *size_location);

void asm_comment(const char *line);
void asm_pop(const char *dest);
void asm_push(const char *src);
void asm_pop_f(const char *dest);
void asm_push_f(const char *src);
void asm_push_const_i(int val);
void asm_push_const_f(float val);
void asm_push_const_c(char val);
void asm_add(const char *dest, const char *other);
void asm_add_i(const char *dest, int i);
void asm_sub(const char *dest, const char *other);
void asm_imul(const char *dest, const char *other);
void asm_div(const char *denom);
void asm_fadd(const char *dest, const char *other);
void asm_fsub(const char *dest, const char *other);
void asm_fmul(const char *dest, const char *other);
void asm_fdiv(const char *dest, const char *other);
void asm_inc(const char *arg);
void asm_dec(const char *arg);
void asm_neg(const char *arg);
void asm_mov(const char *dest, const char *src);
void asm_mov_i(const char *dest, int i);
void asm_lea(const char *dest, const char *src);
void asm_xchg(const char *arg1, const char *arg2);
void asm_and(const char *arg1, const char *arg2);
void asm_or(const char *arg1, const char *arg2);
void asm_xor(const char *arg1, const char *arg2);
void asm_cmp(const char *arg1, const char *arg2);
void asm_jmp(const char *label);
void asm_jmp_g(const char *label);
void asm_jmp_ge(const char *label);
void asm_jmp_l(const char *label);
void asm_jmp_le(const char *label);
void asm_jmp_eq(const char *label);
void asm_jmp_neq(const char *label);
void asm_label(const char *name);
void asm_call(const char *name);
void asm_ret();
// TODO improve this interface
void asm_print_float(const char *src, int spc, int nl);
void asm_print_int(const char *src, int spc, int nl);
void asm_print_char(const char *src, int spc, int nl);
void asm_push_regs();
void asm_pop_regs();

#endif
