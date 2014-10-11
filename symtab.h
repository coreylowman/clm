#ifndef SYM_H
#define SYM_H

typedef struct sym_entry{
	char *name;
	int isFunc;
	struct sym_tab *symbol_table;
	int rows;
	char *rname;
	int cols;
	char *cname;
}SYMENTRY;

typedef struct sym_tab{
	int numSyms;
	SYMENTRY **data;
	struct sym_tab *parent;
}SYMTAB;

SYMTAB *appendSym(SYMTAB *list,SYMENTRY *data);
SYMTAB *copySymTab(SYMTAB *list);
SYMENTRY *copySymEntry(SYMENTRY *sentry);
SYMENTRY *findSym(SYMTAB *list,char *name);
SYMENTRY *makeVarEntry(char *name,int rows,int cols);
SYMENTRY *makeFuncEntry(char *name,SYMTAB *symbol_table,int rows,int cols);
void free_symtab(SYMTAB *sym);
void free_symentry(SYMENTRY *sym);

#endif