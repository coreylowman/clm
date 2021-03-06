#include <stdlib.h>

#include "clm.h"
#include "clm_ast.h"
#include "clm_scope.h"
#include "clm_type.h"

// valid arith ops
// string string         +
// string int
// string float
// string matrix
// int string
// int int               + - * /
// int float             + - * /
// int matrix                *
// float string
// float int             + - * /
// float float           + - * /
// float matrix              *
// matrix string
// matrix int                * /
// matrix float              * /
// matrix matrix         + - * /
static int arith_op_is_valid_for(ArithOp op, ClmType left, ClmType right) {
  if (left == CLM_TYPE_STRING && right == CLM_TYPE_STRING) {
    if (op == ARITH_OP_ADD) {
      return 1;
    }
  } else if (left == right) {
    return 1;
  } else if (clm_type_is_number(left) && clm_type_is_number(right)) {
    return 1;
  } else if (clm_type_is_number(left) && right == CLM_TYPE_MATRIX) {
    if (op == ARITH_OP_MULT) {
      return 1;
    }
  } else if (left == CLM_TYPE_MATRIX && clm_type_is_number(right)) {
    if (op == ARITH_OP_MULT || op == ARITH_OP_DIV) {
      return 1;
    }
  }
  return 0;
}

// valid bool ops
// string string         == !=
// string int
// string float
// string matrix
// int string
// int int               == != > >= < <= && ||
// int float             == != > >= < <= && ||
// int matrix
// float string
// float int             == != > >= < <= && ||
// float float           == != > >= < <= && ||
// float matrix
// matrix string
// matrix int
// matrix float
// matrix matrix         == != && ||
static int bool_op_is_valid_for(BoolOp op, ClmType left, ClmType right) {
  if (left == CLM_TYPE_STRING && right == CLM_TYPE_STRING) {
    if (op == BOOL_OP_EQ || op == BOOL_OP_NEQ) {
      return 1;
    }
  } else if (clm_type_is_number(left) && clm_type_is_number(right)) {
    return 1;
  } else if (left == CLM_TYPE_MATRIX && right == CLM_TYPE_MATRIX) {
    return 1;
  }
  return 0;
}

// valid unary ops
// string
// int            ! -
// float          -
// mat            ~ -
static int unary_op_is_valid_for(UnaryOp op, ClmType type) {
  switch (type) {
  case CLM_TYPE_INT:
    return op == UNARY_OP_MINUS || op == UNARY_OP_NOT;
  case CLM_TYPE_FLOAT:
    return op == UNARY_OP_MINUS;
  case CLM_TYPE_MATRIX:
    return op == UNARY_OP_TRANSPOSE || op == UNARY_OP_MINUS;
  default:
    return 0;
  }
}

static void type_check_expression(ClmExpNode *node, ClmScope *scope) {
  if (node == NULL)
    return;
  switch (node->type) {
  case EXP_TYPE_INT:
  case EXP_TYPE_FLOAT:
  case EXP_TYPE_STRING:
    break;
  case EXP_TYPE_ARITH: {
    type_check_expression(node->arithExp.right, scope);
    type_check_expression(node->arithExp.left, scope);
    ClmType left_type = clm_type_of_exp(node->arithExp.left, scope);
    ClmType right_type = clm_type_of_exp(node->arithExp.right, scope);
    if (!arith_op_is_valid_for(node->arithExp.operand, left_type, right_type)) {
      // error... arith operand not valid for left type and right type
      clm_error(node->lineNo, node->colNo,
                "Arithmetic operand %s not valid for types %s and %s",
                arith_op_to_string(node->arithExp.operand),
                clm_type_to_string(left_type), clm_type_to_string(right_type));
    }

    if (left_type == CLM_TYPE_MATRIX && right_type == CLM_TYPE_MATRIX) {
      int rrows, rcols;
      int lrows, lcols;
      clm_size_of_exp(node->arithExp.right, scope, &rrows, &rcols);
      clm_size_of_exp(node->arithExp.left, scope, &lrows, &lcols);

      switch (node->arithExp.operand) {
      case ARITH_OP_MULT:
        if (lcols > 0 && rrows > 0 && lcols != rrows) {
          clm_error(node->lineNo, node->colNo, "Matrix multiply expects "
                                               "matrices A:[m:n], B:[q:r], "
                                               "where n == q");
        }
        // TODO warn if rowVar != rowVar || colVar != colVar?
        break;
      case ARITH_OP_DIV:
      case ARITH_OP_ADD:
      case ARITH_OP_SUB:
        if ((rrows > 0 && lrows > 0 && rrows != lrows) ||
            (rcols > 0 && lcols > 0 && rcols != lcols)) {
          clm_error(node->lineNo, node->colNo,
                    "Matrix element wise arith operation expects matrices "
                    "A:[m:n], B:[q:r], where m == q and n == r");
        }
        // TODO warn if rowVar != rowVar || colVar != colVar?
        break;
      default:
        break;
      }
    }
    break;
  }
  case EXP_TYPE_BOOL: {
    type_check_expression(node->boolExp.right, scope);
    type_check_expression(node->boolExp.left, scope);
    ClmType left_type = clm_type_of_exp(node->boolExp.left, scope);
    ClmType right_type = clm_type_of_exp(node->boolExp.right, scope);
    if (!bool_op_is_valid_for(node->boolExp.operand, left_type, right_type)) {
      // error... bool operand not valid for left type and right type
      clm_error(node->lineNo, node->colNo,
                "Boolean operand %s not valid for types %s and %s",
                bool_op_to_string(node->boolExp.operand),
                clm_type_to_string(left_type), clm_type_to_string(right_type));
    }

    if (left_type == CLM_TYPE_MATRIX && right_type == CLM_TYPE_MATRIX) {
      int rrows, rcols;
      int lrows, lcols;
      clm_size_of_exp(node->arithExp.right, scope, &rrows, &rcols);
      clm_size_of_exp(node->arithExp.left, scope, &lrows, &lcols);
      if ((rrows > 0 && lrows > 0 && rrows != lrows) ||
          (rcols > 0 && lcols > 0 && rcols != lcols)) {
        clm_error(node->lineNo, node->colNo,
                  "Matrix element wise boolean operation expects matrices "
                  "A:[m:n], B:[q:r], where m == q and n == r");
      }
      // TODO warn if rowVar != rowVar || colVar != colVar?
    }
    break;
  }
  case EXP_TYPE_CALL: {
    int i;
    ClmSymbol *symbol = clm_scope_find(scope, node->callExp.name);
    if (symbol->type != CLM_TYPE_FUNCTION) {
      // error... calling a non-function?
      clm_error(node->lineNo, node->colNo, "%s is not a function",
                node->callExp.name);
    }
    ClmStmtNode *stmt = symbol->declaration;
    if (stmt->funcDecStmt.parameters->length != node->callExp.params->length) {
      // error... not enough or more than enough parameters
      int diff =
          stmt->funcDecStmt.parameters->length - node->callExp.params->length;
      if (diff < 0) {
        clm_error(node->lineNo, node->colNo,
                  "Expected %d less parameters for function %s", -diff,
                  node->callExp.name);
      } else {
        clm_error(node->lineNo, node->colNo,
                  "Expected %d more parameters for function %s", diff,
                  node->callExp.name);
      }
    }
    for (i = 0; i < node->callExp.params->length; i++) {
      ClmExpNode *param = node->callExp.params->data[i];
      ClmExpNode *expected = stmt->funcDecStmt.parameters->data[i];
      type_check_expression(param, scope);

      if (expected->paramExp.type != clm_type_of_exp(param, scope)) {
        // error... incorrect type passed in
        clm_error(node->lineNo, node->colNo,
                  "In call to function %s, expected type %s for parameter %d,\
                        but found type %s",
                  node->callExp.name,
                  clm_type_to_string(expected->paramExp.type), i,
                  clm_type_to_string(clm_type_of_exp(param, scope)));
      }

      if (expected->paramExp.type == CLM_TYPE_MATRIX) {
        int eRows, eCols;
        int pRows, pCols;
        clm_size_of_exp(expected, scope, &eRows, &eCols);
        clm_size_of_exp(param, scope, &pRows, &pCols);

        if (eRows > 0 && pRows > 0 && eRows != pRows) {
          clm_error(node->lineNo, node->colNo, "Parameter was expecting a "
                                               "matrix with %d rows, but is "
                                               "passed a matrix with %d rows",
                    eRows, pRows);
        }

        if (eCols > 0 && pCols > 0 && eCols != pCols) {
          clm_error(node->lineNo, node->colNo, "Parameter was expecting a "
                                               "matrix with %d cols, but is "
                                               "passed a matrix with %d cols",
                    eCols, pCols);
        }
      }
    }
    break;
  }
  case EXP_TYPE_INDEX: {
    // TODO check if index is greater than constant sized matrix size
    type_check_expression(node->indExp.rowIndex, scope);
    type_check_expression(node->indExp.colIndex, scope);

    if (!clm_exp_has_no_inds(node) &&
        clm_type_of_ind(node, scope) != CLM_TYPE_MATRIX) {
      // error... only matrices should be indexed into
      clm_error(node->lineNo, node->colNo,
                "Unexpected indices... only matrices can be indexed into");
    }
    ClmType row_index_type = clm_type_of_exp(node->indExp.rowIndex, scope);
    ClmType col_index_type = clm_type_of_exp(node->indExp.colIndex, scope);
    if (!clm_type_is_number(row_index_type) &&
        row_index_type != CLM_TYPE_NONE) {
      // error... invalid index
      clm_error(node->lineNo, node->colNo,
                "Invalid row index, expecting a number or a '#'");
    }
    if (!clm_type_is_number(col_index_type) &&
        col_index_type != CLM_TYPE_NONE) {
      // error... invalid index
      clm_error(node->lineNo, node->colNo,
                "Invalid column index, expecting a number or a '#'");
    }
    break;
  }
  case EXP_TYPE_MAT_DEC:
    if (node->matDecExp.arr != NULL &&
        node->matDecExp.size.rows * node->matDecExp.size.cols !=
            node->matDecExp.length) {
      // error... mat dec rows or cols have in specific number of items
      clm_error(node->lineNo, node->colNo, "Invalid matrix declaration");
    }
    break;
  case EXP_TYPE_PARAM:
    break;
  case EXP_TYPE_UNARY:
    type_check_expression(node->unaryExp.node, scope);
    ClmType type = clm_type_of_exp(node->unaryExp.node, scope);
    if (!unary_op_is_valid_for(node->unaryExp.operand, type)) {
      // error... unary operand not valid for type
      clm_error(
          node->lineNo, node->colNo, "Unary operand %s not valid for type %s",
          unary_op_to_string(node->unaryExp.operand), clm_type_to_string(type));
    }
    break;
  }
}

static void type_check_stmts(ArrayList *statements, ClmScope *scope) {
  if (statements == NULL)
    return;
  int i;
  for (i = 0; i < statements->length; i++) {
    ClmStmtNode *node = statements->data[i];
    switch (node->type) {
    case STMT_TYPE_ASSIGN:
      // TODO check if size of expression matches size of variable
      type_check_expression(node->assignStmt.lhs, scope);
      type_check_expression(node->assignStmt.rhs, scope);
      if (node->assignStmt.lhs->type != EXP_TYPE_INDEX)
        clm_error(node->lineNo, node->colNo, "Must assign to a variable");
      break;
    case STMT_TYPE_CALL:
      type_check_expression(node->callExpr, scope);
      break;
    case STMT_TYPE_CONDITIONAL: {
      type_check_expression(node->conditionStmt.condition, scope);
      if (clm_type_of_exp(node->conditionStmt.condition, scope) !=
          CLM_TYPE_INT) {
        // error... condition should be a boolean value
        clm_error(node->lineNo, node->colNo,
                  "Condition expression should be a boolean, but got type %s",
                  clm_type_to_string(
                      clm_type_of_exp(node->conditionStmt.condition, scope)));
      }
      ClmScope *true_scope =
          clm_scope_find_child(scope, node->conditionStmt.trueBody);
      type_check_stmts(node->conditionStmt.trueBody, true_scope);
      if (node->conditionStmt.falseBody != NULL) {
        ClmScope *false_scope =
            clm_scope_find_child(scope, node->conditionStmt.falseBody);
        type_check_stmts(node->conditionStmt.falseBody, false_scope);
      }
      break;
    }
    case STMT_TYPE_FUNC_DEC: {
      ClmScope *function_scope = clm_scope_find_child(scope, node);
      type_check_stmts(node->funcDecStmt.body, function_scope);
      // todo check return type
      break;
    }
    case STMT_TYPE_WHILE_LOOP:
    {
      type_check_expression(node->whileLoopStmt.condition, scope);
      ClmType conditionType = clm_type_of_exp(node->whileLoopStmt.condition, scope);
      if(conditionType != CLM_TYPE_INT){
        // error, condition should be a boolean value
        clm_error(node->lineNo, node->colNo, "While loop condition should be a boolean, but got type %s", clm_type_to_string(conditionType));
      }
      break;
    }
    case STMT_TYPE_FOR_LOOP: {
      type_check_expression(node->forLoopStmt.start, scope);
      type_check_expression(node->forLoopStmt.end, scope);
      type_check_expression(node->forLoopStmt.delta, scope);

      if (clm_type_of_exp(node->forLoopStmt.start, scope) != CLM_TYPE_INT) {
        // error.. loop var should be an int
        clm_error(
            node->lineNo, node->colNo,
            "Loop starting expression should be an int, but got type %s",
            clm_type_to_string(clm_type_of_exp(node->forLoopStmt.start, scope)));
      }
      if (clm_type_of_exp(node->forLoopStmt.end, scope) != CLM_TYPE_INT) {
        // error.. loop var should be a number
        clm_error(
            node->lineNo, node->colNo,
            "Loop ending expression should be an int, but got type %s",
            clm_type_to_string(clm_type_of_exp(node->forLoopStmt.end, scope)));
      }
      if (clm_type_of_exp(node->forLoopStmt.delta, scope) != CLM_TYPE_INT) {
        // error.. loop var should be a number
        clm_error(
            node->lineNo, node->colNo,
            "Loop delta expression should be an int, but got type %s",
            clm_type_to_string(clm_type_of_exp(node->forLoopStmt.delta, scope)));
      }

      ClmScope *loop_scope = clm_scope_find_child(scope, node);
      type_check_stmts(node->forLoopStmt.body, loop_scope);
      break;
    }
    case STMT_TYPE_PRINT:
      type_check_expression(node->printStmt.expression, scope);
      break;
    case STMT_TYPE_RET:
      type_check_expression(node->returnExpr, scope);
      break;
    }
  }
}

static int check_function_returns(ArrayList *body, ClmScope *scope,
                                  ClmType returnType) {
  int has_return = 0;
  int i;
  for (i = 0; i < body->length; i++) {
    ClmStmtNode *node = body->data[i];
    switch (node->type) {
    case STMT_TYPE_ASSIGN:
      break;
    case STMT_TYPE_CALL:
      break;
    case STMT_TYPE_CONDITIONAL: {
      // todo are these scopes real?
      ClmScope *true_scope =
          clm_scope_find_child(scope, node->conditionStmt.trueBody);
      int true_return = check_function_returns(node->conditionStmt.trueBody,
                                               true_scope, returnType);

      if (node->conditionStmt.falseBody != NULL) {
        ClmScope *false_scope =
            clm_scope_find_child(scope, node->conditionStmt.falseBody);
        int false_return = check_function_returns(node->conditionStmt.falseBody,
                                                  false_scope, returnType);

        // if both have valid returns in them we have a valid return
        if (true_return && false_return)
          has_return = 1;
      }

      break;
    }
    case STMT_TYPE_FUNC_DEC:
      // func dec in a function dec?
      break;
    case STMT_TYPE_WHILE_LOOP:
      has_return |= check_function_returns(node->whileLoopStmt.body, scope, returnType);
      break;
    case STMT_TYPE_FOR_LOOP:
      has_return |=
          check_function_returns(node->forLoopStmt.body, scope, returnType);
      break;
    case STMT_TYPE_PRINT:
      break;
    case STMT_TYPE_RET:
      if (clm_type_of_exp(node->returnExpr, scope) != returnType) {
        // error... return type invalid
        clm_error(node->lineNo, node->colNo,
                  "Function expects return type %s, but got type %s",
                  clm_type_to_string(returnType),
                  clm_type_to_string(clm_type_of_exp(node->returnExpr, scope)));
      } else {
        has_return = 1;
      }
      break;
    }
  }
  return has_return;
}

static void check_returns(ArrayList *statements, ClmScope *scope) {
  int i;
  for (i = 0; i < statements->length; i++) {
    ClmStmtNode *node = statements->data[i];
    switch (node->type) {
    case STMT_TYPE_ASSIGN:
      break;
    case STMT_TYPE_CALL:
      break;
    case STMT_TYPE_CONDITIONAL: {
      ClmScope *true_scope =
          clm_scope_find_child(scope, node->conditionStmt.trueBody);
      check_returns(node->conditionStmt.trueBody, true_scope);

      if (node->conditionStmt.falseBody != NULL) {
        ClmScope *false_scope =
            clm_scope_find_child(scope, node->conditionStmt.falseBody);
        check_returns(node->conditionStmt.falseBody, false_scope);
      }
      break;
    }
    case STMT_TYPE_FUNC_DEC: {
      ClmScope *func_scope = clm_scope_find_child(scope, node);
      if (node->funcDecStmt.returnType != CLM_TYPE_NONE) {
        if (!check_function_returns(node->funcDecStmt.body, func_scope,
                                    node->funcDecStmt.returnType)) {
          // error... expected return but didn't find a valid one!
          clm_error(node->lineNo, node->colNo,
                    "Function %s does not have a valid return statement,"
                    " but expects a return of type %s",
                    node->funcDecStmt.name,
                    clm_type_to_string(node->funcDecStmt.returnType));
        }
      }
      break;
    }
    case STMT_TYPE_WHILE_LOOP:
      check_returns(node->whileLoopStmt.body, scope);
      break;
    case STMT_TYPE_FOR_LOOP:
      check_returns(node->forLoopStmt.body, scope);
      break;
    case STMT_TYPE_PRINT:
      break;
    case STMT_TYPE_RET:
      if (scope->parent == NULL) {
        // error... shouldn't have a return in global scope
        clm_error(node->lineNo, node->colNo,
                  "Global scope should have no returns");
      }
      break;
    }
  }
}

void clm_type_check_main(ArrayList *statements, ClmScope *globalScope) {
  type_check_stmts(statements, globalScope);
  check_returns(statements, globalScope);
}
