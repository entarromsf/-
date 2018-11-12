#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
const int KEY_NUM = 15;
const int IDMAXLENGTH = 100000;


enum SymType
{
	SYM_CONST,//const
	SYM_DO,//do
	SYM_WHILE,//while
	SYM_IF,//if
	SYM_ELSE,//else
	SYM_INT,//int
	SYM_CHAR,//char
	SYM_SCANF,//scanf
	SYM_PRINTF,//printf
	SYM_MAIN,//main
	SYM_VOID,//void
	SYM_RETURN,//return
	SYM_SWITCH,//switch
	SYM_CASE,//case
	SYM_DEFAULT,//default


	SYM_IDENTIFIER,//标识符

	SYM_NUMBER,//数字
			   //特殊符号
			   SYM_PLUS,//+
			   SYM_MINUS,//-
			   SYM_TIMES,//*
			   SYM_SLASH,///
			   SYM_EQU,//==
			   SYM_NEQ,//!=
			   SYM_LES,//<
			   SYM_LEQ,//<=
			   SYM_GTR,//>
			   SYM_GEQ,//>=
			   SYM_LPAREN,//(
			   SYM_RPAREN,//)
			   SYM_LBRACK,//[
			   SYM_RBRACK,//]
			   SYM_LBRACE,//{
			   SYM_RBRACE,//}
			   SYM_COMMA,//,
			   SYM_SEMICOLON,//;
			   SYM_PERIOD,//.
			   SYM_BECOMES,//=
			   SYS_QUOTE,//'
			   SYS_DQUOTE,//"
};

int key_table[KEY_NUM] = { SYM_CONST,SYM_DO,SYM_WHILE,SYM_IF,SYM_ELSE,SYM_INT,SYM_CHAR,SYM_SCANF,SYM_PRINTF,SYM_MAIN,SYM_VOID,SYM_RETURN,SYM_SWITCH,SYM_CASE,SYM_DEFAULT};

char *SYM_NAME[] = {
	"CONST","DO","WHILE","IF","ELSE","INT","CHAR","SCANF","PRINTF","MAIN","VOID","RETURN","SWITCH","CASE","DEFAULT",
	"IDENTIFIER","NUMBER",
	""
	
};

char* key[KEY_NUM] =
{
	"const","do","while","if","else","int","char","scanf","printf","main","void","return","switch","case","default"
};