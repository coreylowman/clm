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
    "section '.code' code executable\n"
    "; m1 and m2 are addresses to the matrix\n"
    "; cols <- m1 + 8\n"
    "; rows <- m1 + 4\n"
    "; type <- m1\n"
    "macro add_mat m1, m2 {\n"
    "    local loop1, loop2\n"
    "    mov eax, [m1 + 4]\n"
    "    imul eax, [m1 + 8] \n"
    "    dec eax \n"
    "    imul eax, 4 ;eax = 4*(rows * cols - 1)\n"
    "loop1:\n"
    "    cmp eax, -4\n"
    "    je loop2\n"
    "\n"
    "    mov ebx, [m1 + eax + 12]\n"
    "    add ebx, [m2 + eax + 12]\n"
    "    push ebx\n"
    "\n"
    "    sub eax, 4\n"
    "    jmp loop1\n"
    "loop2:\n"
    "    push [m1+8] ;push cols\n"
    "    push [m1+4] ;push rows\n"
    "    push [m1] ;push type\n"
    "}\n"
    "\n"
    "macro print_mat m1 {\n"
    "    local loop1, loop2, loop3\n"
    "    mov ecx, [m1+4]\n"
    "    imul ecx,4 ;ecx to keep track of when to print newline... this is "
    "rows * 4\n"
    "    mov ebx, [m1+4]\n"
    "    imul ebx, [m1+8] ;ebx to keep track of how many more elements to "
    "print...\n"
    "    imul ebx, 4 ; ebx is 4 * rows * cols\n"
    "    mov eax, -4 ; eax is current index\n"
    "loop1:\n"
    "    add eax,4\n"
    "    cmp eax, ebx\n"
    "    je loop2\n"
    "    \n"
    "    push eax ;save eax and ecx on stack\n"
    "    push ecx\n"
    "\n"
    "    mov edx,0 ;if eax % rows*4 == 0 print new line\n"
    "    div ecx\n"
    "    cmp edx,0\n"
    "    jne loop3\n"
    "    cmp eax,0\n"
    "    je loop3\n"
    "    cinvoke printf,print_char,'' ;eax and ecx invalidated here\n"
    "\n"
    "loop3:\n"
    "    pop ecx ;restore eax and ecx\n"
    "    pop eax\n"
    "\n"
    "    push eax ;savee eax and ecx on stack\n"
    "    push ecx\n"
    "\n"
    "    cinvoke printf,print_int,[m1 + eax + 12] ;eax and ecx invalidated "
    "here\n"
    "    \n"
    "    pop ecx ;restore eax and ecx\n"
    "    pop eax\n"
    "\n"
    "    jmp loop1\n"
    "loop2:\n"
    "    cinvoke printf,print_char,''\n"
    "}\n";

static const char ASM_EXIT_PROCESS[] = "invoke ExitProcess, 0\n";
static const char ASM_START[] = "start:\n";
static const char ASM_DATA[] = "section '.data' data readable writable\n"
                               "T_EAX dd 0\n"
                               "T_EBX dd 0\n";

void asm_comment(const char *line);
void asm_pop(const char *dest);
void asm_push(const char *src);
void asm_push_i(int val);
void asm_push_f(float val);
void asm_push_c(char val);
void asm_add(const char *dest, const char *other);
void asm_add_i(const char *dest, int i);
void asm_sub(const char *dest, const char *other);
void asm_imul(const char *dest, const char *other);
void asm_fadd(const char *dest, const char *other);
void asm_fsub(const char *dest, const char *other);
void asm_fmul(const char *dest, const char *other);
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
void asm_print_mat(const char *src, int nl);
void asm_print_const_mat(float *arr, int num_elements, int nl);
void asm_print_float(const char *src, int nl);
void asm_print_const_float(float f, int nl);
void asm_print_int(const char *src, int nl);
void asm_print_const_int(int i, int nl);
void asm_print_char(const char *src, int nl);
void asm_print_const_char(char c, int nl);

#endif
