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
static ClmStmtNode *consume_for_loop();
static ClmStmtNode *consume_while_loop();
static ClmStmtNode *consume_function_decl();
static ClmStmtNode *consume_statement();
static int consume_int();
static float consume_float();
static int consume_int_or_id(char **dest);
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

static int curr_is_int() {
  return curr()->sym == LITERAL_INT ||
         (curr()->sym == TOKEN_MINUS && next()->sym == LITERAL_INT);
}

static int curr_is_float() {
  return curr()->sym == LITERAL_FLOAT ||
         (curr()->sym == TOKEN_MINUS && next()->sym == LITERAL_FLOAT);
}

static int curr_is_number() { return curr_is_int() || curr_is_float(); }

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

static float consume_number() {
  if (curr_is_int()) {
    return consume_int();
  }
  return consume_float();
}

static int consume_int_or_id(char **dest) {
  if (accept(LITERAL_INT)) {
    dest = NULL;
    return atoi(data.prevTokenRaw);
  }
  expect(LITERAL_ID);
  *dest = data.prevTokenRaw;
  return 0;
}

// id[r:c]
// id:type
static ClmExpNode *consume_parameter() {
  ClmExpNode *node = NULL;
  char *name;
  ClmType type;
  int rows, cols;
  char *rowVar, *colVar;

  if (accept(LITERAL_ID)) {
    name = data.prevTokenRaw;

    if (accept(TOKEN_LBRACK)) {
      rows = consume_int_or_id(&rowVar);

      expect(TOKEN_COLON);

      cols = consume_int_or_id(&colVar);

      expect(TOKEN_RBRACK);

      type = CLM_TYPE_MATRIX;
    } else {
      expect(TOKEN_COLON);
      if (accept(KEYWORD_INT)) {
        type = CLM_TYPE_INT;
      } else if (accept(KEYWORD_FLOAT)) {
        type = CLM_TYPE_FLOAT;
      } else if (accept(KEYWORD_STRING)) {
        type = CLM_TYPE_STRING;
      }
    }

    node = clm_exp_new_param(name, type, rows, cols, rowVar, colVar);
    node->lineNo = prev()->lineNo;
    node->colNo = prev()->colNo;
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

/*
  for id in exp[,exp]..exp do
    statements
  end
*/
static ClmStmtNode *consume_for_loop(){
  int lineNo = curr()->lineNo, colNo = curr()->colNo;
  
  expect(KEYWORD_FOR);
  
  expect(LITERAL_ID);
  char *name = data.prevTokenRaw;

  expect(KEYWORD_IN);    

  ClmExpNode *start = consume_expression();
  
  ClmExpNode *delta;
  if (accept(TOKEN_COMMA)) {
    delta = consume_expression();
  } else {
    delta = clm_exp_new_int(1);
  }    

  expect(TOKEN_PERIOD);
  expect(TOKEN_PERIOD);

  ClmExpNode *end = consume_expression();

  expect(KEYWORD_DO);

  ArrayList *body = consume_statements(0);

  ClmStmtNode *stmt = clm_stmt_new_for_loop(name, start, end, delta, body);
  stmt->lineNo = lineNo;
  stmt->colNo = colNo;

  return stmt;
}

/*
  while exp do
    statements
  end
*/
static ClmStmtNode *consume_while_loop(){
  int lineNo = curr()->lineNo, colNo = curr()->colNo;
  
  expect(KEYWORD_WHILE);   

  ClmExpNode *condition = consume_expression();
  
  expect(KEYWORD_DO);

  ArrayList *body = consume_statements(0);

  ClmStmtNode *stmt = clm_stmt_new_while_loop(condition, body);
  stmt->lineNo = lineNo;
  stmt->colNo = colNo;

  return stmt;
}

/*
  \id [param]* [-> returnType] =
    statements
  end
*/
static ClmStmtNode *consume_function_decl(){
  int lineNo = curr()->lineNo, colNo = curr()->colNo;

  expect(TOKEN_BSLASH);

  expect(LITERAL_ID);
  char *name = data.prevTokenRaw;

  ArrayList *params = array_list_new(clm_exp_free);
  
  ClmExpNode *param = consume_parameter();
  while (param) {
    array_list_push(params, param);

    if (curr()->sym == TOKEN_MINUS || curr()->sym == TOKEN_EQ)
      break;

    param = consume_parameter();
  }

  int rows = -1, cols = -1;
  char *rowVar = NULL, *colVar = NULL;
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
      rows = consume_int_or_id(&rowVar);
      expect(TOKEN_COLON);
      cols = consume_int_or_id(&colVar);
      expect(TOKEN_RBRACK);

      returnType = CLM_TYPE_MATRIX;
    }
  }

  expect(TOKEN_EQ);

  ArrayList *body = consume_statements(0);

  ClmStmtNode *stmt = clm_stmt_new_dec(name, params, returnType, rows, cols,
                                       rowVar, colVar, body);
  stmt->lineNo = lineNo;
  stmt->colNo = colNo;
  return stmt;
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
  } else if (curr()->sym == KEYWORD_FOR) {
    return consume_for_loop();
  } else if (curr()->sym == KEYWORD_WHILE) {
    return consume_while_loop();
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
  } else if (curr()->sym == TOKEN_BSLASH) {
    consume_function_decl();
  } else {
    clm_error(curr()->lineNo, curr()->colNo, "Unexpected symbol %s",
              clmLexerSymbolStrings[curr()->sym]);
  }
  return NULL;
}

static BoolOp sym_to_bool_op(ClmLexerSymbol sym) {
  switch (sym) {
  case KEYWORD_AND:
    return BOOL_OP_AND;
  case KEYWORD_OR:
    return BOOL_OP_OR;
  case TOKEN_EQ:
    return BOOL_OP_EQ;
  case TOKEN_BANGEQ:
    return BOOL_OP_NEQ;
  case TOKEN_GT:
    return BOOL_OP_GT;
  case TOKEN_LT:
    return BOOL_OP_LT;
  case TOKEN_GTE:
    return BOOL_OP_GTE;
  case TOKEN_LTE:
  // fallthrough
  default:
    return BOOL_OP_LTE;
  }
}

static ArithOp sym_to_arith_op(ClmLexerSymbol sym) {
  switch (sym) {
  case TOKEN_PLUS:
    return ARITH_OP_ADD;
  case TOKEN_MINUS:
    return ARITH_OP_SUB;
  case TOKEN_STAR:
    return ARITH_OP_MULT;
  case TOKEN_FSLASH:
  // fallthrough
  default:
    return ARITH_OP_DIV;
  }
}

static UnaryOp sym_to_unary_op(ClmLexerSymbol sym) {
  switch (sym) {
  case TOKEN_MINUS:
    return UNARY_OP_MINUS;
  case TOKEN_BANG:
    return UNARY_OP_NOT;
  case TOKEN_TILDA:
  // fallthrough
  default:
    return UNARY_OP_TRANSPOSE;
  }
}

static ClmExpNode *consume_expression() {
  //&& or ||
  ClmExpNode *node1, *node2;
  BoolOp op;
  node1 = consume_expression_2();
  while (accept(KEYWORD_AND) || accept(KEYWORD_OR)) {
    op = sym_to_bool_op(prev()->sym);
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
  while (accept(TOKEN_EQ) || accept(TOKEN_BANGEQ)) {
    op = sym_to_bool_op(prev()->sym);
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
  while (accept(TOKEN_GT) || accept(TOKEN_LT) || accept(TOKEN_GTE) ||
         accept(TOKEN_LTE)) {
    op = sym_to_bool_op(prev()->sym);
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
  while (accept(TOKEN_PLUS) || accept(TOKEN_MINUS)) {
    op = sym_to_arith_op(prev()->sym);
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
  while (accept(TOKEN_STAR) || accept(TOKEN_FSLASH)) {
    op = sym_to_arith_op(prev()->sym);
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
  } else if (accept(TOKEN_MINUS) || accept(TOKEN_BANG) || accept(TOKEN_TILDA)) {
    ClmExpNode *exp =
        clm_exp_new_unary(sym_to_unary_op(prev()->sym), consume_expression_6());
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

    rows = consume_int_or_id(&rowVar);
    expect(TOKEN_COLON);
    cols = consume_int_or_id(&colVar);
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

    // TODO consume expressions instead of just numbers
    do {
      list = realloc(list, (num + 1) * sizeof(*list));
      list[num++] = consume_number();
    } while (curr_is_number());

    cols = num;

    int i;
    while (accept(TOKEN_COMMA)) {
      for (i = 0; i < cols; i++) {
        list = realloc(list, (num + 1) * sizeof(*list));
        list[num++] = consume_number();
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
  return NULL;
}
