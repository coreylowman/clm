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
#include "util/clm_error.h"
#include "clm_lexer.h"
#include "clm_parser.h"
#include "clm_symbol_gen.h"
#include "clm_type_check.h"
#include "clm_optimizer.h"
#include "clm_code_gen.h"

char *fileName;
int CLM_BUILD_TESTS = 0;

int main(int argc,char *argv[]){
	fileName = clm_string_copy("test_indexing.clm");

    const char *fileContents = clm_file_get_contents(fileName);
	if (fileContents == NULL)
		clm_error(0, 0, "No file with name %s", fileName);

    ClmArrayList *tokens = clm_lexer_main(fileContents);
    //clm_lexer_print(tokens);
    
    ClmArrayList *parseTree = clm_parser_main(tokens);
    //clm_parser_print(parseTree);

    ClmScope *globalScope = clm_symbol_gen_main(parseTree);
	//clm_scope_print(globalScope, 0);

    clm_type_check_main(parseTree,globalScope);

	const char *asm_source = clm_code_gen_main(parseTree, globalScope);

	printf("\n%s\n", asm_source);


	free(fileName);
	free(fileContents);
    clm_array_list_free(tokens);
    clm_array_list_free(parseTree);
    clm_scope_free(globalScope);

    return 0;
}
