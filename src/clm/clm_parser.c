#include "clm_lexer.h"
#include "clm_parser.h"
#include "util/clm_error.h"
#include "util/clm_expression.h"
#include "util/clm_statement.h"
#include "util/clm_string.h"

typedef struct ClmParserData {
    ClmArrayList *parseTree; //ClmArrayList of ClmStmtNode
    int curInd;
    int numTokens;
    ClmLexerToken **tokens; //pointer to ClmLexerData.tokens->data
    char *prevTokenRaw; //pointer to a token's raw str in ^
} ClmParserData;

static ClmParserData data;

static void consume(){
    data.prevTokenRaw = data.tokens[data.curInd++]->raw;
}

static ClmLexerToken *curr(){
    return data.tokens[data.curInd];   
}

static ClmLexerToken *prev(){
    return data.tokens[data.curInd - 1];
}

static ClmLexerToken *next(){
    return data.tokens[data.curInd + 1];
}

static ClmArrayList *consume_statements(int ifElse);
static ClmStmtNode *consume_statement();
static int consume_int();
static float consume_float();
static int consume_param_size();
static int consume_return_size();
static ClmExpNode *consume_parameter();
static ClmExpNode *consume_lhs();
static ClmExpNode *consume_expression();
static ClmExpNode *consume_expression_2();
static ClmExpNode *consume_expression_3();
static ClmExpNode *consume_expression_4();
static ClmExpNode *consume_expression_5();
static ClmExpNode *consume_expression_6();

static int accept(ClmLexerSymbol symbol){
	if (curr()->sym == symbol){
		consume();
		return 1;
	}
	return 0;
}

static int expect(ClmLexerSymbol symbol){
	if (accept(symbol)){
		return 1;
	}
    clm_error(curr()->lineNo, curr()->colNo,
              "Expected token %s, but found token %s",
              clm_lexer_sym_to_string(symbol),
              clm_lexer_sym_to_string(curr()->sym));
	return 0;
}

ClmArrayList *clm_parser_main(ClmArrayList *tokens){
    data.curInd = 0;
    data.numTokens = tokens->length;
    data.tokens = (ClmLexerToken **) tokens->data;
    return consume_statements(0);
}

void clm_parser_print(ClmArrayList *parseTree){
    clm_array_list_foreach_2(parseTree,0,clm_stmt_print);
}

static ClmArrayList *consume_statements(int ifElse){
    ClmArrayList *statements = clm_array_list_new(clm_stmt_free);

    //if we are parsing an if else and an else is next, we don't want to handle
    //that here... it is parsed in the consume_statement function
    //when we parse the actual if else
    while(!accept(LEX_END) && !(ifElse && curr()->sym == LEX_ELSE)){
        clm_array_list_push(statements,consume_statement());
    }

    return statements;
}

static int consume_int(){
    int val = 0;
    int neg = 0;
    if(accept(LEX_SUB))
        neg = 1;
    expect(LEX_INT);

    val = atoi(data.prevTokenRaw);
    return neg ? -val : val;
}

static float consume_float(){
    float val = 0;
    int neg = 0;
    if(accept(LEX_SUB)){
        neg = 1;
    }
    expect(LEX_FLOAT);
    val = atof(data.prevTokenRaw);
    return neg ? -val : val;
}

static int consume_param_size(){
    if(accept(LEX_INT)){
        return atoi(data.prevTokenRaw);
    }
    expect(LEX_ID);
    return 0;
}

static int consume_return_size(){
    if(accept(LEX_INT)){
        return atoi(data.prevTokenRaw);
    }
    expect(LEX_ID);
    return 0;
}

//id:{r:c}
//id:type
static ClmExpNode *consume_parameter(){
    ClmExpNode *node = NULL;
    if(accept(LEX_ID)){
        node = clm_exp_new_param(data.prevTokenRaw,CLM_TYPE_INT,1,1,NULL,NULL);
        node->lineNo = prev()->lineNo;
        node->colNo = prev()->colNo;
        expect(LEX_COLON);
        if(accept(LEX_LCURL)){
            node->paramExp->rows = consume_param_size();
            if(!node->paramExp->rows){
                node->paramExp->rowVar = clm_string_copy(data.prevTokenRaw);
            }
            expect(LEX_COLON);
            node->paramExp->cols = consume_param_size();
            if(!node->paramExp->cols){
                node->paramExp->colVar = clm_string_copy(data.prevTokenRaw);
            }
            expect(LEX_RCURL);
            node->paramExp->type = CLM_TYPE_MATRIX_POINTER;
        }else{
            if(accept(LEX_INT_WORD)){
                node->paramExp->type = CLM_TYPE_INT;
            }else if(accept(LEX_FLOAT_WORD)){
                node->paramExp->type = CLM_TYPE_FLOAT;
            }else if (accept(LEX_STRING_WORD)){
                node->paramExp->type = CLM_TYPE_STRING;
            }
        }
    }
    return node;
}

static ClmExpNode *consume_lhs(){
    int lineNo = curr()->lineNo, colNo = curr()->colNo;
    
    ClmExpNode *node = NULL;
    expect(LEX_ID);
    char *name = data.prevTokenRaw;

    ClmExpNode *rowIndex = NULL,*colIndex = NULL;
    if(accept(LEX_LBRACK)){
        //accepts A[x,y] A[,y] A[x,] A[,]
        if(next()->sym != LEX_COMMA)
            rowIndex = consume_expression();
        expect(LEX_COMMA);
        if(next()->sym != LEX_RBRACK)
            colIndex = consume_expression();
        expect(LEX_RBRACK);
    }

    node = clm_exp_new_index(name,rowIndex,colIndex);
    node->lineNo = lineNo;
    node->colNo = colNo;

    return node;
}

static ClmStmtNode *consume_statement(){
    int lineNo = curr()->lineNo, colNo = curr()->colNo;

    if(curr()->sym == LEX_ID){
        ClmExpNode *lhs = consume_lhs();
        expect(LEX_ASSIGN);
        ClmExpNode *rhs = consume_expression();
        
        ClmStmtNode *stmt = clm_stmt_new_assign(lhs,rhs);
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_CALL)){
        ClmStmtNode *stmt = clm_stmt_new_call(consume_expression());
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_PRINT)){
        ClmStmtNode *stmt = clm_stmt_new_print(consume_expression(),0);
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_PRINTL)){
        ClmStmtNode *stmt = clm_stmt_new_print(consume_expression(),1);
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_FOR)){            
        expect(LEX_ID);
        char *name = data.prevTokenRaw;        
        expect(LEX_ASSIGN);
        int startInclusive = 0;
        if(accept(LEX_LBRACK))
            startInclusive = 1;
        else expect(LEX_LPAREN);

        ClmExpNode *start = consume_expression();
        expect(LEX_COMMA);
        ClmExpNode *end = consume_expression();
        int endInclusive = 0;
        if(accept(LEX_RBRACK))
            endInclusive = 1;
        else expect(LEX_RPAREN);

        ClmExpNode *delta;
        if(accept(LEX_BY)){
            delta = consume_expression();
        }else{
            delta = clm_exp_new_int(1);
        }
        
        expect(LEX_DO);
        ClmArrayList *body = consume_statements(0);

        ClmStmtNode *stmt = clm_stmt_new_loop(name,start,end,delta,body, startInclusive, endInclusive);
        stmt->lineNo = lineNo; stmt->colNo = colNo;

        return stmt;
    }else if(accept(LEX_IF)){
        ClmExpNode *condition = consume_expression();
        expect(LEX_THEN);
        ClmArrayList *trueBody = consume_statements(1);

        ClmArrayList *falseBody = NULL;
        if(accept(LEX_ELSE)){
            falseBody = consume_statements(0);
        }

        ClmStmtNode *stmt = clm_stmt_new_cond(condition,trueBody,falseBody);
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_RETURN)){
        ClmStmtNode *stmt = clm_stmt_new_return(consume_expression());
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else if(accept(LEX_BACKSLASH)){                
        expect(LEX_ID);
        char *name = data.prevTokenRaw;

        ClmArrayList *params = clm_array_list_new(clm_exp_free);
        ClmExpNode *param = consume_parameter();
        while(param){
			clm_array_list_push(params, param);

			if (curr()->sym == LEX_SUB || curr()->sym == LEX_ASSIGN) break;
			else expect(LEX_COMMA);
            
            param = consume_parameter();
        }

        int r = -2, c = -2;
        char *rv = NULL, *cv = NULL;
        ClmType returnType = CLM_TYPE_NONE;
        if(accept(LEX_SUB)){
            expect(LEX_GT);
            if(accept(LEX_INT_WORD)){ //\x ... -> int
                returnType = CLM_TYPE_INT;
            }else if(accept(LEX_FLOAT_WORD)){ //\x ... -> float
                returnType = CLM_TYPE_FLOAT;
            }else if(accept(LEX_STRING_WORD)){ //\x ... -> string
                returnType = CLM_TYPE_STRING;
            }else {                         //\x ... -> [m:n]
                expect(LEX_LBRACK);
                r = consume_return_size();
                if(!r)
                    rv = data.prevTokenRaw;
                expect(LEX_COLON);
                c = consume_return_size();
                if(!c)
                    cv = data.prevTokenRaw;
                expect(LEX_RBRACK);

                returnType = CLM_TYPE_MATRIX;
            }
        }

        expect(LEX_ASSIGN);
        ClmArrayList *body = consume_statements(0);

        ClmStmtNode *stmt = clm_stmt_new_dec(name,params,returnType,r,rv,c,cv,body);
        stmt->lineNo = lineNo; stmt->colNo = colNo;
        return stmt;
    }else{
        clm_error(curr()->lineNo,curr()->colNo,
                  "Unexpected symbol %s",
                  clm_lexer_sym_to_string(curr()->sym));
    }
    return NULL;
}

static ClmExpNode *consume_expression(){
    //&& or ||
    ClmExpNode *node1,*node2;
    ClmBoolOp op;
    node1 = consume_expression_2();
    while(curr()->sym == LEX_AND || curr()->sym == LEX_OR){
        op = accept(LEX_AND) ? BOOL_OP_AND :
             BOOL_OP_OR, accept(LEX_OR);
        node2 = consume_expression_2();
        node1 = clm_exp_new_bool(op, node1, node2);
    }
    return node1;
}

static ClmExpNode *consume_expression_2(){
    //== or !=
    ClmExpNode *node1,*node2;
    ClmBoolOp op;
    node1 = consume_expression_3();
    while(curr()->sym == LEX_EQ || curr()->sym == LEX_NEQ){
        op = accept(LEX_EQ) ? BOOL_OP_EQ :
             BOOL_OP_NEQ, accept(LEX_NEQ);
        node2 = consume_expression_2();
        node1 = clm_exp_new_bool(op, node1, node2);
    }
    return node1;
}

static ClmExpNode *consume_expression_3(){
    //> or < or >= or <=
    ClmExpNode *node1;
    ClmExpNode *node2;
    ClmBoolOp op;
    node1 = consume_expression_4();
    while (curr()->sym == LEX_GT || curr()->sym == LEX_LT
        || curr()->sym == LEX_GTE || curr()->sym == LEX_LTE){
        op = accept(LEX_GT) ? BOOL_OP_GT : 
             accept(LEX_LT) ? BOOL_OP_LT :
             accept(LEX_GTE) ? BOOL_OP_GTE :
             BOOL_OP_LTE, accept(LEX_LTE);
        node2 = consume_expression_4();
        node1 = clm_exp_new_bool(op, node1, node2);
    }
    return node1;
}

static ClmExpNode *consume_expression_4(){
    //+ or -
    ClmExpNode *node1;
    ClmExpNode *node2;
    ClmArithOp op;
    node1 = consume_expression_5();
    while (curr()->sym == LEX_ADD || curr()->sym == LEX_SUB){
        op = accept(LEX_ADD) ? ARITH_OP_ADD :
             ARITH_OP_SUB, accept(LEX_SUB);
        node2 = consume_expression_5();
        node1 = clm_exp_new_arith(op, node1, node2);
    }
    return node1;
}

static ClmExpNode *consume_expression_5(){
    //* or /
    ClmExpNode *node1;
    ClmExpNode *node2;
    ClmArithOp op;
    node1 = consume_expression_6();
    while (curr()->sym == LEX_MULT || curr()->sym == LEX_DIV){
        op = accept(LEX_MULT) ? ARITH_OP_MULT :
             ARITH_OP_DIV, accept(LEX_DIV);
        node2 = consume_expression_6();
        node1 = clm_exp_new_arith(op, node1, node2);
    }
    return node1;
}

static ClmExpNode *consume_expression_6(){    
    int lineNo = curr()->lineNo, colNo = curr()->colNo;
    if (accept(LEX_LPAREN)){
        ClmExpNode *exp = consume_expression();
        expect(LEX_RPAREN);

        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (accept(LEX_INT)){
        ClmExpNode *exp = clm_exp_new_int(atoi(data.prevTokenRaw));
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (accept(LEX_FLOAT)){
        ClmExpNode *exp = clm_exp_new_float(atof(data.prevTokenRaw));
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if(accept(LEX_STRING)){
        ClmExpNode *exp = clm_exp_new_string(data.prevTokenRaw);
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (accept(LEX_SUB)){
        ClmExpNode *exp = clm_exp_new_unary(UNARY_OP_MINUS, consume_expression_6());
        return exp;
    }
    else if (accept(LEX_NOT)){
        ClmExpNode *exp = clm_exp_new_unary(UNARY_OP_NOT, consume_expression_6());
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (accept(LEX_TILDA)){
        ClmExpNode *exp = clm_exp_new_unary(UNARY_OP_TRANSPOSE, consume_expression_6());
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (curr()->sym == LEX_ID){   
        if (next()->sym == LEX_LPAREN){
            expect(LEX_ID);
            char *name = data.prevTokenRaw;
            expect(LEX_LPAREN);

            ClmArrayList *params = clm_array_list_new(clm_exp_free);
            while (curr()->sym != LEX_RPAREN){
                clm_array_list_push(params, consume_expression());
                if (accept(LEX_COMMA))
                    continue;
                else
                    break;
            }
            expect(LEX_RPAREN);

            ClmExpNode *exp = clm_exp_new_call(name, params);
            exp->lineNo = lineNo; exp->colNo = colNo;
            return exp;
        }
        else{
			ClmExpNode *exp = consume_lhs();
            exp->lineNo = lineNo; exp->colNo = colNo;
            return exp;
        }
    }
    else if (accept(LEX_LBRACK)){       
        int rows = 0, cols = 0;
        char *rowVar = NULL, *colVar = NULL;
        if (accept(LEX_ID)){
            rowVar = data.prevTokenRaw;
        }
        else{
            expect(LEX_INT);
            rows = atoi(data.prevTokenRaw);
        }
        expect(LEX_COLON);     
        if (accept(LEX_ID)){
            colVar = data.prevTokenRaw;
        }
        else{
            expect(LEX_INT);
            cols = atoi(data.prevTokenRaw);
        }
        expect(LEX_RBRACK);

        ClmExpNode *exp = clm_exp_new_empty_mat_dec(rows,cols,rowVar, colVar);
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else if (accept(LEX_RCURL)){
        int cols, num = 0;
        float *list;      
        int start = prev()->lineNo;       
        
        list = malloc(sizeof(*list));		
        if(curr()->sym == LEX_INT || (curr()->sym == LEX_SUB && next()->sym == LEX_INT))
            list[num++] = consume_int();
		else{
			list[num++] = consume_float();
		}

        //curr is int or float
        //or curr is negative and next is int or float
        while (curr()->sym == LEX_INT || curr()->sym == LEX_FLOAT
			|| (curr()->sym == LEX_SUB && (next()->sym == LEX_INT || next()->sym == LEX_FLOAT))){            
            list = realloc(list, (num + 1)*sizeof(*list));
			if (curr()->sym == LEX_INT)
                list[num++] = consume_int();
			else{
				list[num++] = consume_float();
			}
        }

        cols = num;
        while (accept(LEX_COMMA)){
			while (curr()->sym == LEX_INT || curr()->sym == LEX_FLOAT
				|| (curr()->sym == LEX_SUB && (next()->sym == LEX_INT || next()->sym == LEX_FLOAT))){
                list = realloc(list, (num + 1)*sizeof(*list));
				if (curr()->sym == LEX_INT)
                    list[num++] = consume_int();
				else{
					list[num++] = consume_float();
				}
            }
        }
        expect(LEX_RCURL);
        
        ClmExpNode *exp = clm_exp_new_mat_dec(list, num, cols);
        exp->lineNo = lineNo; exp->colNo = colNo;
        return exp;
    }
    else{
        clm_error(curr()->lineNo, curr()->colNo,
                  "Unexpected symbol %s",
                  clm_lexer_sym_to_string(curr()->sym));
    }

    //should never return from here... as it will error and exit
    return clm_exp_new_int(-1);
}