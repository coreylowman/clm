#ifndef CLM_PARSER_H_
#define CLM_PARSER_H_

#include "util/clm_array_list.h"

ClmArrayList *clm_parser_main(ClmArrayList *tokens);
void clm_parser_print(ClmArrayList *parseTree);

#endif