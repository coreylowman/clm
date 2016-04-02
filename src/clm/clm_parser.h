#ifndef CLM_PARSER_H_
#define CLM_PARSER_H_

#include "util/array_list.h"

ArrayList *clm_parser_main(ArrayList *tokens);
void clm_parser_print(ArrayList *parseTree);

#endif
