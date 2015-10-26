#ifndef CLM_ASM_HEADERS_H_
#define CLM_ASM_HEADERS_H_

static const char ASM_HEADER[] = "format PE console\n"
"entry start\n"
"\n"
"include 'win32a.inc'\n"
"include 'macro/import32.inc'\n"
"\n"
"section '.rdata' data readable\n"
"        print_int db '%d',32,0\n"
"        print_int_nl db '%d',32,10,0\n"
"        print_char db '%c',13,10,0\n"
"\n"
"section '.idata' data readable import\n"
"        library kernel32, 'kernel32.dll', \n"
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
"    imul ecx,4 ;ecx to keep track of when to print newline... this is rows * 4\n"
"    mov ebx, [m1+4]\n"
"    imul ebx, [m1+8] ;ebx to keep track of how many more elements to print...\n"
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
"    cinvoke printf,print_int,[m1 + eax + 12] ;eax and ecx invalidated here\n"
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

#endif