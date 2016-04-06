#ifndef CLM_LEXER_H_
#define CLM_LEXER_H_

#include "array_list.h"

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

ArrayList *clm_lexer_main(const char *fileContents);
void clm_lexer_print(ArrayList *data);
const char *clm_lexer_sym_to_string(ClmLexerSymbol s);

#endif
