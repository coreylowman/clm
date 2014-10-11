#include "clm_errors.h"

char *symToString(Symbol s){
	switch(s){
	case SYM_ADD:
		return "+";
	case SYM_AND:
		return "and";
	case SYM_ASSIGN:
		return "=";
	case SYM_AT:
		return "@";
	case SYM_BACKSLASH:
		return "\\";
	case SYM_BY:
		return "by";
	case SYM_COLON:
		return ":";
	case SYM_COMMA:
		return ",";
	case SYM_CONSTANT:
		return "{int constant}";
	case SYM_DIV:
		return "/";
	case SYM_DO:
		return "do";
	case SYM_ELSE:
		return "else";
	case SYM_EQ:
		return "==";
	case SYM_FOR:
		return "for";
	case SYM_GT:
		return ">";
	case SYM_GTE:
		return ">=";
	case SYM_HASH:
		return "#";
	case SYM_ID:
		return "{id}";
	case SYM_IF:
		return "if";
	case SYM_INT:
		return "int";
	case SYM_LBRACK:
		return "[";
	case SYM_LPAREN:
		return "(";
	case SYM_LT:
		return "<";
	case SYM_LTE:
		return "<=";
	case SYM_MULT:
		return "*";
	case SYM_NEQ:
		return "!=";
	case SYM_NOT:
		return "!";
	case SYM_OR:
		return "or";
	case SYM_PERIOD:
		return ".";
	case SYM_PRINT:
		return "print";
	case SYM_QUESTION:
		return "?";
	case SYM_RBRACK:
		return "]";
	case SYM_RETURN:
		return "return";
	case SYM_RPAREN:
		return ")";
	case SYM_SEMI:
		return ";";
	case SYM_SUB:
		return "-";
	case SYM_THEN:
		return "then";
	default:
		return "err symbol";
	}
}