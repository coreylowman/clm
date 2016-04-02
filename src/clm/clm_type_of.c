#include <stdlib.h>

#include "clm_statement.h"
#include "clm_symbol.h"
#include "clm_type.h"
#include "clm_type_of.h"

ClmType clm_type_of_ind(ClmExpNode *node, ClmScope *scope) {
  ClmSymbol *symbol = clm_scope_find(scope, node->indExp.id);
  return symbol->type;
}

// string string         +
// string int
// string float
// string matrix
static ClmType type_of_arith_string_any(ArithOp op, ClmType right_type) {
  // only adding is defined for strings
  switch (right_type) {
  case CLM_TYPE_STRING:
    if (op == ARITH_OP_ADD) {
      return CLM_TYPE_STRING;
    } else {
      return CLM_TYPE_NONE;
    }
  default:
    return CLM_TYPE_NONE;
  }
}

// int op otherType
// int string
// int int               + - * /
// int float             + - * /
// int matrix                *
static ClmType type_of_arith_int_any(ArithOp op, ClmType right_type) {
  switch (right_type) {
  case CLM_TYPE_INT:
    return CLM_TYPE_INT;
  case CLM_TYPE_FLOAT:
    return CLM_TYPE_FLOAT;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT) {
      return CLM_TYPE_MATRIX;
    } else {
      return CLM_TYPE_NONE;
    }
  default:
    return CLM_TYPE_NONE;
  }
}

// float string
// float int             + - * /
// float float           + - * /
// float matrix              *
static ClmType type_of_arith_float_any(ArithOp op, ClmType right_type) {
  switch (right_type) {
  case CLM_TYPE_INT:
    return CLM_TYPE_FLOAT;
  case CLM_TYPE_FLOAT:
    return CLM_TYPE_FLOAT;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT) {
      return CLM_TYPE_MATRIX;
    } else {
      return CLM_TYPE_NONE;
    }
  default:
    return CLM_TYPE_NONE;
  }
}

// matrix string
// matrix int                * /
// matrix float              * /
// matrix matrix         + - * /
static ClmType type_of_arith_matrix_any(ArithOp op, ClmType right_type) {
  switch (right_type) {
  case CLM_TYPE_INT:
    switch (op) {
    case ARITH_OP_MULT:
    case ARITH_OP_DIV:
      return CLM_TYPE_MATRIX;
    default:
      return CLM_TYPE_NONE;
    }
  case CLM_TYPE_FLOAT:
    switch (op) {
    case ARITH_OP_MULT:
    case ARITH_OP_DIV:
      return CLM_TYPE_MATRIX;
    default:
      return CLM_TYPE_NONE;
    }
  case CLM_TYPE_MATRIX:
    return CLM_TYPE_MATRIX;
  default:
    return CLM_TYPE_NONE;
  }
}

ClmType clm_type_of_exp(ClmExpNode *node, ClmScope *scope) {
  if (node == NULL)
    return CLM_TYPE_NONE;
  switch (node->type) {
  case EXP_TYPE_INT:
    return CLM_TYPE_INT;
  case EXP_TYPE_FLOAT:
    return CLM_TYPE_FLOAT;
  case EXP_TYPE_STRING:
    return CLM_TYPE_STRING;
  case EXP_TYPE_ARITH: {
    ClmType right_type = clm_type_of_exp(node->arithExp.right, scope);
    ClmType left_type = clm_type_of_exp(node->arithExp.left, scope);
    switch (left_type) {
    case CLM_TYPE_STRING:
      return type_of_arith_string_any(node->arithExp.operand, right_type);
    case CLM_TYPE_INT:
      return type_of_arith_int_any(node->arithExp.operand, right_type);
    case CLM_TYPE_FLOAT:
      return type_of_arith_float_any(node->arithExp.operand, right_type);
    case CLM_TYPE_MATRIX:
      return type_of_arith_matrix_any(node->arithExp.operand, right_type);
    default:
      return CLM_TYPE_NONE;
    }
  }
  case EXP_TYPE_BOOL:
    return CLM_TYPE_INT;
  case EXP_TYPE_CALL: {
    ClmSymbol *symbol = clm_scope_find(scope, node->callExp.name);
    ClmStmtNode *func_dec = symbol->declaration;
    return func_dec->funcDecStmt.returnType;
  }
  case EXP_TYPE_INDEX: {
    ClmSymbol *symbol = clm_scope_find(scope, node->indExp.id);
    if (node->indExp.rowIndex == NULL && node->indExp.colIndex == NULL) // A
      return symbol->type;
    else if (node->indExp.rowIndex == NULL) // A[#,x]
      return CLM_TYPE_MATRIX;
    else if (node->indExp.colIndex == NULL) // A[x,#]
      return CLM_TYPE_MATRIX;
    else // A[x,y]
      return CLM_TYPE_INT;
  }
  case EXP_TYPE_MAT_DEC:
    return CLM_TYPE_MATRIX;
  case EXP_TYPE_PARAM:
    return node->paramExp.type;
  case EXP_TYPE_UNARY:
    return clm_type_of_exp(node->unaryExp.node, scope);
  default:
    return CLM_TYPE_NONE;
  }
}

int clm_type_is_number(ClmType type) {
  return type == CLM_TYPE_INT || type == CLM_TYPE_FLOAT;
}
