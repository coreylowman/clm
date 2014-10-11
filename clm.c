#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "gen.h"

int main(int argc,char *argv[]){
	char a;
	char str[] = "mytest4.clm";	
	PARSE_DATA *parseData;
	parseData = (PARSE_DATA *)parser_main(str);
	/*if(parseData->compile)
		gen_main(str,parseData->parseTree,parseData->symbol_table);*/
	free_parse_data(parseData);
	
	scanf("%c",&a);
	return 0;
}