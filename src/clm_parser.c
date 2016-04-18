#include "clm.h"
#include "clm_ast.h"

typedef struct ClmParserData {
  ArrayList *parseTree; // ArrayList of ClmStmtNode
  int curInd;
  int numTokens;
  ClmLexerToken **tokens; // pointer to ClmLexerData.tokens->data
  char *prevTokenRaw;     // pointer to a token's raw str in ^
} ClmParserData;

static ClmParserData data;

static void consume() { data.prevTokenRaw = data.tokens[data.curInd++]->raw; }

static ClmLexerToken *curr() { return data.tokens[data.curInd]; }

static ClmLexerToken *prev() { return data.tokens[data.curInd - 1]; }

static ClmLexerToken *next() { return data.tokens[data.curInd + 1]; }

static ArrayList *consume_statements(int ifElse);
static ClmStmtNode *consume_statement();
static int consume_int();
static float consume_float();
static int consume_param_size();
static int consume_return_size();
static ClmExpNode *consume_parameter();
static ClmExpNode *consume_lhs();
static ClmExpNode *consume_expression();
static ClmExpNode *consume_expression_2();
static ClmExpNode *consume_expression_3();
static ClmExpNode *consume_expression_4();
static ClmExpNode *consume_expression_5();
static ClmExpNode *consume_expression_6();

static int accept(ClmLexerSymbol symbol) {
  if (curr()->sym == symbol) {
    consume();
    return 1;
  }
  return 0;
}

static int expect(ClmLexerSymbol symbol) {
  if (accept(symbol)) {
    return 1;
  }
  clm_error(curr()->lineNo, curr()->colNo,
            "Expected token '%s', but found token '%s'",
            clmLexerSymbolStrings[symbol], clmLexerSymbolStrings[curr()->sym]);
  return 0;
}

ArrayList *clm_parser_main(ArrayList *tokens) {
  data.curInd = 0;
  data.numTokens = tokens->length;
  data.tokens = (ClmLexerToken **)tokens->data;
  return consume_statements(0);
}

void clm_print_statements(ArrayList *parseTree) {
  array_list_foreach_2(parseTree, 0, clm_stmt_print);
}

static ArrayList *consume_statements(int ifElse) {
  ArrayList *statements = array_list_new(clm_stmt_free);

  // if we are parsing an if else and an else is next, we don't want to handle
  // that here... it is parsed in the consume_statement function
  // when we parse the actual if else
  while (!accept(KEYWORD_END) && !(ifElse && curr()->sym == KEYWORD_ELSE)) {
    array_list_push(statements, consume_statement());
  }

  return statements;
}

static int consume_int() {
  int val = 0;
  int neg = 0;
  if (accept(TOKEN_MINUS))
    neg = 1;
  expect(LITERAL_INT);

  val = atoi(data.prevTokenRaw);
  return neg ? -val : val;
}

static float consume_float() {
  float val = 0;
  int neg = 0;
  if (accept(TOKEN_MINUS)) {
    neg = 1;
  }
  expect(LITERAL_FLOAT);
  val = atof(data.prevTokenRaw);
  return neg ? -val : val;
}

static int consume_param_size() {
  if (accept(LITERAL_INT)) {
    return atoi(data.prevTokenRaw);
  }
  expect(LITERAL_ID);
  return 0;
}

static int consume_return_size() {
  if (accept(LITERAL_INT)) {
    return atoi(data.prevTokenRaw);
  }
  expect(LITERAL_ID);
  return 0;
}

// id:{r:c}
// id:type
static ClmExpNode *consume_parameter() {
  ClmExpNode *node = NULL;
  if (accept(LITERAL_ID)) {
    node = clm_exp_new_param(data.prevTokenRaw, CLM_TYPE_INT, 1, 1, NULL, NULL);
    node->lineNo = prev()->lineNo;
    node->colNo = prev()->colNo;
    expect(TOKEN_COLON);
    if (accept(TOKEN_LCURL)) {
      node->paramExp.rows = consume_param_size();
      if (!node->paramExp.rows) {
        node->paramExp.rowVar = string_copy(data.prevTokenRaw);
      }
      expect(TOKEN_COLON);
      node->paramExp.cols = consume_param_size();
      if (!node->paramExp.cols) {
        node->paramExp.colVar = string_copy(data.prevTokenRaw);
      }
      expect(TOKEN_RCURL);
      node->paramExp.type = CLM_TYPE_MATRIX;
    } else {
      if (accept(KEYWORD_INT)) {
        node->paramExp.type = CLM_TYPE_INT;
      } else if (accept(KEYWORD_FLOAT)) {
        node->paramExp.type = CLM_TYPE_FLOAT;
      } else if (accept(KEYWORD_STRING)) {
        node->paramExp.type = CLM_TYPE_STRING;
      }
    }
  }
  return node;
}

static ClmExpNode *consume_lhs() {
  int lineNo = curr()->lineNo, colNo = curr()->colNo;

  ClmExpNode *node = NULL;
  expect(LITERAL_ID);
  char *name = data.prevTokenRaw;

  ClmExpNode *rowIndex = NULL, *colIndex = NULL;
  if (accept(TOKEN_LBRACK)) {
    // accepts A[x,y] A[,y] A[x,] A[,]
    if (next()->sym != TOKEN_COMMA)
      rowIndex = consume_expression();
    expect(TOKEN_COMMA);
    if (next()->sym != TOKEN_RBRACK)
      colIndex = consume_expression();
    expect(TOKEN_RBRACK);
  }

  node = clm_exp_new_index(name, rowIndex, colIndex);
  node->lineNo = lineNo;
  node->colNo = colNo;

  return node;
}

static ClmStmtNode *consume_statement() {
  int lineNo = curr()->lineNo, colNo = curr()->colNo;

  if (curr()->sym == LITERAL_ID) {
    ClmExpNode *lhs = consume_lhs();
    expect(TOKEN_EQ);
    ClmExpNode *rhs = consume_expression();

    ClmStmtNode *stmt = clm_stmt_new_assign(lhs, rhs);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(KEYWORD_CALL)) {
    ClmStmtNode *stmt = clm_stmt_new_call(consume_expression());
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(KEYWORD_PRINT)) {
    ClmStmtNode *stmt = clm_stmt_new_print(consume_expression(), 0);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(KEYWORD_PRINTL)) {
    ClmStmtNode *stmt = clm_stmt_new_print(consume_expression(), 1);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(KEYWORD_FOR)) {
    expect(LITERAL_ID);
    char *name = data.prevTokenRaw;
    expect(TOKEN_EQ);
    int startInclusive = 0;
    if (accept(TOKEN_LBRACK))
      startInclusive = 1;
    else
      expect(TOKEN_LPAREN);

    ClmExpNode *start = consume_expression();
    expect(TOKEN_COMMA);
    ClmExpNode *end = consume_expression();
    int endInclusive = 0;
    if (accept(TOKEN_RBRACK))
      endInclusive = 1;
    else
      expect(TOKEN_RPAREN);

    ClmExpNode *delta;
    if (accept(KEYWORD_BY)) {
      delta = consume_expression();
    } else {
      delta = clm_exp_new_int(1);
    }

    expect(KEYWORD_DO);
    ArrayList *body = consume_statements(0);

    ClmStmtNode *stmt = clm_stmt_new_loop(name, start, end, delta, body,
                                          startInclusive, endInclusive);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;

    return stmt;
  } else if (accept(KEYWORD_IF)) {
    ClmExpNode *condition = consume_expression();
    expect(KEYWORD_THEN);
    ArrayList *trueBody = consume_statements(1);

    ArrayList *falseBody = NULL;
    if (accept(KEYWORD_ELSE)) {
      falseBody = consume_statements(0);
    }

    ClmStmtNode *stmt = clm_stmt_new_cond(condition, trueBody, falseBody);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(KEYWORD_RETURN)) {
    ClmStmtNode *stmt = clm_stmt_new_return(consume_expression());
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else if (accept(TOKEN_BSLASH)) {
    expect(LITERAL_ID);
    char *name = data.prevTokenRaw;

    ArrayList *params = array_list_new(clm_exp_free);
    ClmExpNode *param = consume_parameter();
    while (param) {
      array_list_push(params, param);

      if (curr()->sym == TOKEN_MINUS || curr()->sym == TOKEN_EQ)
        break;
      else
        expect(TOKEN_COMMA);

      param = consume_parameter();
    }

    int r = -2, c = -2;
    char *rv = NULL, *cv = NULL;
    ClmType returnType = CLM_TYPE_NONE;
    if (accept(TOKEN_MINUS)) {
      expect(TOKEN_GT);
      if (accept(KEYWORD_INT)) { //\x ... -> int
        returnType = CLM_TYPE_INT;
      } else if (accept(KEYWORD_FLOAT)) { //\x ... -> float
        returnType = CLM_TYPE_FLOAT;
      } else if (accept(KEYWORD_STRING)) { //\x ... -> string
        returnType = CLM_TYPE_STRING;
      } else { //\x ... -> [m:n]
        expect(TOKEN_LBRACK);
        r = consume_return_size();
        if (!r)
          rv = data.prevTokenRaw;
        expect(TOKEN_COLON);
        c = consume_return_size();
        if (!c)
          cv = data.prevTokenRaw;
        expect(TOKEN_RBRACK);

        returnType = CLM_TYPE_MATRIX;
      }
    }

    expect(TOKEN_EQ);
    ArrayList *body = consume_statements(0);

    ClmStmtNode *stmt =
        clm_stmt_new_dec(name, params, returnType, r, rv, c, cv, body);
    stmt->lineNo = lineNo;
    stmt->colNo = colNo;
    return stmt;
  } else {
    clm_error(curr()->lineNo, curr()->colNo, "Unexpected symbol %s",
              clmLexerSymbolStrings[curr()->sym]);
  }
  return NULL;
}

static ClmExpNode *consume_expression() {
  //&& or ||
  ClmExpNode *node1, *node2;
  BoolOp op;
  node1 = consume_expression_2();
  while (curr()->sym == KEYWORD_AND || curr()->sym == KEYWORD_OR) {
    op = accept(KEYWORD_AND) ? BOOL_OP_AND : BOOL_OP_OR, accept(KEYWORD_OR);
    node2 = consume_expression_2();
    node1 = clm_exp_new_bool(op, node1, node2);
  }
  return node1;
}

static ClmExpNode *consume_expression_2() {
  //== or !=
  ClmExpNode *node1, *node2;
  BoolOp op;
  node1 = consume_expression_3();
  while (curr()->sym == TOKEN_EQ || curr()->sym == TOKEN_BANGEQ) {
    op = accept(TOKEN_EQ) ? BOOL_OP_EQ : BOOL_OP_NEQ, accept(TOKEN_BANGEQ);
    node2 = consume_expression_2();
    node1 = clm_exp_new_bool(op, node1, node2);
  }
  return node1;
}

static ClmExpNode *consume_expression_3() {
  //> or < or >= or <=
  ClmExpNode *node1;
  ClmExpNode *node2;
  BoolOp op;
  node1 = consume_expression_4();
  while (curr()->sym == TOKEN_GT || curr()->sym == TOKEN_LT ||
         curr()->sym == TOKEN_GTE || curr()->sym == TOKEN_LTE) {
    op = accept(TOKEN_GT) ? BOOL_OP_GT : accept(TOKEN_LT)
                                             ? BOOL_OP_LT
                                             : accept(TOKEN_GTE) ? BOOL_OP_GTE
                                                                 : BOOL_OP_LTE,
    accept(TOKEN_LTE);
    node2 = consume_expression_4();
    node1 = clm_exp_new_bool(op, node1, node2);
  }
  return node1;
}

static ClmExpNode *consume_expression_4() {
  //+ or -
  ClmExpNode *node1;
  ClmExpNode *node2;
  ArithOp op;
  node1 = consume_expression_5();
  while (curr()->sym == TOKEN_PLUS || curr()->sym == TOKEN_MINUS) {
    op = accept(TOKEN_PLUS) ? ARITH_OP_ADD : ARITH_OP_SUB, accept(TOKEN_MINUS);
    node2 = consume_expression_5();
    node1 = clm_exp_new_arith(op, node1, node2);
  }
  return node1;
}

static ClmExpNode *consume_expression_5() {
  //* or /
  ClmExpNode *node1;
  ClmExpNode *node2;
  ArithOp op;
  node1 = consume_expression_6();
  while (curr()->sym == TOKEN_STAR || curr()->sym == TOKEN_FSLASH) {
    op = accept(TOKEN_STAR) ? ARITH_OP_MULT : ARITH_OP_DIV,
    accept(TOKEN_FSLASH);
    node2 = consume_expression_6();
    node1 = clm_exp_new_arith(op, node1, node2);
  }
  return node1;
}

static ClmExpNode *consume_expression_6() {
  int lineNo = curr()->lineNo, colNo = curr()->colNo;
  if (accept(TOKEN_LPAREN)) {
    ClmExpNode *exp = consume_expression();
    expect(TOKEN_RPAREN);

    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(LITERAL_INT)) {
    ClmExpNode *exp = clm_exp_new_int(atoi(data.prevTokenRaw));
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(LITERAL_FLOAT)) {
    ClmExpNode *exp = clm_exp_new_float(atof(data.prevTokenRaw));
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(LITERAL_STRING)) {
    ClmExpNode *exp = clm_exp_new_string(data.prevTokenRaw);
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(TOKEN_MINUS)) {
    ClmExpNode *exp = clm_exp_new_unary(UNARY_OP_MINUS, consume_expression_6());
    return exp;
  } else if (accept(TOKEN_BANG)) {
    ClmExpNode *exp = clm_exp_new_unary(UNARY_OP_NOT, consume_expression_6());
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(TOKEN_TILDA)) {
    ClmExpNode *exp =
        clm_exp_new_unary(UNARY_OP_TRANSPOSE, consume_expression_6());
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (curr()->sym == LITERAL_ID) {
    if (next()->sym == TOKEN_LPAREN) {
      expect(LITERAL_ID);
      char *name = data.prevTokenRaw;
      expect(TOKEN_LPAREN);

      ArrayList *params = array_list_new(clm_exp_free);
      while (curr()->sym != TOKEN_RPAREN) {
        array_list_push(params, consume_expression());
        if (accept(TOKEN_COMMA))
          continue;
        else
          break;
      }
      expect(TOKEN_RPAREN);

      ClmExpNode *exp = clm_exp_new_call(name, params);
      exp->lineNo = lineNo;
      exp->colNo = colNo;
      return exp;
    } else {
      ClmExpNode *exp = consume_lhs();
      exp->lineNo = lineNo;
      exp->colNo = colNo;
      return exp;
    }
  } else if (accept(TOKEN_LBRACK)) {
    int rows = 0, cols = 0;
    char *rowVar = NULL, *colVar = NULL;
    if (accept(LITERAL_ID)) {
      rowVar = data.prevTokenRaw;
    } else {
      expect(LITERAL_INT);
      rows = atoi(data.prevTokenRaw);
    }
    expect(TOKEN_COLON);
    if (accept(LITERAL_ID)) {
      colVar = data.prevTokenRaw;
    } else {
      expect(LITERAL_INT);
      cols = atoi(data.prevTokenRaw);
    }
    expect(TOKEN_RBRACK);

    ClmExpNode *exp = clm_exp_new_empty_mat_dec(rows, cols, rowVar, colVar);
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else if (accept(TOKEN_LCURL)) {
    int cols, num = 0;
    float *list;
    int start = prev()->lineNo;

    list = malloc(sizeof(*list));
    if (curr()->sym == LITERAL_INT ||
        (curr()->sym == TOKEN_MINUS && next()->sym == LITERAL_INT))
      list[num++] = consume_int();
    else {
      list[num++] = consume_float();
    }

    // curr is int or float
    // or curr is negative and next is int or float
    while (curr()->sym == LITERAL_INT || curr()->sym == LITERAL_FLOAT ||
           (curr()->sym == TOKEN_MINUS &&
            (next()->sym == LITERAL_INT || next()->sym == LITERAL_FLOAT))) {
      list = realloc(list, (num + 1) * sizeof(*list));
      if (curr()->sym == LITERAL_INT)
        list[num++] = consume_int();
      else {
        list[num++] = consume_float();
      }
    }

    cols = num;
    while (accept(TOKEN_COMMA)) {
      while (curr()->sym == LITERAL_INT || curr()->sym == LITERAL_FLOAT ||
             (curr()->sym == TOKEN_MINUS &&
              (next()->sym == LITERAL_INT || next()->sym == LITERAL_FLOAT))) {
        list = realloc(list, (num + 1) * sizeof(*list));
        if (curr()->sym == LITERAL_INT)
          list[num++] = consume_int();
        else {
          list[num++] = consume_float();
        }
      }
    }
    expect(TOKEN_RCURL);

    ClmExpNode *exp = clm_exp_new_mat_dec(list, num, cols);
    exp->lineNo = lineNo;
    exp->colNo = colNo;
    return exp;
  } else {
    clm_error(curr()->lineNo, curr()->colNo, "Unexpected symbol %s",
              clmLexerSymbolStrings[curr()->sym]);
  }

  // should never return from here... as it will error and exit
  return clm_exp_new_int(-1);
}
