#include <stdio.h>
#include <stdlib.h>

#include "clm.h"
#include "clm_tests.h"

static int clm_test_lexer_ids();
static int clm_test_lexer_numbers();
static int clm_test_lexer_keywords();

int clm_test_lexer() {
  int result = 1;

  printf("Testing ids... ");
  if (!clm_test_lexer_ids()) {
    result = 0;
  } else {
    printf(" OK.\n");
  }

  printf("Testing numbers... ");
  if (!clm_test_lexer_numbers()) {
    result = 0;
  } else {
    printf(" OK.\n");
  }

  printf("Testing keywords... ");
  if (!clm_test_lexer_keywords()) {
    result = 0;
  } else {
    printf(" OK.\n");
  }

  return result;
}

int clm_test_lexer_ids() {
  const char *program = "thisIsAGoodID\n"
                        "this_one_is_too\n"
                        "and_2this_1\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = (ClmLexerToken **)tokens_list->data;

  int i = 0;
  CLM_ASSERT(tokens[i]->sym == LITERAL_ID &&
             string_equals(tokens[i++]->raw, "thisIsAGoodID"));
  CLM_ASSERT(tokens[i]->sym == LITERAL_ID &&
             string_equals(tokens[i++]->raw, "this_one_is_too"));
  CLM_ASSERT(tokens[i]->sym == LITERAL_ID &&
             string_equals(tokens[i++]->raw, "and_2this_1"));
  CLM_ASSERT(tokens[i]->sym == KEYWORD_END);

  array_list_free(tokens_list);
  return 1;
}

int clm_test_lexer_numbers() {
  const char *program = "12345\n"
                        "-3412.\n"
                        "0.0112\n"
                        "2345.0012\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = (ClmLexerToken **)tokens_list->data;

  int i = 0;
  CLM_ASSERT(tokens[i]->sym == LITERAL_INT &&
             string_equals(tokens[i++]->raw, "12345"));
  CLM_ASSERT(tokens[i]->sym == TOKEN_MINUS &&
             string_equals(tokens[i++]->raw, "-"));
  CLM_ASSERT(tokens[i]->sym == LITERAL_FLOAT &&
             string_equals(tokens[i++]->raw, "3412."));
  CLM_ASSERT(tokens[i]->sym == LITERAL_FLOAT &&
             string_equals(tokens[i++]->raw, "0.0112"));
  CLM_ASSERT(tokens[i]->sym == LITERAL_FLOAT &&
             string_equals(tokens[i++]->raw, "2345.0012"));
  CLM_ASSERT(tokens[i]->sym == KEYWORD_END);

  array_list_free(tokens_list);
  return 1;
}

int clm_test_lexer_keywords() {
  const char *program = "and\n"
                        "call\n"
                        "do\n"
                        "else\n"
                        "end\n"
                        "float\n"
                        "for\n"
                        "if\n"
                        "in\n"
                        "int\n"
                        "or\n"
                        "print\n"
                        "printl\n"
                        "return\n"
                        "string\n"
                        "then\n"
                        "to\n"
                        "!\n"
                        "\\\n"
                        ":\n"
                        ",\n"
                        "=\n"
                        "==\n"
                        "/\n"
                        ">\n"
                        ">=\n"
                        "[\n"
                        "{\n"
                        "(\n"
                        "<\n"
                        "<=\n"
                        "-\n"
                        "!=\n"
                        ".\n"
                        "+\n"
                        "]\n"
                        "}\n"
                        ")\n"
                        ";\n"
                        "*\n"
                        "~\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = (ClmLexerToken **)tokens_list->data;

  int i = 0;
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_AND);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_CALL);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_DO);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_ELSE);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_END);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_FLOAT);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_FOR);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_IF);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_IN);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_INT);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_OR);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_PRINT);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_PRINTL);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_RETURN);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_STRING);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_THEN);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_TO);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_BANG);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_BSLASH);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_COLON);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_COMMA);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_EQ);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_EQEQ);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_FSLASH);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_GT);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_GTE);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_LBRACK);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_LCURL);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_LPAREN);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_LT);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_LTE);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_MINUS);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_BANGEQ);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_PERIOD);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_PLUS);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_RBRACK);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_RCURL);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_RPAREN);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_SEMI);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_STAR);
  CLM_ASSERT(tokens[i++]->sym == TOKEN_TILDA);
  CLM_ASSERT(tokens[i++]->sym == KEYWORD_END);

  array_list_free(tokens_list);
  return 1;
}
