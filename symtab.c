#include <string.h>
#include <stdlib.h>
#include "symtab.h"

SYMTAB *appendSym(SYMTAB *list,SYMENTRY *data){
	if(list == NULL){
		list = (SYMTAB *)malloc(sizeof(SYMTAB));
		list->numSyms = 0;
		list->data = NULL;
		list->parent = NULL;
	}
	list->data = realloc(list->data,sizeof(SYMENTRY)*(list->numSyms + 1));
	list->data[list->numSyms] = data;
	list->numSyms++;
	return list;
}

SYMTAB *copySymTab(SYMTAB *list){
	int i = 0;
	if (list == NULL) return NULL;
	SYMTAB *newSym = (SYMTAB *)malloc(sizeof(SYMTAB));
	newSym->numSyms = list->numSyms;
	newSym->parent = list->parent;
	newSym->data = malloc(sizeof(SYMENTRY) * (newSym->numSyms));
	for (i = 0; i < list->numSyms; i++){
		newSym->data[i] = copySymEntry(list->data[i]);
	}
	return newSym;
}

SYMENTRY *copySymEntry(SYMENTRY *sentry){
	if (sentry == NULL) return NULL;
	SYMENTRY *newSym = (SYMENTRY *)malloc(sizeof(SYMENTRY));
	newSym->rows = sentry->rows;
	newSym->cols = sentry->cols;
	newSym->isFunc = sentry->isFunc;
	newSym->symbol_table = NULL;
	if (sentry->isFunc){
		newSym->symbol_table = copySymTab(sentry->symbol_table);
	}
	newSym->name = (char *)malloc(strlen(sentry->name) + 1);
	strcpy(newSym->name, sentry->name);
	return newSym;
}

SYMENTRY *findSym(SYMTAB *list,char *name){
	int i;
	
	if (list == NULL) return NULL;

	for(i = 0;i < list->numSyms;i++){
		if(!strcmp(list->data[i]->name,name)){
			return list->data[i];
		}
	}
	if(list->parent == NULL)
		return NULL;
	else
		return findSym(list->parent,name);
}

SYMENTRY *makeVarEntry(char *name,int rows,int cols){
	SYMENTRY *sym = NULL;
	sym = (SYMENTRY *)malloc(sizeof(SYMENTRY));
	sym->name = (char *)malloc(strlen(name) + 1);
	strcpy(sym->name,name);
	sym->isFunc = 0;
	sym->symbol_table = NULL;
	sym->rows = rows;
	sym->rname = NULL;
	sym->cols = cols;
	sym->cname = NULL;
	return sym;
}

SYMENTRY *makeFuncEntry(char *name,SYMTAB *symbol_table,int rows,int cols){
	SYMENTRY *sym = NULL;
	sym = (SYMENTRY *)malloc(sizeof(SYMENTRY));
	sym->name = (char *)malloc(strlen(name) + 1);
	strcpy(sym->name,name);
	sym->isFunc = 1;
	sym->symbol_table = symbol_table;
	sym->rows = rows;
	sym->cols = cols;
	return sym;
}


void free_symtab(SYMTAB *sym){	
	int i;
	if (sym == NULL) return;
	sym->parent = NULL;
	for (i = sym->numSyms - 1; i >= 0; i--)
		free_symentry(sym->data[i]);
	free(sym->data);
	free(sym);
}
void free_symentry(SYMENTRY *sym){
	if (sym == NULL) return;
	if (sym->isFunc){
		free_symtab(sym->symbol_table);
	}
	free(sym->name);
	free(sym);
}