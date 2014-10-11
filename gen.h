#ifndef GEN_H
#define GEN_H

#include "ast.h"
#include "symtab.h"

char *appendLine(char *code,char *line);
char *genINDEXNODE(char *code,INDEXNODE *node,SYMTAB *syms);
char *genEXPNODE(char *code, EXPNODE *node, SYMTAB *syms);
char *genASTNODE(char *code, ASTNODE *node, SYMTAB *syms);
char *genAST(char *code, AST *ast, SYMTAB *syms);
char *genVars(char *decs, SYMTAB *syms);
void gen_main(char *filename, AST *ast, SYMTAB *syms);

static const char ASM_HEADER[] = "format PE console\n\
entry start\n\
\n\
include 'win32a.inc'\n\
include 'macro/import32.inc'\n\
\n\
struct MATRIX\n\
	rows dd ?\n\
	cols dd ?\n\
	data dd 256 dup (?)\n\
ends\n\
\n\
section '.rdata' data readable\n\
		print_int db '%d',32,0\n\
		print_int_nl db '%d',32,10,0\n\
		print_char db '%c',13,10,0\n\
\n\
section '.idata' data readable import\n\
        library kernel32, 'kernel32.dll', \\\n\
                msvcrt,   'msvcrt.dll'\n\
        import kernel32, ExitProcess, 'ExitProcess'\n\
        import msvcrt, printf, 'printf'\n\
\n\
section '.code' code executable\n\
start:\n";

static const char ASM_FOOTER[] = "invoke ExitProcess,0\n\n\
\n\
section '.data' data readable writable\n\
__index1 dd 0\n\
__index2 dd 0\n";

#endif