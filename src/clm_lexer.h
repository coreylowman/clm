#ifndef CLM_LEXER_H_
#define CLM_LEXER_H_

#include "array_list.h"

// TODO add commas after each one
#define token_list                                                             \
  token(ADD, "+") token(AND, "and") token(ASSIGN, "=") token(BACKSLASH, "\\")  \
      token(BY, "by") token(CALL, "call") token(COLON, ":") token(             \
          COMMA, ",") token(DIV, "/") token(DO, "do") token(ELSE, "else")      \
          token(END, "end") token(EQ, "==") token(FLOAT, "float") token(       \
              FLOAT_WORD, "float_word") token(FOR, "for") token(GT, ">")       \
              token(GTE, ">=") token(ID, "id") token(IF, "if") token(          \
                  INT, "int") token(INT_WORD, "int_word") token(LBRACK, "[")   \
                  token(LCURL, "{") token(LPAREN, "(") token(LT, "<")          \
                      token(LTE, "<=") token(MULT, "*") token(NEQ, "!=")       \
                          token(NOT, "not") token(OR, "or") token(PERIOD, ".") \
                              token(PRINT, "print") token(PRINTL, "printl")    \
                                  token(RBRACK, "]") token(RCURL, "}") token(  \
                                      RETURN, "return") token(RPAREN, ")")     \
                                      token(SEMI, ";") token(STRING, "string") \
                                          token(STRING_WORD, "string_word")    \
                                              token(SUB, "-")                  \
                                                  token(THEN, "then")          \
                                                      token(TILDA, "~")        \
                                                          token(TO, "to")

typedef enum ClmLexerSymbol {
// TODO remove comma after this
#define token(tok, str) LEX_##tok,
  token_list
#undef token
} ClmLexerSymbol;

static char *clmLexerSymbolStrings[] = {
// TODO remove comma after this
#define token(tok, str) str,
    token_list
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
