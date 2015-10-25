#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "util/clm_file.h"
#include "util/clm_array_list.h"
#include "util/clm_scope.h"
#include "util/clm_string.h"
#include "lexer/clm_lexer.h"
#include "parser/clm_parser.h"
#include "symbolGen/clm_symbol_gen.h"
#include "typeCheck/clm_type_check.h"
//#include "optimizer.h"
//#include "codeGen.h"

char *fileName;
int CLM_BUILD_TESTS = 0;

int main(int argc,char *argv[]){
	fileName = clm_string_copy("test_funcs.clm");

    const char *fileContents = clm_file_get_contents(fileName);
    
    ClmArrayList *tokens = clm_lexer_main(fileContents);
    //clm_lexer_print(tokens);
    
    ClmArrayList *parseTree = clm_parser_main(tokens);
    //clm_parser_print(parseTree);

    ClmScope *globalScope = clm_symbol_gen_main(parseTree);
	//clm_scope_print(globalScope, 0);

    clm_type_check_main(parseTree,globalScope);

	free(fileName);
	free(fileContents);
    clm_array_list_free(tokens);
    clm_array_list_free(parseTree);
    clm_scope_free(globalScope);

    return 0;
}
