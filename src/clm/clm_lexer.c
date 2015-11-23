#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clm_lexer.h"
#include "util/clm_error.h"
#include "util/clm_string.h"

typedef struct ClmLexerData {
    char *programString;
    int programLength;
    int curInd;
    int lineNo;
    int colNo;
    int numTokens;
    ClmArrayList *tokens; //array list of ClmLexerToken
} ClmLexerData;

#define tok_str_eq(x,s) clm_string_equals_n((x),(s),strlen(s))

static ClmLexerData data;

static ClmLexerToken *get_token();

static int is_pd(char c){
    return c == '.';
}

static int is_dig(char c){
    return c >= '0' && c <= '9';
}

static int is_letter(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_whitespace(char c){
    return c == '\t' || c == '\n' || c == '\r' || c == ' ';
}

static int is_quote(char c){
    return c == '"';
}

static int is_id_char(char c){
	return is_letter(c) || is_dig(c) || c == '_';
}

static char consume(){
    return data.programString[data.curInd++];
}

static char curr(){
    return data.programString[data.curInd];   
}

static char prev(){
    return data.programString[data.curInd - 1];
}

static void clm_rewind(){
    data.curInd--;
}

static int valid(){
    return data.curInd <= data.programLength;
}

static void lexer_token_free(void *data){
    if(data == NULL) return;
    ClmLexerToken *token = (ClmLexerToken *)data;
    free(token->raw);
    free(token);
}

static void lexer_token_print(void *data){
    ClmLexerToken *token = (ClmLexerToken *)data;
    printf("%s : { %s }\n",clm_lexer_sym_to_string(token->sym), token->raw);
}

void clm_lexer_print(ClmArrayList *tokens){
    clm_array_list_foreach(tokens,lexer_token_print);
}

ClmArrayList *clm_lexer_main(const char *fileContents){    
    data.curInd = 0;
    data.lineNo = 1;
    data.colNo = 0;
    data.numTokens = 0;
    data.programString = clm_string_copy(fileContents);
    data.tokens = clm_array_list_new(lexer_token_free);

    data.programLength = clm_string_length(fileContents);
    
    while (valid() && data.programString[data.curInd] != '\0'){
        ClmLexerToken *token = get_token();
        if(token != NULL)
            clm_array_list_push(data.tokens,token);
    }

    ClmLexerToken *end = malloc(sizeof(*end));
    end->sym = LEX_END;
    end->raw = clm_string_copy("end");
    end->lineNo = 0;
    end->colNo = 0;    
    clm_array_list_push(data.tokens,end);

    free(data.programString);

    return data.tokens;
}

static ClmLexerToken *get_token(){
    char c;
    ClmLexerToken *token = malloc(sizeof(*token));
    token->lineNo = data.lineNo;
    token->colNo = data.colNo;
	
	while (is_whitespace(consume()) && valid()) {
        c = prev();
        if(c == '\n' || c == '\r'){
            data.lineNo++;
            data.colNo = 0;
        }
        data.colNo++;
    }

	clm_rewind();

    int start = data.curInd;

    c = curr();

    if(is_dig(c) || is_pd(c)){
        int is_float = 0;

		while ((is_dig(consume()) || is_pd(prev())) && valid()){
            if(is_float && is_pd(prev())){
                clm_error(data.lineNo, data.colNo, "found multiple '.' in number declaration");
                return NULL;
            }
            is_float |= is_pd(prev());
        }
		clm_rewind();

        if(is_pd(prev()) && data.curInd == start + 1 ){
            clm_error(data.lineNo, data.colNo, "expected '.' to be used in number declaration");
            return NULL;
        }

        if(is_float)
            token->sym = LEX_FLOAT;
        else
            token->sym = LEX_INT;
    }else if(is_quote(c)){
        consume(); //eat the first quote
		while (!is_quote(consume()) && valid()); //eat everything thats not a quote & the last quote

        token->sym = LEX_STRING;        
    }else{
        char *word = data.programString + start;
        if(tok_str_eq(word,              "for")){
            token->sym = LEX_FOR;
            data.curInd += strlen("for");
        }else if(tok_str_eq(word,        "by")){
            token->sym = LEX_BY;
            data.curInd += strlen("by");
        }else if(tok_str_eq(word,        "do")){
            token->sym = LEX_DO;
            data.curInd += strlen("do");
        }else if(tok_str_eq(word,        "printl")){
            token->sym = LEX_PRINTL;
            data.curInd += strlen("printl");
        }else if(tok_str_eq(word,        "print")){
            token->sym = LEX_PRINT;
            data.curInd += strlen("print");
        }else if(tok_str_eq(word,        "if")){
            token->sym = LEX_IF;
            data.curInd += strlen("if");
        }else if(tok_str_eq(word,        "end")){
            token->sym = LEX_END;
            data.curInd += strlen("end");
        }else if(tok_str_eq(word,        "then")){
            token->sym = LEX_THEN;
            data.curInd += strlen("then");
        }else if(tok_str_eq(word,        "else")){
            token->sym = LEX_ELSE;
            data.curInd += strlen("else");
        }else if(tok_str_eq(word,        "int")){
            token->sym = LEX_INT_WORD;
            data.curInd += strlen("int");
        }else if(tok_str_eq(word,        "float")){
            token->sym = LEX_FLOAT_WORD;
            data.curInd += strlen("float");
        }else if(tok_str_eq(word,        "string")){
            token->sym = LEX_STRING_WORD;
            data.curInd += strlen("string");
        }else if(tok_str_eq(word,        "and")){
            token->sym = LEX_AND;
            data.curInd += strlen("and");
        }else if(tok_str_eq(word,        "or")){
            token->sym = LEX_OR;
            data.curInd += strlen("or");
        }else if(tok_str_eq(word,        "return")){
            token->sym = LEX_RETURN;
            data.curInd += strlen("return");
        }else if(tok_str_eq(word,        "call")){
            token->sym = LEX_CALL;
            data.curInd += strlen("call");
        }else if(tok_str_eq(word,        "to")){
            token->sym = LEX_TO;
            data.curInd += strlen("to");
        }else if(tok_str_eq(word,        "~")){
            token->sym = LEX_TILDA;
            data.curInd += strlen("~");
        }else if(tok_str_eq(word,        ".")){
            token->sym = LEX_PERIOD;
            data.curInd += strlen(".");
        }else if(tok_str_eq(word,        "\\")){
            token->sym = LEX_BACKSLASH;
            data.curInd += strlen("\\");
        }else if(tok_str_eq(word,        "[")){
            token->sym = LEX_LBRACK;
            data.curInd += strlen("[");
        }else if(tok_str_eq(word,        "]")){
            token->sym = LEX_RBRACK;
            data.curInd += strlen("]");
        }else if(tok_str_eq(word,        "{")){
            token->sym = LEX_LCURL;
            data.curInd += strlen("{");
        }else if(tok_str_eq(word,        "}")){
            token->sym = LEX_RCURL;
            data.curInd += strlen(")");
        }else if(tok_str_eq(word,        "(")){
            token->sym = LEX_LPAREN;
            data.curInd += strlen("(");
        }else if(tok_str_eq(word,        ")")){
            token->sym = LEX_RPAREN;
            data.curInd += strlen(")");
        }else if(tok_str_eq(word,        ",")){
            token->sym = LEX_COMMA;
            data.curInd += strlen(",");
        }else if(tok_str_eq(word,        ":")){
            token->sym = LEX_COLON;
            data.curInd += strlen(":");
        }else if(tok_str_eq(word,        ";")){
            token->sym = LEX_SEMI;
            data.curInd += strlen(";");
        }else if(tok_str_eq(word,        "*")){
            token->sym = LEX_MULT;
            data.curInd += strlen("*");
        }else if(tok_str_eq(word,        "@")){
            token->sym = LEX_AT;
            data.curInd += strlen("@");
        }else if(tok_str_eq(word,        "-")){
            token->sym = LEX_SUB;
            data.curInd += strlen("-");
        }else if(tok_str_eq(word,        "+")){
            token->sym = LEX_ADD;
            data.curInd += strlen("+");
        }else if(tok_str_eq(word,        ">=")){
            token->sym = LEX_GTE;
            data.curInd += strlen(">=");
        }else if(tok_str_eq(word,        ">")){
            token->sym = LEX_GT;
            data.curInd += strlen(">");
        }else if(tok_str_eq(word,        "<=")){
            token->sym = LEX_LTE;
            data.curInd += strlen("<=");
        }else if(tok_str_eq(word,        "<")){
            token->sym = LEX_LT;
            data.curInd += strlen("<");
        }else if(tok_str_eq(word,        "==")){
            token->sym = LEX_EQ;
            data.curInd += strlen("==");
        }else if(tok_str_eq(word,        "=")){
            token->sym = LEX_ASSIGN;
            data.curInd += strlen("=");
        }else if(tok_str_eq(word,        "!=")){
            token->sym = LEX_NEQ;
            data.curInd += strlen("!=");
        }else if(tok_str_eq(word,        "!")){
            token->sym = LEX_NOT;
            data.curInd += strlen("!");
        }else if(tok_str_eq(word,        "//")){
            while(consume() != '\n' && prev() != '\r');
            return NULL;
		}else if (tok_str_eq(word,		  "/")){
			token->sym = LEX_DIV;
			data.curInd += strlen("/");
		}else if (curr() == '\0'){
			data.curInd += 1;
			return NULL;
		}
		else{
			int can_be_id = 1;			
			//id must start with underscore or letter
            if(!(curr() == '_' || is_letter(curr())))
                can_be_id = 0;
			else{
				while (is_id_char(consume()) && valid());
				clm_rewind();
			}

            if(!can_be_id){
                clm_error(data.lineNo, data.colNo, "unknown symbol %s", token->raw);
                return NULL;
            }

            token->sym = LEX_ID;        
        }
    }
    token->raw = 
        clm_string_copy_n(data.programString + start, data.curInd - start);   
    return token;
}

const char *clm_lexer_sym_to_string(ClmLexerSymbol s){
    const char *ClmLexerSymbol_String[] = {
    "ADD", "AND", "ASSIGN", "AT", "BACKSLASH", "BY", "CALL", 
    "COLON", "COMMA", "DIV", "DO", "ELSE", "END", "EQ", "FLOAT",
    "FLOAT_WORD", "FOR", "GT", "GTE", "ID", "IF", "INT",
    "INT_WORD", "LBRACK", "LCURL", "LPAREN", "LT", "LTE", "MULT",
    "NEQ", "NOT", "OR", "PERIOD", "PRINT", "PRINTL", "QUESTION",
    "RBRACK", "RCURL", "RETURN", "RPAREN", "SEMI", "STRING",
    "STRING_WORD", "SUB", "THEN", "TILDA", "TO"
    };
    return ClmLexerSymbol_String[(int)s];
}