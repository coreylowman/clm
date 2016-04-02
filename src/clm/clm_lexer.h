#ifndef CLM_LEXER_H_
#define CLM_LEXER_H_

#include "util/array_list.h"

typedef enum ClmLexerSymbol {
  LEX_ADD,
  LEX_AND,
  LEX_ASSIGN,
  LEX_AT,
  LEX_BACKSLASH,
  LEX_BY,
  LEX_CALL,
  LEX_COLON,
  LEX_COMMA,
  LEX_DIV,
  LEX_DO,
  LEX_ELSE,
  LEX_END,
  LEX_EQ,
  LEX_FLOAT,
  LEX_FLOAT_WORD,
  LEX_FOR,
  LEX_GT,
  LEX_GTE,
  LEX_ID,
  LEX_IF,
  LEX_INT,
  LEX_INT_WORD,
  LEX_LBRACK,
  LEX_LCURL,
  LEX_LPAREN,
  LEX_LT,
  LEX_LTE,
  LEX_MULT,
  LEX_NEQ,
  LEX_NOT,
  LEX_OR,
  LEX_PERIOD,
  LEX_PRINT,
  LEX_PRINTL,
  LEX_RBRACK,
  LEX_RCURL,
  LEX_RETURN,
  LEX_RPAREN,
  LEX_SEMI,
  LEX_STRING,
  LEX_STRING_WORD,
  LEX_SUB,
  LEX_THEN,
  LEX_TILDA,
  LEX_TO
} ClmLexerSymbol;

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
