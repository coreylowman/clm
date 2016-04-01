#include <stdlib.h>
#include "clm_type_of.h"
#include "clm_statement.h"
#include "clm_type.h"
#include "clm_symbol.h"

ClmType clm_type_of_ind(ClmExpNode *node, ClmScope *scope) {
  ClmSymbol *symbol = clm_scope_find(scope, node->indExp->id);
  return symbol->type;
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
  case EXP_TYPE_ARITH:
    return clm_type_of_exp(node->arithExp->right, scope);
  case EXP_TYPE_BOOL:
    return clm_type_of_exp(node->boolExp->right, scope);
  case EXP_TYPE_CALL: {
    ClmSymbol *symbol = clm_scope_find(scope, node->callExp->name);
    ClmStmtNode *func_dec = symbol->declaration;
    return func_dec->funcDecStmt->returnType;
  }
  case EXP_TYPE_INDEX: {
    ClmSymbol *symbol = clm_scope_find(scope, node->indExp->id);
    if (node->indExp->rowIndex == NULL && node->indExp->colIndex == NULL) // A
      return symbol->type;
    else if (node->indExp->rowIndex == NULL) // A[#,x]
      return CLM_TYPE_MATRIX;
    else if (node->indExp->colIndex == NULL) // A[x,#]
      return CLM_TYPE_MATRIX;
    else // A[x,y]
      return CLM_TYPE_INT;
  }
  case EXP_TYPE_MAT_DEC:
    return CLM_TYPE_MATRIX;
  case EXP_TYPE_PARAM:
    return node->paramExp->type;
  case EXP_TYPE_UNARY:
    return clm_type_of_exp(node->unaryExp->node, scope);
  }
}

int clm_type_is_number(ClmType type) {
  return type == CLM_TYPE_INT || type == CLM_TYPE_FLOAT;
}

int clm_type_is_matrix(ClmType type) {
  return type == CLM_TYPE_MATRIX || type == CLM_TYPE_MATRIX_POINTER;
}