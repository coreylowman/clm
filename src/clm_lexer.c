#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clm.h"

typedef struct ClmLexerData {
  const char *programString;
  int programLength;
  int curInd;
  int lineNo;
  int colNo;
  int numTokens;
  ArrayList *tokens; // array list of ClmLexerToken
} ClmLexerData;

#define tok_str_eq(x, s) string_equals_n((x), (s), strlen(s))

static ClmLexerData data;

static ClmLexerToken *get_token();

static int is_pd(char c) { return c == '.'; }

static int is_dig(char c) { return c >= '0' && c <= '9'; }

static int is_letter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_whitespace(char c) {
  return c == '\t' || c == '\n' || c == '\r' || c == ' ';
}

static int is_quote(char c) { return c == '"'; }

static int is_id_char(char c) { return is_letter(c) || is_dig(c) || c == '_'; }

static char consume() { return data.programString[data.curInd++]; }

static char next() { return data.programString[data.curInd + 1]; }

static char curr() { return data.programString[data.curInd]; }

static char prev() { return data.programString[data.curInd - 1]; }

static void reverse() {
  if (data.curInd > 0)
    data.curInd--;
}

static int valid() { return data.curInd < data.programLength; }

static void lexer_token_free(void *data) {
  if (data == NULL)
    return;
  ClmLexerToken *token = (ClmLexerToken *)data;
  free(token->raw);
  free(token);
}

static void lexer_token_print(void *data) {
  ClmLexerToken *token = (ClmLexerToken *)data;
  printf("%s : { %s }\n", clmLexerSymbolStrings[token->sym], token->raw);
}

void clm_print_tokens(ArrayList *tokens) {
  array_list_foreach(tokens, lexer_token_print);
}

ArrayList *clm_lexer_main(const char *file_contents) {
  data.curInd = 0;
  data.lineNo = 1;
  data.colNo = 0;
  data.numTokens = 0;
  data.programString = file_contents;
  data.tokens = array_list_new(lexer_token_free);

  data.programLength = strlen(file_contents);

  while (valid() && data.programString[data.curInd] != '\0') {
    ClmLexerToken *token = get_token();
    if (token != NULL)
      array_list_push(data.tokens, token);
  }

  ClmLexerToken *end = malloc(sizeof(*end));
  end->sym = KEYWORD_END;
  end->raw = string_copy("end");
  end->lineNo = 0;
  end->colNo = 0;
  array_list_push(data.tokens, end);

  return data.tokens;
}

// all numbers will start with a digit, and may optionally contain a period
static ClmLexerToken *read_number() {
  char c;
  int num_pds = 0;
  int start = data.curInd;

  c = curr();

  if (!is_dig(c)) {
    return NULL;
  }

  while (valid() && (is_dig(c = curr()) || is_pd(c))) {
    num_pds += is_pd(c);
    consume();
  }

  if (num_pds > 1) {
    clm_error(data.lineNo, data.colNo,
              "found multiple '.' in number declaration");
    return NULL;
  }

  ClmLexerToken *token = malloc(sizeof(*token));
  token->lineNo = data.lineNo;
  token->colNo = data.colNo;
  token->raw = string_copy_n(data.programString + start, data.curInd - start);

  if (num_pds > 0)
    token->sym = LITERAL_FLOAT;
  else
    token->sym = LITERAL_INT;

  return token;
}

// all words must start with [a-zA-Z]
// a word can either be a keyword or an identifier
static ClmLexerToken *read_word() {
  char c;
  int num_id_chars = 0;
  int start = data.curInd;

  c = curr();

  if (!is_letter(c)) {
    return NULL;
  }

  while (valid() && is_id_char(c = curr())) {
    num_id_chars += (c == '_' || is_dig(c));
    consume();
  }

  if (num_id_chars > 0) {
    ClmLexerToken *token = malloc(sizeof(*token));
    token->lineNo = data.lineNo;
    token->colNo = data.colNo;
    token->raw = string_copy_n(data.programString + start, data.curInd - start);
    token->sym = LITERAL_ID;
    return token;
  }

  ClmLexerSymbol sym;
  char word_buffer[32];
  strncpy(word_buffer, data.programString + start, data.curInd - start);
  word_buffer[data.curInd - start] = '\0';

  if (0) {
    // include if branch just for parsing correctly
    // all the keywords will map to else ifs
  }
#define literal(tok, str)
#define token(tok, str)
#define keyword(tok, str)                                                      \
  else if (strcmp(word_buffer, str) == 0) {                                    \
    sym = tok;                                                                 \
  }
#include "keywords.inc"
#undef literal
#undef token
#undef keyword
  else {
    // the word in word_buffer didn't match any keywords... its an id
    sym = LITERAL_ID;
  }

  ClmLexerToken *token = malloc(sizeof(*token));
  token->lineNo = data.lineNo;
  token->colNo = data.colNo;
  token->raw = string_copy_n(data.programString + start, data.curInd - start);
  token->sym = sym;
  return token;
}

static ClmLexerToken *read_string_literal() {
  char c;
  int start = data.curInd;

  c = curr();

  if (c != '"') {
    return NULL;
  }

  while (valid() && (c = curr()) != '"') {
    if (c == '\n' || c == '\r') {
      data.lineNo++;
      data.colNo = 0;
    }
    if (c == '\\' && next() == '"') {
      consume();
    }
    consume();
  }

  // eat the last quote
  consume();

  ClmLexerToken *token = malloc(sizeof(*token));
  token->lineNo = data.lineNo;
  token->colNo = data.colNo;
  token->raw = string_copy_n(data.programString + start, data.curInd - start);
  token->sym = LITERAL_STRING;
  return token;
}

static ClmLexerToken *get_token() {
  char c;
  ClmLexerSymbol sym;

  while (valid() && is_whitespace(c = curr())) {
    if (c == '\n' || c == '\r') {
      data.lineNo++;
      data.colNo = 0;
    }
    data.colNo++;
    consume();
  }

  // capture white space at end of file
  if (!valid()) {
    return NULL;
  }

  int start = data.curInd;
  c = curr();

  if (is_dig(c)) {
    return read_number();
  } else if (is_letter(c)) {
    return read_word();
  } else if (c == '"') {
    return read_string_literal();
  } else {
    // read operator
    switch (c) {
    case '\\':
      sym = TOKEN_BSLASH;
      break;
    case ':':
      sym = TOKEN_COLON;
      break;
    case ',':
      sym = TOKEN_COMMA;
      break;
    case '=':
      if (next() == '=') {
        sym = TOKEN_EQEQ;
        consume();
      } else
        sym = TOKEN_EQ;
      break;
    case '/':
      sym = TOKEN_FSLASH;
      break;
    case '>':
      if (next() == '=') {
        sym = TOKEN_GTE;
        consume();
      } else
        sym = TOKEN_GT;
      break;
    case '[':
      sym = TOKEN_LBRACK;
      break;
    case '{':
      sym = TOKEN_LCURL;
      break;
    case '(':
      sym = TOKEN_LPAREN;
      break;
    case '<':
      if (next() == '=') {
        sym = TOKEN_LTE;
        consume();
      } else
        sym = TOKEN_LT;
      break;
    case '-':
      sym = TOKEN_MINUS;
      break;
    case '!':
      if (next() == '=') {
        sym = TOKEN_BANGEQ;
        consume();
      } else
        sym = TOKEN_BANG;
      break;
    case '.':
      sym = TOKEN_PERIOD;
      break;
    case '+':
      sym = TOKEN_PLUS;
      break;
    case ']':
      sym = TOKEN_RBRACK;
      break;
    case '}':
      sym = TOKEN_RCURL;
      break;
    case ')':
      sym = TOKEN_RPAREN;
      break;
    case ';':
      sym = TOKEN_SEMI;
      break;
    case '*':
      sym = TOKEN_STAR;
      break;
    case '~':
      sym = TOKEN_TILDA;
      break;
    default:
      clm_error(data.lineNo, data.colNo, "unknown symbol '%c'", c);
      return NULL;
    }

    consume();
  }

  ClmLexerToken *token = malloc(sizeof(*token));
  token->lineNo = data.lineNo;
  token->colNo = data.colNo;
  token->sym = sym;
  token->raw = string_copy_n(data.programString + start, data.curInd - start);

  return token;
}

const char *clm_lexer_sym_to_string(ClmLexerSymbol s) {
  return clmLexerSymbolStrings[(int)s];
}
