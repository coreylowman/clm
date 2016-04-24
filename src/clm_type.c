#include <stdlib.h>
#include <string.h>

#include "clm_ast.h"
#include "clm_scope.h"
#include "clm_type.h"

const char *clm_type_to_string(ClmType type) {
  const char *strings[] = {
      "INT", "MATRIX", "MATRIX_POINTER", "STRING", "FLOAT", "FUNCTION", "NONE"};
  return strings[(int)type];
}

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

int clm_exp_has_size(ClmExpNode *node, ClmScope *scope) {
  int rows, cols;
  clm_size_of_exp(node, scope, &rows, &cols);
  return rows > 0 && cols > 0;
}

void size_of_arith_string_any(ArithOp op, ClmType right_type, int length,
                              int rlength, int *out_length) {
  *out_length = 0;
  switch (right_type) {
  case CLM_TYPE_STRING:
    if (op == ARITH_OP_ADD) {
      *out_length = length + rlength;
      return;
    }
  default:
    return;
  }
}

void size_of_arith_int_any(ArithOp op, ClmType right_type, int rrows, int rcols,
                           int *out_rows, int *out_cols) {
  *out_rows = 0;
  *out_cols = 0;
  switch (right_type) {
  case CLM_TYPE_INT:
  case CLM_TYPE_FLOAT:
    *out_rows = 1;
    *out_cols = 1;
    return;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT) {
      *out_rows = rrows;
      *out_cols = rcols;
      return;
    }
  default:
    return;
  }
}

void size_of_arith_float_any(ArithOp op, ClmType right_type, int rrows,
                             int rcols, int *out_rows, int *out_cols) {
  *out_rows = 0;
  *out_cols = 0;
  switch (right_type) {
  case CLM_TYPE_INT:
  case CLM_TYPE_FLOAT:
    *out_rows = 1;
    *out_cols = 1;
    return;
  case CLM_TYPE_MATRIX:
    if (op == ARITH_OP_MULT) {
      *out_rows = rrows;
      *out_cols = rcols;
      return;
    }
  default:
    return;
  }
}

void size_of_arith_matrix_any(ArithOp op, ClmType right_type, int rows,
                              int cols, int rrows, int rcols, int *out_rows,
                              int *out_cols) {
  *out_rows = 0;
  *out_cols = 0;
  switch (right_type) {
  case CLM_TYPE_INT:
    switch (op) {
    case ARITH_OP_MULT:
    case ARITH_OP_DIV:
      *out_rows = rows;
      *out_cols = cols;
      return;
    default:
      return;
    }
  case CLM_TYPE_FLOAT:
    switch (op) {
    case ARITH_OP_MULT:
    case ARITH_OP_DIV:
      *out_rows = rows;
      *out_cols = cols;
      return;
    default:
      return;
    }
  case CLM_TYPE_MATRIX:
    switch (op) {
    case ARITH_OP_MULT:
      *out_rows = rows;
      *out_cols = rcols;
      return;
    case ARITH_OP_DIV:
      return;
    case ARITH_OP_ADD:
    case ARITH_OP_SUB:
      *out_rows = rows;
      *out_cols = cols;
      return;
    }
    return;
  default:
    return;
  }
}

static void decompose_matrix_size(MatrixSize size, int *out_rows,
                                  int *out_cols) {
  *out_rows = size.rows;
  *out_cols = size.cols;
}

void clm_size_of_exp(ClmExpNode *node, ClmScope *scope, int *out_rows,
                     int *out_cols) {
  if (node == NULL) {
    return;
  }

  *out_rows = 0;
  *out_cols = 0;

  switch (node->type) {
  case EXP_TYPE_INT:
  case EXP_TYPE_FLOAT:
  case EXP_TYPE_BOOL:
    *out_rows = 1;
    *out_cols = 1;
    return;
  case EXP_TYPE_STRING:
    *out_rows = strlen(node->str);
    *out_cols = 1;
    return;
  case EXP_TYPE_ARITH: {
    int lrows, lcols;
    int rrows, rcols;

    ClmType right_type = clm_type_of_exp(node->arithExp.right, scope);
    ClmType left_type = clm_type_of_exp(node->arithExp.left, scope);

    clm_size_of_exp(node->arithExp.left, scope, &lrows, &lcols);
    clm_size_of_exp(node->arithExp.right, scope, &rrows, &rcols);

    switch (left_type) {
    case CLM_TYPE_STRING:
      size_of_arith_string_any(node->arithExp.operand, right_type, lrows, rrows,
                               out_rows);
      *out_cols = 1;
      return;
    case CLM_TYPE_INT:
      size_of_arith_int_any(node->arithExp.operand, right_type, rrows, rcols,
                            out_rows, out_cols);
      return;
    case CLM_TYPE_FLOAT:
      size_of_arith_float_any(node->arithExp.operand, right_type, rrows, rcols,
                              out_rows, out_cols);
      return;
    case CLM_TYPE_MATRIX:
      size_of_arith_matrix_any(node->arithExp.operand, right_type, lrows, lcols,
                               rrows, rcols, out_rows, out_cols);
      return;
    default:
      return;
    }

    return;
  }
  case EXP_TYPE_CALL: {
    ClmSymbol *symbol = clm_scope_find(scope, node->callExp.name);
    ClmStmtNode *func_dec = symbol->declaration;
    *out_rows = func_dec->funcDecStmt.returnSize.rows;
    *out_cols = func_dec->funcDecStmt.returnSize.cols;
    return;
  }
  case EXP_TYPE_INDEX: {
    ClmSymbol *symbol = clm_scope_find(scope, node->indExp.id);
    ClmStmtNode *declaration = symbol->declaration;
    clm_size_of_exp(declaration->assignStmt.rhs, scope, out_rows, out_cols);

    // note: if both are NULL, then we have the size of the whole matrix
    //      if only col is !NULL, then we are doing A[#,x], which is all rows 1
    //      col
    //      if only row is !NULL, then we are doing A[x,#], which is 1 row all
    //      cols
    //      if both are !NULL, then we are doing A[x,y], which is 1 row 1 col

    if (node->indExp.rowIndex != NULL)
      *out_rows = 1;

    if (node->indExp.colIndex != NULL)
      *out_cols = 1;

    return;
  }
  case EXP_TYPE_MAT_DEC:
    decompose_matrix_size(node->matDecExp.size, out_rows, out_cols);
    return;
  case EXP_TYPE_PARAM:
    decompose_matrix_size(node->paramExp.size, out_rows, out_cols);
    return;
  case EXP_TYPE_UNARY:
    clm_size_of_exp(node->unaryExp.node, scope, out_rows, out_cols);
    return;
  default:
    return;
  }
}
