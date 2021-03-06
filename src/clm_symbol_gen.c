#include <stdlib.h>

#include "clm.h"
#include "clm_ast.h"
#include "clm_scope.h"
#include "clm_type.h"

static void gen_expnode_symbols(ClmScope *scope, ClmExpNode *node);
static void gen_statement_symbols(ClmScope *scope, ClmStmtNode *node);
static void gen_statements_symbols(ClmScope *scope, ArrayList *statements);
static void gen_symbol_offsets(ClmScope *scope);

static ClmSymbol *gen_new_sym(ClmScope *scope, const char *name, ClmType type,
                              void *declaration, int isParam) {
  ClmSymbol *symbol = clm_symbol_new(name, type, declaration);
  if (isParam) {
    // parameter to a function
    symbol->location = LOCATION_PARAMETER;
  } else if (scope->parent == NULL) {
    // global variable
    symbol->location = LOCATION_GLOBAL;
  } else {
    // local variable
    symbol->location = LOCATION_LOCAL;
    symbol->offset = clm_scope_next_local_offset(scope);
  }
  return symbol;
}

static void gen_expnode_symbols(ClmScope *scope, ClmExpNode *node) {
  switch (node->type) {
  case EXP_TYPE_INT:
  case EXP_TYPE_FLOAT:
  case EXP_TYPE_STRING:
    break;
  case EXP_TYPE_ARITH:
    gen_expnode_symbols(scope, node->arithExp.left);
    gen_expnode_symbols(scope, node->arithExp.right);
    break;
  case EXP_TYPE_BOOL:
    gen_expnode_symbols(scope, node->boolExp.left);
    gen_expnode_symbols(scope, node->boolExp.right);
    break;
  case EXP_TYPE_CALL: {
    int i;
    if (!clm_scope_contains(scope, node->callExp.name))
      clm_error(node->lineNo, node->colNo, "Use of undeclared function %s",
                node->callExp.name);

    for (i = 0; i < node->callExp.params->length; i++) {
      gen_expnode_symbols(scope, node->callExp.params->data[i]);
    }
    break;
  }
  case EXP_TYPE_INDEX:
    if (!clm_scope_contains(scope, node->indExp.id))
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                node->indExp.id);
    break;
  case EXP_TYPE_MAT_DEC: {
    if (node->matDecExp.size.rowVar != NULL &&
        !clm_scope_contains(scope, node->matDecExp.size.rowVar))
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                node->matDecExp.size.rowVar);
    if (node->matDecExp.size.colVar != NULL &&
        !clm_scope_contains(scope, node->matDecExp.size.colVar))
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                node->matDecExp.size.colVar);
    break;
  }
  case EXP_TYPE_PARAM: {
    if (node->paramExp.size.rowVar != NULL &&
        !clm_scope_contains(scope, node->paramExp.size.rowVar))
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                node->paramExp.size.rowVar);
    if (node->paramExp.size.colVar != NULL &&
        !clm_scope_contains(scope, node->paramExp.size.colVar))
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                node->paramExp.size.colVar);
    break;
  }
  case EXP_TYPE_UNARY:
    gen_expnode_symbols(scope, node->unaryExp.node);
    break;
  }
}

static void gen_statement_symbols(ClmScope *scope, ClmStmtNode *node) {
  switch (node->type) {
  case STMT_TYPE_ASSIGN: {
    ClmExpNode *lhs = node->assignStmt.lhs;
    ClmExpNode *rhs = node->assignStmt.rhs;
    ClmSymbol *symbol = clm_scope_find(scope, lhs->indExp.id);

    gen_expnode_symbols(scope, rhs);

    if (clm_exp_has_no_inds(lhs) && symbol == NULL) {
      clm_scope_push(scope, gen_new_sym(scope, lhs->indExp.id,
                                        clm_type_of_exp(rhs, scope), node, 0));
    } else if (symbol == NULL) {
      clm_error(node->lineNo, node->colNo, "Use of undeclared variable %s",
                lhs->indExp.id);
    }

    break;
  }
  case STMT_TYPE_CALL:
    gen_expnode_symbols(scope, node->callExpr);
    break;
  case STMT_TYPE_CONDITIONAL: {
    ArrayList *trueBody = node->conditionStmt.trueBody;
    ArrayList *falseBody = node->conditionStmt.falseBody;

    gen_expnode_symbols(scope, node->conditionStmt.condition);
    gen_statements_symbols(clm_scope_new(scope, trueBody), trueBody);
    if (node->conditionStmt.falseBody != NULL)
      gen_statements_symbols(clm_scope_new(scope, falseBody), falseBody);
    break;
  }
  case STMT_TYPE_FUNC_DEC: {
    ClmSymbol *symbol;
    ClmScope *functionScope = clm_scope_new(scope, node);
    if (node->funcDecStmt.parameters->length > 0) {
      int i;
      for (i = 0; i < node->funcDecStmt.parameters->length; i++) {
        ClmExpNode *param = node->funcDecStmt.parameters->data[i];
        symbol = gen_new_sym(functionScope, param->paramExp.name,
                             param->paramExp.type, param, 1);
        symbol->offset = i * 8 + 8;
        // framepointer + 8 is first param
        // matrices are passed as an address
        // val <- ebp + 20
        // type <- ebp + 16
        // val <- ebp + 12
        // type <- ebp + 8 //params
        // func:

        // i * 8 because each param takes up 2 places on the stack
        clm_scope_push(functionScope, symbol);
      }
    }
    gen_statements_symbols(functionScope, node->funcDecStmt.body);
    clm_scope_push(scope, gen_new_sym(scope, node->funcDecStmt.name,
                                      CLM_TYPE_FUNCTION, node, 0));
    break;
  }
  case STMT_TYPE_WHILE_LOOP:
    // todo should this generate a new scope?
    gen_expnode_symbols(scope, node->whileLoopStmt.condition);
    gen_statements_symbols(scope, node->whileLoopStmt.body);
    break;
  case STMT_TYPE_FOR_LOOP: {
    // todo shoudl this generate a new scope?
    if (!clm_scope_contains(scope, node->forLoopStmt.varId)) {
      clm_scope_push(scope, gen_new_sym(scope, node->forLoopStmt.varId,
                                        CLM_TYPE_INT, node, 0));
    }
    gen_expnode_symbols(scope, node->forLoopStmt.start);
    gen_expnode_symbols(scope, node->forLoopStmt.end);
    gen_expnode_symbols(scope, node->forLoopStmt.delta);
    gen_statements_symbols(scope, node->forLoopStmt.body);
    break;
  }
  case STMT_TYPE_PRINT:
    gen_expnode_symbols(scope, node->printStmt.expression);
    break;
  case STMT_TYPE_RET:
    gen_expnode_symbols(scope, node->returnExpr);
    break;
  }
}

static void gen_statements_symbols(ClmScope *scope, ArrayList *statements) {
  if (statements == NULL)
    return;
  int i;
  for (i = 0; i < statements->length; i++) {
    gen_statement_symbols(scope, statements->data[i]);
  }
}

ClmScope *clm_symbol_gen_main(ArrayList *statements) {
  ClmScope *globalScope = clm_scope_new(NULL, NULL);
  gen_statements_symbols(globalScope, statements);
  return globalScope;
}
