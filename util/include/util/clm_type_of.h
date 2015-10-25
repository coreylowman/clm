#ifndef CLM_TYPE_OF_H_
#define CLM_TYPE_OF_H_

#include "clm_type.h"
#include "clm_scope.h"
#include "clm_expression.h"

ClmType clm_type_of_exp(ClmExpNode *node, ClmScope *scope);
int clm_type_is_number(ClmType type);

#endif