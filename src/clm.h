#ifndef CLM_H_
#define CLM_H_

#include <stdarg.h>
#include <stdlib.h>

//
// Forward Declarations
//
typedef struct ClmScope ClmScope;

//
// Errors (note: not implemented in clm.c - implemented in the corresponding main.c)
//
void clm_error(int line, int col, const char *fmt, ...);

//
// String
//
char *string_copy(const char *string);
char *string_copy_n(const char *string, size_t n);
int string_equals(const char *string1, const char *string2);
int string_equals_n(const char *string1, const char *string2, size_t n);

//
// ArrayList
//
typedef struct ArrayList {
  void **data;
  void (*free_element)(void *element);
  int capacity;
  int length;
} ArrayList;

ArrayList *array_list_new(void (*free_element)(void *element));
void array_list_free(void *data);

void array_list_push(ArrayList *self, void *data);

void array_list_foreach(ArrayList *self, void (*func)(void *data));
void array_list_foreach_2(ArrayList *self, int level,
                          void (*func)(void *data, int l));

//
// Lexer Structs
//
typedef enum ClmLexerSymbol {
#define keyword(tok, str) tok,
#define literal(tok, str) tok,
#define token(tok, str) tok,
#include "keywords.inc"
#undef keyword
#undef literal
#undef token
} ClmLexerSymbol;

static char *clmLexerSymbolStrings[] = {
#define keyword(tok, str) str,
#define literal(tok, str) str,
#define token(tok, str) str,
#include "keywords.inc"
#undef keyword
#undef literal
#undef token
};

typedef struct ClmLexerToken {
  ClmLexerSymbol sym;
  char *raw;
  int lineNo;
  int colNo;
} ClmLexerToken;

//
// Pretty Printing
//
void clm_print_tokens(ArrayList *tokens);
void clm_print_statements(ArrayList *statements);

//
// Main functions for each module
//
ArrayList *clm_lexer_main(const char *fileContents);
ArrayList *clm_parser_main(ArrayList *tokens);
ClmScope *clm_symbol_gen_main(ArrayList *statements);
void clm_type_check_main(ArrayList *statements, ClmScope *globalScope);
void clm_optimizer_main(ArrayList *statements, ClmScope *globalScope);
const char *clm_code_gen_main(ArrayList *statements, ClmScope *globalScope);

#endif
