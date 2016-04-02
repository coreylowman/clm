#include <stdio.h>
#include <stdlib.h>

#include "lexer/clm_lexer.h"
#include "util/array_list.h"
#include "util/string.h"

#include "clm_test_lexer.h"
#include "clm_test_utils.h"

int clm_test_lexer() {
  int result = 1;
  result &= clm_test_lexer_ids();
  result &= clm_test_lexer_numbers();
  result &= clm_test_lexer_keywords();
  return result;
}

int clm_test_lexer_ids() {
  const char *program = "thisIsAGoodID\n"
                        "this_one_is_too\n"
                        "_and_this_1\n"
                        "_2\n"
                        "1not_this_one\n"
                        "__nore~thisOne\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = tokens_list->data;

  CLM_ASSERT(tokens_list->length == 10);
  CLM_ASSERT(tokens[0]->sym == LEX_ID &&
             string_equals(tokens[0]->raw, "thisIsAGoodID"));
  CLM_ASSERT(tokens[1]->sym == LEX_ID &&
             string_equals(tokens[1]->raw, "this_one_is_too"));
  CLM_ASSERT(tokens[2]->sym == LEX_ID &&
             string_equals(tokens[2]->raw, "_and_this_1"));
  CLM_ASSERT(tokens[3]->sym == LEX_ID && string_equals(tokens[3]->raw, "_2"));
  CLM_ASSERT(tokens[4]->sym == LEX_INT && string_equals(tokens[4]->raw, "1"));
  CLM_ASSERT(tokens[5]->sym == LEX_ID &&
             string_equals(tokens[5]->raw, "not_this_one"));
  CLM_ASSERT(tokens[6]->sym == LEX_ID &&
             string_equals(tokens[6]->raw, "__nore"));
  CLM_ASSERT(tokens[7]->sym == LEX_TILDA && string_equals(tokens[7]->raw, "~"));
  CLM_ASSERT(tokens[8]->sym == LEX_ID &&
             string_equals(tokens[8]->raw, "thisOne"));
  CLM_ASSERT(tokens[9]->sym == LEX_END);

  array_list_free(tokens_list);
  return 1;
}

int clm_test_lexer_numbers() {
  const char *program = "12345\n"
                        "-3412.\n"
                        ".0112\n"
                        "2345.0012\n"
                        ".\n"
                        ".1.1.2\n"
                        "4.3.2.31\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = tokens_list->data;

  CLM_ASSERT(tokens_list->length == 8);
  CLM_ASSERT(tokens[0]->sym == LEX_INT &&
             string_equals(tokens[0]->raw, "12345"));
  CLM_ASSERT(tokens[1]->sym == LEX_SUB && string_equals(tokens[1]->raw, "-"));
  CLM_ASSERT(tokens[2]->sym == LEX_FLOAT &&
             string_equals(tokens[2]->raw, "3412."));
  CLM_ASSERT(tokens[3]->sym == LEX_FLOAT &&
             string_equals(tokens[3]->raw, ".0112"));
  CLM_ASSERT(tokens[4]->sym == LEX_FLOAT &&
             string_equals(tokens[4]->raw, "2345.0012"));
  CLM_ASSERT(tokens[5]->sym == LEX_FLOAT &&
             string_equals(tokens[5]->raw, "1.2"));
  CLM_ASSERT(tokens[6]->sym == LEX_FLOAT &&
             string_equals(tokens[6]->raw, "2.31"));
  CLM_ASSERT(tokens[7]->sym == LEX_END);

  array_list_free(tokens_list);
  return 1;
}

int clm_test_lexer_keywords() {
  const char *program = "+\n"
                        "and\n"
                        "=\n"
                        "@\n"
                        "\\\n"
                        "by\n"
                        "call"
                        ":\n"
                        ",\n"
                        "/\n"
                        "do\n"
                        "else\n"
                        "end\n"
                        "==\n"
                        "float\n"
                        "for\n"
                        ">\n"
                        ">=\n"
                        "#\n"
                        "if\n"
                        "int\n"
                        "[\n"
                        "{\n"
                        "(\n"
                        "<\n"
                        "<=\n"
                        "*\n"
                        "!=\n"
                        "!\n"
                        "or\n"
                        "print\n"
                        "printl\n"
                        "?\n"
                        "]\n"
                        "}\n"
                        "return\n"
                        ")\n"
                        ";\n"
                        "string\n"
                        "-\n"
                        "then\n"
                        "~\n"
                        "to\n";

  ArrayList *tokens_list = clm_lexer_main(program);
  ClmLexerToken **tokens = tokens_list->data;

  int i = 0;
  CLM_ASSERT(tokens_list->length == 43);
  CLM_ASSERT(tokens[i++]->sym == LEX_ADD);
  CLM_ASSERT(tokens[i++]->sym == LEX_AND);
  CLM_ASSERT(tokens[i++]->sym == LEX_ASSIGN);
  CLM_ASSERT(tokens[i++]->sym == LEX_BACKSLASH);
  CLM_ASSERT(tokens[i++]->sym == LEX_BY);
  CLM_ASSERT(tokens[i++]->sym == LEX_CALL);
  CLM_ASSERT(tokens[i++]->sym == LEX_COLON);
  CLM_ASSERT(tokens[i++]->sym == LEX_COMMA);
  CLM_ASSERT(tokens[i++]->sym == LEX_DIV);
  CLM_ASSERT(tokens[i++]->sym == LEX_DO);
  CLM_ASSERT(tokens[i++]->sym == LEX_ELSE);
  CLM_ASSERT(tokens[i++]->sym == LEX_END);
  CLM_ASSERT(tokens[i++]->sym == LEX_EQ);
  CLM_ASSERT(tokens[i++]->sym == LEX_FLOAT_WORD);
  CLM_ASSERT(tokens[i++]->sym == LEX_FOR);
  CLM_ASSERT(tokens[i++]->sym == LEX_GT);
  CLM_ASSERT(tokens[i++]->sym == LEX_GTE);
  CLM_ASSERT(tokens[i++]->sym == LEX_IF);
  CLM_ASSERT(tokens[i++]->sym == LEX_INT_WORD);
  CLM_ASSERT(tokens[i++]->sym == LEX_LBRACK);
  CLM_ASSERT(tokens[i++]->sym == LEX_LCURL);
  CLM_ASSERT(tokens[i++]->sym == LEX_LPAREN);
  CLM_ASSERT(tokens[i++]->sym == LEX_LT);
  CLM_ASSERT(tokens[i++]->sym == LEX_LTE);
  CLM_ASSERT(tokens[i++]->sym == LEX_MULT);
  CLM_ASSERT(tokens[i++]->sym == LEX_NEQ);
  CLM_ASSERT(tokens[i++]->sym == LEX_NOT);
  CLM_ASSERT(tokens[i++]->sym == LEX_OR);
  CLM_ASSERT(tokens[i++]->sym == LEX_PRINT);
  CLM_ASSERT(tokens[i++]->sym == LEX_PRINTL);
  CLM_ASSERT(tokens[i++]->sym == LEX_RBRACK);
  CLM_ASSERT(tokens[i++]->sym == LEX_RCURL);
  CLM_ASSERT(tokens[i++]->sym == LEX_RETURN);
  CLM_ASSERT(tokens[i++]->sym == LEX_RPAREN);
  CLM_ASSERT(tokens[i++]->sym == LEX_SEMI);
  CLM_ASSERT(tokens[i++]->sym == LEX_STRING_WORD);
  CLM_ASSERT(tokens[i++]->sym == LEX_SUB);
  CLM_ASSERT(tokens[i++]->sym == LEX_THEN);
  CLM_ASSERT(tokens[i++]->sym == LEX_TILDA);
  CLM_ASSERT(tokens[i++]->sym == LEX_TO);
  CLM_ASSERT(tokens[i++]->sym == LEX_END);

  array_list_free(tokens_list);
  return 1;
}
