#include "clm.h"
#include "clm_ast.h"

// TODO
static void foldConstants(ClmExpNode *node, int *changed);

// TODO
static void foldMatrices(ClmExpNode *node, int *changed);

// TODO
static void reduceDoubleUnary(ClmExpNode *node, int *changed);

// TODO
static void reduceIdArithmetic(ClmExpNode *node, int *changed);

// TODO
static void eliminateDeadCode(ArrayList *statements, int *changed);

// TODO
static void reduceConditionals(ArrayList *statements, int *changed);

// TODO
void clm_optimizer_main(ArrayList *statements, ClmScope *globalScope) {}
