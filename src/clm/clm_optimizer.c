#include "clm_optimizer.h"
#include "util/clm_expression.h"

static void foldConstants(ClmExpNode *node, int *changed);
static void foldMatrices(ClmExpNode *node, int *changed);
static void reduceDoubleUnary(ClmExpNode *node, int *changed);
static void reduceIdArithmetic(ClmExpNode *node, int *changed);

static void eliminateDeadCode(ArrayList *statements, int *changed);
static void reduceConditionals(ArrayList *statements, int *changed);

void clm_optimizer_main(ArrayList *statements, ClmScope *globalScope);