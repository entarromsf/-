#include "global.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

FILE *infile, *outfile;
ofstream out,m_out;
ifstream m_in;
SymType sym;//储存字类型
SymType F_sym;//记录一行开头的类型(函数外)
SymType F_isym;//记录一行开头的类型(函数内)
char id[IDMAXLENGTH];//储存标识符
char F_id[IDMAXLENGTH];//记录一行开头的标识符(函数外)
char F_iid[IDMAXLENGTH];//记录一行开头的标识符(函数内)
char Str[STRMAXLENGTH];//储存字符串
int Num = 0;//储存数字
char ch = ' ';//读字符
int line = 0;//行数
int ll = 0;//一行的字符数
int cc = 0;//读字符指针
char cline[MAXLINE];//记录每次读入的一行
bool funbeginflag = false;
bool skipflag = false;
int errpos = 0;//出错时读到一行的字符位置

//词法分析开始
void getCh() {
//	ch = fgetc(infile);
	if (cc == ll) {
		ll = 0;
		cc = 0;
		if (errpos != 0) {
			if (skipflag) {
				endskip();
			}
			errpos = 0;
			fprintf(outfile, "%c", '\n');
		}
		for (int i = 0; i < MAXLINE; i++) {
			cline[i] = '\0';
		}
		fgets(cline, MAXLINE, infile);
		do {
			cout << cline[ll];
			if (cline[ll] == '\n') {
				cline[ll] = ' ';
			}
			if (cline[ll] != '\0') {
				ll++;
			}
			else
			{
				break;
			}
		} while (1);
		line++;
		if (feof(infile)) {
			cline[ll] = ' ';
			cline[ll + 1] = EOF;
		}
	}//读一行
	ch = cline[cc];
	cc++;
}
int key_match(char inword[]) {
	int temp = 0;
	while (temp < KEY_NUM) {
		if (strcmp(inword, key[temp]) == 0)
			return temp;
		temp++;
	}
	return -1;
}
int getSYM() {//取一个词
	Num = 0;
	char inword[IDMAXLENGTH];
	//初始化标识符数组
//	cout << "getsym" << endl;
	for (int i = 0; i < IDMAXLENGTH; i++) {
		id[i] = '\0';
	}
	//处理空白和换行
	while (ch == ' ' || ch == '\t') {
		getCh();
	}
	//程序完
	if (ch == EOF)
		return 0;
	//识别标识符
	if (isalpha(ch) || ch == '_') {
		int idl = 0;
		do {
			if (ch >= 'A' && ch <= 'Z') {
				ch = ch + 'a' - 'A';
			}
			inword[idl] = ch;
			getCh();

			idl++;
		} while (isalpha(ch) || ch == '_' || isdigit(ch));
		inword[idl] = '\0';
		int match = key_match(inword);
		if (match != -1) {
			sym = key_table[match];
			strcpy(id, inword);
		}
		else
		{
			strcpy(id, inword);
			sym = SYM_IDENTIFIER;
		}
		return 1;
	}
	//数字
	else if (isdigit(ch))
	{
		int numl = 0;
		sym = SYM_NUMBER;
		do {
			Num = 10 * Num + ch - '0';
			id[numl] = ch;
			numl++;
			getCh();
		} while (isdigit(ch));
		//可以限制位数
		//if (numl > 100000000){}
		return 2;
	}
	//字符3
	else if (ch == '\'') {
		sym = SYM_ZF;
		getCh();
		id[0] = '\'';
		if (!isalpha(ch) && !isdigit(ch) &&
			ch != '+' && ch != '-' && ch != '*'
			&& ch != '/' && ch != '_') {
			printf("%c is not char", ch);
			error(1);
			id[1] = ch;
			id[2] = '\'';
			Num = 0;
		}
		else if (ch == '\'') {
			printf("字符为空");
			error(1);
			id[1] = '\'';
			getCh();
			Num = 0;
			return 3;
		}
		else {
			Num = ch;
			id[1] = ch;
			id[2] = '\'';
		}
		
		do {
			getCh();
			if (ch != '\'') {
				error(3);
				break;
			}
		} while (ch != '\'');
		getCh();
		return 3;
	}
	//字符串4
	else if (ch == '\"') {
		sym = SYM_STRING;
		int i = 0;
		do {
			getCh();

			if (ch != '\"') {
				if (ch < 32 || ch >126) {
					printf("%c can not exist in string", ch);
					error(2);
				}
				else {
					id[i] = ch;
					i++;
				}
			}
			else {
				//id[i] = ch;
			}
		} while (ch != '\"');
		getCh();
		return 4;
	}
	else if (ch == '+') {
		id[0] = '+';
		sym = SYM_PLUS;
		getCh();
	}
	else if (ch == '-') {
		id[0] = '-';
		sym = SYM_MINUS;
		getCh();
	}
	else if (ch == '*') {
		id[0] = '*';
		sym = SYM_TIMES;
		getCh();
	}
	else if (ch == '/') {
		id[0] = '/';
		sym = SYM_SLASH;
		getCh();
	}
	else if (ch == '=') {
		getCh();
		if (ch == '=') {
			sym = SYM_EQU;
			id[0] = '=';
			id[1] = '=';
			getCh();

		}
		else
		{
			sym = SYM_BECOMES;
			id[0] = '=';
		}
	}
	else if (ch == '!') {
		getCh();
		if (ch == '=') {
			sym = SYM_NEQ;
			id[0] = '!';
			id[1] = '=';
			getCh();

		}
		else
		{
			sym = SYM_NUL;
			id[0] = '!';
			error(4);
		}
	}
	else if (ch == '<') {
		getCh();
		if (ch == '=') {
			sym = SYM_LEQ;
			id[0] = '<';
			id[1] = '=';
			getCh();

		}
		else
		{
			sym = SYM_LES;
			id[0] = '<';
		}
	}
	else if (ch == '>') {
		getCh();
		if (ch == '=') {
			sym = SYM_GEQ;
			id[0] = '>';
			id[1] = '=';
			getCh();

		}
		else
		{
			sym = SYM_GTR;
			id[0] = '>';
		}
	}
	else if (ch == '(') {
		id[0] = '(';
		sym = SYM_LPAREN;
		getCh();
	}
	else if (ch == ')') {
		id[0] = ')';
		sym = SYM_RPAREN;
		getCh();
	}
	else if (ch == '[') {
		id[0] = '[';
		sym = SYM_LBRACK;
		getCh();
	}
	else if (ch == ']') {
		id[0] = ']';
		sym = SYM_RBRACK;
		getCh();
	}
	else if (ch == '{') {
		id[0] = '{';
		sym = SYM_LBRACE;
		getCh();
	}
	else if (ch == '}') {
		id[0] = '}';
		sym = SYM_RBRACE;
		getCh();
	}
	else if (ch == ',') {
		id[0] = ',';
		sym = SYM_COMMA;
		getCh();
	}
	else if (ch == ':') {
		id[0] = ':';
		sym = SYM_COLON;
		getCh();
	}
	else if (ch == ';') {
		id[0] = ';';
		sym = SYM_SEMICOLON;
		getCh();
	}
	else if (ch == '.') {
		id[0] = '.';
		sym = SYM_PERIOD;
		getCh();
	}
	else {
		id[0] = ch;
		sym = SYM_NUL;
		error(0);
		getCh();
	}
	return 5;
}
//词法分析结束

//语法分析开始
void test(SymType typ_t, int n) {
	if (sym == typ_t) {
		getSYM();
	}
	else
	{
		error(n);
	}
}
void constdec() {
	while (sym == SYM_CONST) {//每次读一行const声明
		SymType line_const_type;
		char line_const_id[IDMAXLENGTH];
		int const_value;
		getSYM();//int
		line_const_type = sym;
		if (sym != SYM_INT && sym != SYM_CHAR) {
			error(6);
			skip2semi();//出错时跳过这行
			continue;
		}
		
		getSYM();//a
		strcpy(line_const_id, id);
		if (sym == SYM_IDENTIFIER) {
			//printf("声明常量%s", id);
			getSYM();//=
			if (sym != SYM_BECOMES) {
				if (sym == SYM_EQU) {
					getSYM();
					error(23);
					error(24);
				}
				else {
					skip2semi();
					error(23);
					continue;
				}
			}
			else {
				getSYM();//[+/-]或1234
			}
			if (sym == SYM_PLUS || sym == SYM_MINUS) {
				if (line_const_type == SYM_INT) {
					if (sym == SYM_PLUS) {//+
						getSYM();//1234
						if (sym == SYM_NUMBER) {
							const_value = Num;
							enterConst(line_const_type, line_const_id, const_value);
						}
						else {
							error(23);
							skip2semi();
							continue;
						}
					}
					else {//-
						getSYM();//1234
						if (sym == SYM_NUMBER) {
							const_value = -Num;
							enterConst(line_const_type, line_const_id, const_value);
						}
						else {
							error(23);
							skip2semi();
							continue;
						}
					}
				}
				else {
					error(25);
					skip2semi();
				}			
			}
			else if(sym ==SYM_NUMBER){
				if (line_const_type == SYM_INT) {
					const_value = Num;
					enterConst(line_const_type, line_const_id, const_value);
				}
				else {
					error(25);
					skip2semi();
					continue;
				}
			}
			else if (sym == SYM_ZF) {
				if (line_const_type == SYM_CHAR) {
					const_value = Num;
					enterConst(line_const_type, line_const_id, const_value);
				}
				else {
					error(25);
					skip2semi();
					continue;
				}
			}
			else {
				error(23);
				skip2semi();
				continue;
			}
			getSYM();//,
			if (Ftab_num != 0) {
				ftab[Ftab_num - 1].size++;
			}
			/////读完第一个常量
			while (sym == SYM_COMMA) {
				getSYM();//a
				//printf("声明常量%s", id);
				strcpy(line_const_id, id);
				getSYM();//=
				if (sym != SYM_BECOMES) {
					if (sym == SYM_EQU) {
						getSYM();
						error(23);
						error(24);
					}
					else {
						skip2semi();
						error(23);
						continue;
					}
				}
				else {
					getSYM();//[+/-]或1234
				}
				if (sym == SYM_PLUS || sym == SYM_MINUS) {
					if (line_const_type == SYM_INT) {
						if (sym == SYM_PLUS) {//+
							getSYM();//1234
							if (sym == SYM_NUMBER) {
								const_value = Num;
								enterConst(line_const_type, line_const_id, const_value);
							}
							else {
								error(23);
								skip2semi();
								continue;
							}
						}
						else {//-
							getSYM();//1234
							if (sym == SYM_NUMBER) {
								const_value = -Num;
								enterConst(line_const_type, line_const_id, const_value);
							}
							else {
								error(23);
								skip2semi();
								continue;
							}
						}
					}
					else {
						error(25);
						skip2semi();
						continue;
					}
				}
				else if (sym == SYM_NUMBER) {
					if (line_const_type == SYM_INT) {
						const_value = Num;
						enterConst(line_const_type, line_const_id, const_value);
					}
					else {
						error(25);
						skip2semi();
						continue;
					}
				}
				else if (sym == SYM_ZF) {
					if (line_const_type == SYM_CHAR) {
						const_value = Num;
						enterConst(line_const_type, line_const_id, const_value);
					}
					else {
						error(25);
						skip2semi();
						continue;
					}
				}
				else {
					error(23);
					skip2semi();
					continue;
				}
				if (Ftab_num != 0) {
					ftab[Ftab_num - 1].size++;
				}
				getSYM();//,
			}
			test(SYM_SEMICOLON, 5);
			//printf("一行常量声明");
		}
		else {
			error(23);
			skip2semi();
			continue;
		}	
		//fprintf(outfile, "一行常量声明");
	}
}
void enterConst(SymType const_typ_t, char const_id_t[], int value_t) {
	char numid[20];
	if (const_typ_t == SYM_INT) {
		enter(constant, inttyp, 0, const_id_t);
		tab[Tab_num].addr = value_t;
		itoa(value_t, numid, 10);
		buildOrder("const", "int", numid, const_id_t);
	}
	else {
		enter(constant, chartyp, 0, const_id_t);
		tab[Tab_num].addr = value_t;
		itoa(value_t, numid, 10);
		buildOrder("const","char",numid,const_id_t);
	}	
}
void enterVar(SymType var_typ_t, char var_id_t[]) {
	if (var_typ_t == SYM_INT) {
		enter(variable, inttyp, 0, var_id_t);
		buildOrder("int", "", "", var_id_t);
	}
	else {
		enter(variable, chartyp, 0, var_id_t);
		buildOrder("char", "", "", var_id_t);
	}
	ftab[Ftab_num - 1].size++;
}
void enterArray(SymType arry_typ_t,char array_id_t[]) {
	char array_num_str[20];
	enter(variable, arraytyp, 0, array_id_t);
	getSYM();//20
	if (sym == SYM_NUMBER) {
		enterAtab(arry_typ_t, Num);
		itoa(Num, array_num_str, 10);
		if (arry_typ_t == SYM_INT) {
			buildOrder("ints", "", array_num_str, array_id_t);
		}
		else {
			buildOrder("chars", "", array_num_str, array_id_t);
		}
		ftab[Ftab_num - 1].size += Num;
		getSYM();//]
	}
	else {
		error(26);
		enterAtab(arry_typ_t, 0);
		itoa(0, array_num_str, 10);
		if (arry_typ_t == SYM_INT) {
			buildOrder("ints", "", array_num_str, array_id_t);
		}
		else {
			buildOrder("chars", "", array_num_str, array_id_t);
		}
		getSYM();//]
	}
	if (sym == SYM_RBRACK) {
		getSYM();
	}
	else {
		error(28);
		skip2semi();
	}
	tab[Tab_num].ref = Atab_num;
}
void enterAtab(SymType array_typ_t, int array_num_t) {
	if (Atab_num == Amax) {
		exit(0);
	}
	Atab_num++;
	if (array_typ_t == SYM_INT) {
		atab[Atab_num].eletyp = inttyp;
		atab[Atab_num].elesize = 4;
	}
	else {
		atab[Atab_num].eletyp = chartyp;
		atab[Atab_num].elesize = 1;
	}
	atab[Atab_num].size = array_num_t;
}
void vardec(SymType typ_thisline,char firstid[]) {//处理一行变量声明
	char temp_id[IDMAXLENGTH];
	if (sym == SYM_SEMICOLON) {//;
		enterVar(typ_thisline, firstid);
		//cout << "变量" << firstid << endl;
	}
	else if (sym == SYM_COMMA) {//,
		//cout << "变量" << firstid << endl;
		enterVar(typ_thisline, firstid);
		while (sym == SYM_COMMA) {
			getSYM();//a
			if (sym == SYM_IDENTIFIER) {
				strcpy(temp_id, id);
				getSYM();
				if (sym == SYM_LBRACK) {
					enterArray(typ_thisline, temp_id);
					//cout << "数组" << temp_id << endl;
				}
				else if (sym == SYM_COMMA) {
					enterVar(typ_thisline,temp_id);
					//cout << "变量" << temp_id << endl;
				}
				else if (sym == SYM_SEMICOLON) {
					enterVar(typ_thisline, temp_id);
					//cout << "变量" << temp_id << endl;
				}
				else {
					error(29);
					enterVar(typ_thisline, temp_id);
					//cout << "变量" << temp_id << endl;
				}
			}
		}
	}
	else if (sym == SYM_LBRACK) {//[
		enterArray(typ_thisline, firstid);
		//cout << "数组" << firstid << endl;
		while (sym == SYM_COMMA) {
			getSYM();//a
			if (sym == SYM_IDENTIFIER) {
				strcpy(temp_id, id);
				getSYM();
				if (sym == SYM_LBRACK) {
					enterArray(typ_thisline, temp_id);
					//cout << "数组" << temp_id << endl;
				}
				else if (sym == SYM_COMMA) {
					enterVar(typ_thisline, temp_id);
					//cout << "变量" << temp_id << endl;
				}
				else if (sym == SYM_SEMICOLON) {
					enterVar(typ_thisline, temp_id);
					//cout << "变量" << temp_id << endl;
				}
				else {
					error(29);
					enterVar(typ_thisline, temp_id);
					//cout << "变量" << temp_id << endl;
				}
			}
		}
	}
	else {
		error(29);
		enterVar(typ_thisline, firstid);
		//cout << "变量" << firstid << endl;
	}
	test(SYM_SEMICOLON, 5);
}
void fundec(SymType pre_sym) {
	if (F_sym == SYM_INT) {
		enter(function, inttyp, 1, F_id);
		enterF(inttyp, Tab_num, F_id);
		buildOrder("func", "int", "", F_id);		
	}
	else if (F_sym == SYM_CHAR) {
		enter(function, chartyp, 1, F_id);
		enterF(chartyp, Tab_num, F_id);
		buildOrder("func", "char", "", F_id);
	}
	else if (F_sym == SYM_VOID) {
		enter(function, voidtyp, 1, F_id);
		enterF(voidtyp, Tab_num, F_id);
		buildOrder("func", "void", "", F_id);
	}
	
	if (pre_sym == SYM_LPAREN) {//(有参数
		getSYM();
		parameterlist();
		if (sym != SYM_LBRACE) {//{
			error(10);
			skip2cich();
		}
		else {//{
			getSYM();
		}
		if (sym == SYM_CONST) {
			constdec();
		}
		while (sym == SYM_INT || sym == SYM_CHAR) {
			F_isym = sym;//int
			getSYM();
			if (sym != SYM_IDENTIFIER) {
				error(8);
				skip2semi();
				getSYM();
			}
			else {
				strcpy(F_iid, id);
				getSYM();
				vardec(F_isym, F_iid);
			}
		}
		statementlist();
		if (sym == SYM_RBRACE) {
			//cout << "函数结束" << endl;
			getSYM();
		}
		else {
			error(19);
		}
	}
	else {//{无参数
		getSYM();
		if (sym == SYM_CONST) {
			constdec();
		}
		while (sym == SYM_INT || sym == SYM_CHAR) {
			F_isym = sym;//int
			getSYM();
			if (sym != SYM_IDENTIFIER) {
				error(8);
				skip2semi();
				getSYM();
			}
			else {
				strcpy(F_iid, id);
				getSYM();
				vardec(F_isym, F_iid);
			}
		}
		statementlist();
		if (sym == SYM_RBRACE) {
			//cout << "函数结束" << endl;
			getSYM();
		}
		else {
			error(19);
		}
	}
	buildOrder("end", "", "", F_id);
}
void parameterlist() {
	if (sym != SYM_INT && sym != SYM_CHAR) {
		error(9);
	}
	SymType temp;
	temp = sym;
	getSYM();
	if (sym != SYM_IDENTIFIER) {
		error(9);
		skip2lbrace();//跳过参数
	}
	else {
		//cout << "函数参数:" << id << endl;
		if (temp == SYM_INT) {
			enter(variable, inttyp, 0, id);
			buildOrder("para", "int", "", id);
		}
		else if (temp == SYM_CHAR) {
			enter(variable, chartyp, 0, id);
			buildOrder("para", "char", "", id);
		}
		//TODO:两个paranum能不能删一个
		tab[ftab[Ftab_num - 1].ref].parnum++;
		ftab[Ftab_num - 1].parnum++;
		ftab[Ftab_num - 1].size++;
		getSYM();
		if (sym == SYM_COMMA) {
			getSYM();
			parameterlist();
		}
		else if (sym == SYM_RPAREN) {//参数处理完
			getSYM();
			if (ftab[Ftab_num - 1].parnum == 0) {
				error(33);
			}
		}
		else {
			error(9);
			skip2lbrace();
		}
	}
}

void statementlist() {
	do {
		if (sym == SYM_RBRACE) {//}
			break;
		}
		else if (sym == SYM_DO || sym == SYM_IF || sym == SYM_SCANF || sym == SYM_PRINTF || sym == SYM_RETURN || sym == SYM_SWITCH || sym == SYM_LBRACE || sym == SYM_SEMICOLON || sym == SYM_IDENTIFIER) {
			statement();
		}
		else {
			error(11);
			skipstatementlist();
		}
	} while (true);
}
void statement() {
	int idpos;
	char para[IDMAXLENGTH][IDMAXLENGTH];
	int para_num = 0;
	///////////////////////////
	if (sym == SYM_DO) {
		dowhilestatement();
	}
	else if (sym == SYM_IF) {
		ifelsestatement();
	}
	else if (sym == SYM_SCANF) {
		scanfstatement();
		test(SYM_SEMICOLON, 5);
	}
	else if (sym == SYM_PRINTF) {
		printfstatement();
		test(SYM_SEMICOLON, 5);
	}
	else if (sym == SYM_RETURN) {
		returnstatement();
		test(SYM_SEMICOLON, 5);
	}
	else if (sym == SYM_SWITCH) {
		switchstatement();
	}
	else if (sym == SYM_LBRACE) {//{
		getSYM();
		statementlist();//}
		if (sym == SYM_RBRACE) {
			getSYM();
		}
		else {
			error(19);
		}
	}
	else if (sym == SYM_SEMICOLON) {
		//cout << "空语句" << endl;
		getSYM();
	}
	else if (sym == SYM_IDENTIFIER) {//函数调用,赋值语句
		idpos = findidpos(id);
		//if (sym == SYM_LPAREN) {
		//	if (sym == SYM_LPAREN) {//( 函数调用
		//		cout << "有参数函数调用" << endl;
		//		getSYM();
		//		v_para();//)
		//		getSYM();
		//	}
		//	else {
		//		error(12);
		//	}
		//}
		//else if (sym == SYM_LBRACK) {//[数组元素赋值		
		//	getSYM();
		//	expression();//]
		//	getSYM();//=
		//	if (sym == SYM_BECOMES) {
		//		cout << "数组元素赋值语句" << endl;
		//		getSYM();
		//		expression();
		//	}
		//	else {
		//		error(20);
		//	}
		//}
		//else if (sym == SYM_BECOMES) {
		//	cout << "赋值语句" << endl;
		//	getSYM();
		//	expression();
		//}
		//else if (sym == SYM_SEMICOLON) {
		//	cout << "无参数函数调用" << endl;
		//}
		//之后需要重构
		if (idpos == 0) {//标识符未定义
			error(30);
			skip2semi();
			return;
		}
		getSYM();
		if (tab[idpos].obj == constant) {
			error(38);
			skip2semi();
		}
		else if (tab[idpos].obj == variable) {
			assignstatement(idpos);
		}
		else if (tab[idpos].obj == function){
			if (sym == SYM_LPAREN) {
				getSYM();
				//cout << "有参数函数调用" << endl;
				para_num = v_para(para, idpos);
				if (sym == SYM_RPAREN) {
					getSYM();
					for (int i = 0; i < para_num; i++) {
						buildOrder("push", "", "", para[i]);
					}
					buildOrder("call", tab[idpos].name, "", "");
				}
				else {
					error(13);
					for (int i = 0; i < para_num; i++) {
						buildOrder("push", "", "", para[i]);
					}
					skip2semi();
				}
			}
			else if (sym == SYM_SEMICOLON) {
				//cout << "无参数函数调用" << endl;
				buildOrder("call", tab[idpos].name, "", "");
			}
		}
		test(SYM_SEMICOLON, 5);
	}
	else {
		error(11);
		skip2semi();
	}
	///////////////////////////
}
void dowhilestatement() {
	//cout << "dowhile语句开始{" << endl;
	char lab1[10];
	char lab2[10];
	strcpy(lab1, newlabel());
	buildOrder("lab:", "", "", lab1);
	getSYM();
	statement();
	if (sym == SYM_WHILE) {
		getSYM();//(
		if (sym == SYM_LPAREN) {
			getSYM();
			condition();
		}
		else {
			error(12);
		}
		if (sym == SYM_RPAREN) {
			getSYM();
		}
		else {
			error(13);
		}
		buildOrder("bz", "", "", lab1);
	}
	//cout << "}\ndowhile语句结束" << endl;
}
void ifelsestatement() {
	char lab1[10];//if语句开始
	char lab2[10];//else语句开始
	char lab3[10];//结尾
	//cout << "if语句开始" << endl;
	strcpy(lab1, newlabel());
	strcpy(lab2, newlabel());
	strcpy(lab3, newlabel());
	getSYM();
	if (sym == SYM_LPAREN) {
		getSYM();
		condition();
		if (sym == SYM_RPAREN) {
			buildOrder("bz", "", "", lab1);
			buildOrder("goto", "", "", lab2);
			buildOrder("lab:", "", "", lab1);
			getSYM();
			statement();
			buildOrder("goto", "", "", lab3);	
			buildOrder("lab:", "", "", lab2);
			if (sym == SYM_ELSE) {
				//cout << "else语句开始" << endl;
				getSYM();
				statement();
				buildOrder("lab:", "", "", lab3);
				//cout << "else语句结束" << endl;
			}
			else {
				error(14);
				skip2semi();
			}
		}
		else {
			error(13);
			skip2semi();
		}
	}
	else {
		error(12);
		skip2semi();
	}
}
void scanfstatement() {
	int idpos;
	//cout << "读语句开始" << endl;
	getSYM();//(
	if (sym == SYM_LPAREN) {
		getSYM();
		if (sym == SYM_IDENTIFIER) {
			//cout << "读入至" << id << endl;
			idpos = findidpos(id);
			if (idpos == 0) {//未找到标识符
				error(30);
				skip2semi();
				return;
			}
			else {
				if (tab[idpos].obj != variable) {
					error(36);
					skip2semi();
					return;
				}
				else {
					buildOrder("scf", "", "", id);
				}
			}
			getSYM();
			while (sym == SYM_COMMA) {
				getSYM();//IDENTIFIER
				if (sym != SYM_IDENTIFIER) {
					error(8);
					skip2semi();
					return;
				}
				//cout << "读入至" << id << endl;
				idpos = findidpos(id);
				if (idpos == 0) {//未找到标识符
					error(30);
					skip2semi();
					return;
				}
				else {
					if (tab[idpos].obj != variable) {
						error(36);
						skip2semi();
						return;
					}
					else {
						buildOrder("scf", "", "", id);
					}
				}
				getSYM();
			}
			if (sym == SYM_RPAREN) {
				getSYM();
			}
			else {
				error(13);
				skip2semi();
				return;
			}
		}
		else {
			error(8);
			skip2semi();
			return;
		}
	}
	else {
		error(12);
		skip2semi();
	}
	//cout << "读语句结束" << endl;
}
void printfstatement() {
	char var_1[IDMAXLENGTH];
	char var_2[IDMAXLENGTH];
	strcpy(var_1, "");
	strcpy(var_2, "");
	//cout << "写语句开始" << endl;
	getSYM();
	if (sym == SYM_LPAREN) {
		getSYM();
		if (sym == SYM_STRING) {
			//cout << "输出字符串" << id << endl;
			strcpy(var_1, id);
			getSYM();
			if (sym == SYM_COMMA) {
				getSYM();
				//cout << "输出表达式" << endl;
				endflag = false;
				expression();
				if (endflag) {
					skip2rparen();
				}
				strcpy(var_2, nowvar);
				if (sym == SYM_RPAREN) {
					getSYM();
				}
				else {
					error(13);
					skip2semi();
				}
			}
			else if (sym == SYM_RPAREN) {
				getSYM();
			}
			else {
				error(13);
				skip2semi();
			}
		}
		else {
			//cout << "输出表达式" << endl;
			endflag = false;
			expression();
			if (endflag) {
				skip2rparen();
			}
			strcpy(var_2, nowvar);
			if (sym == SYM_RPAREN) {
				getSYM();
			}
			else {
				error(13);
				skip2semi();
			}
		}
		buildOrder("prt", var_1, var_2, (print_lx == SYM_ZF) ? "char" : "int");
	}
	else {
		error(12);
		skip2semi();
	}
	//cout << "写语句结束" << endl;
}
void returnstatement() {
	char var_1[IDMAXLENGTH];
	//cout << "return语句开始" << endl;
	returnflag = true;
	getSYM();
	if (sym == SYM_LPAREN) {
		if (F_sym == SYM_VOID) {
			error(37);
		}
		getSYM();
		endflag = false;
		expression();//)
		if (endflag) {
			skip2rparen();
		}
		strcpy(var_1, nowvar);
		if (sym == SYM_RPAREN) {
			getSYM();
			buildOrder("ret", "", "", var_1);
		}
		else {
			error(13);
			skip2semi();
		}
	}
	else if (sym == SYM_SEMICOLON) {
		if (F_sym != SYM_VOID) {
			error(37);	
		}
		buildOrder("ret", "", "", "");
	}
	else {
		error(11);
		skip2semi();
	}
	//cout << "return语句结束" << endl;
}
void switchstatement() {
	char var_1[IDMAXLENGTH];
	//cout << "switch语句开始" << endl;
	getSYM();
	if (sym == SYM_LPAREN) {//(
		getSYM();
		//cout << "选择表达式" << endl;
		endflag = false;
		expression();
		strcpy(var_1, nowvar);
		if (endflag) {
			skip2rparen();
		}
		if (sym == SYM_RPAREN) {//)
			getSYM();
			if (sym == SYM_LBRACE) {//{
				getSYM();
				situation_table(var_1);//}
				if (sym == SYM_RBRACE) {
					getSYM();
				}
				else {
					error(19);
				}
			}
			else {
				error(10);
				skip2case();
				situation_table(var_1);//}
				if (sym == SYM_RBRACE) {
					getSYM();
				}
				else {
					error(19);
				}
			}
		}
		else {
			error(13);
		}
	}
	else {
		error(12);
		skip2lbrace();
	}
	//cout << "switch语句结束" << endl;
}
void situation_table(char casevar[]) {
	char labend[10];
	char labdefault[10];
	char labsitu[10];
	char num_str[IDMAXLENGTH];
	strcpy(labend, newlabel());
	if (sym == SYM_CASE) {
		//cout << "case情况开始" << endl;
		strcpy(labsitu, newlabel());
		getSYM();
		if (sym == SYM_ZF || sym == SYM_PLUS || sym  == SYM_MINUS || sym == SYM_NUMBER) {//整数或者字符
			if (sym == SYM_PLUS || sym == SYM_MINUS) {
				int sign = 0;
				if (sym == SYM_PLUS) {
					sign = 1;
				}
				else {
					sign = 0;
				}
				getSYM();
				if (sym == SYM_NUMBER) {
					if (sign == 0) {
						Num = -Num;
					}
					itoa(Num, num_str, 10);
					buildOrder("!=", casevar, num_str, "");
					getSYM();//:
					if (sym == SYM_COLON) {
						buildOrder("bz", "", "", labsitu);
						getSYM();
						statement();
						buildOrder("goto", "", "", labend);
						buildOrder("lab:", "", "", labsitu);
					}
					else {
						error(15);
						skip2case();
					}
				}
				else {
					error(16);
					skip2case();
				}
			}
			else {//字符和无符号整数
				itoa(Num, num_str, 10);
				buildOrder("!=", casevar, num_str, "");
				getSYM();//:
				if (sym == SYM_COLON) {
					buildOrder("bz", "", "", labsitu);
					getSYM();
					statement();
					buildOrder("goto", "", "", labend);
					buildOrder("lab:", "", "", labsitu);
				}
				else {
					error(15);
				}
			}	
		}
		//cout << "case情况结束" << endl;
	}
	else {
		error(17);
	}
	while (sym == SYM_CASE) {
		//cout << "case情况开始" << endl;
		strcpy(labsitu, newlabel());
		getSYM();
		if (sym == SYM_ZF || sym == SYM_PLUS || sym == SYM_MINUS || sym == SYM_NUMBER) {//整数或者字符
			if (sym == SYM_PLUS || sym == SYM_MINUS) {
				int sign = 0;
				if (sym == SYM_PLUS) {
					sign = 1;
				}
				else {
					sign = 0;
				}
				getSYM();
				if (sym == SYM_NUMBER) {
					if (sign == 0) {
						Num = -Num;
					}
					itoa(Num, num_str, 10);
					buildOrder("!=", casevar, num_str, "");
					getSYM();//:
					if (sym == SYM_COLON) {
						buildOrder("bz", "", "", labsitu);
						getSYM();
						statement();
						buildOrder("goto", "", "", labend);
						buildOrder("lab:", "", "", labsitu);
					}
					else {
						error(15);
						skip2case();
					}
				}
				else {
					error(16);
					skip2case();
				}
			}
			else {//字符和无符号整数
				itoa(Num, num_str, 10);
				buildOrder("!=", casevar, num_str, "");
				getSYM();//:
				if (sym == SYM_COLON) {
					buildOrder("bz", "", "", labsitu);
					getSYM();
					statement();
					buildOrder("goto", "", "", labend);
					buildOrder("lab:", "", "", labsitu);
				}
				else {
					error(15);
				}
			}
		}
		//cout << "case情况结束" << endl;
	}
	if (sym == SYM_DEFAULT) {
		//cout << "default开始" << endl;
		getSYM();
		if (sym == SYM_COLON) {
			getSYM();
			statement();
		}
		else {
			error(15);
		}
		buildOrder("lab:", "", "", labend);
		//cout << "default结束" << endl;
	}
	else {
		error(18);
	}
}
void assignstatement(int idpos) {
	char var_1[IDMAXLENGTH];
	char var_2[IDMAXLENGTH];
	char var_3[IDMAXLENGTH];
	strcpy(var_1, "");
	strcpy(var_2, "");
	strcpy(var_3, tab[idpos].name);

	if (sym == SYM_LBRACK) {//[数组元素赋值		
		getSYM();
		if (tab[idpos].typ != arraytyp) {//变量不为数组
			error(40);
			skip2semi();
			return;
		}
		endflag = false;
		expression();
		strcpy(var_2, nowvar);
		if (endflag) {
			skip2rbrack();
		}
		if (sym == SYM_RBRACK) {//]
			getSYM();//=
		}
		else {
			error(28);
		}
		if (is_num(nowvar)) {//检测数组越界
			if (tab[idpos].typ == arraytyp) {
				if (atab[tab[idpos].ref].size <= atoi(nowvar)) {
					error(31);
				}
			}
		}
		if (sym == SYM_BECOMES) {	
			getSYM();		
		}
		else {
			error(20);
			if (sym == SYM_EQU) {
				error(24);
				getSYM();
			}
			else {
				skip2semi();
				return;
			}
		}
		//cout << "数组元素赋值语句" << endl;
		endflag = false;
		expression();
		strcpy(var_1, nowvar);
		if (endflag) {
			skip2semi();
		}
		buildOrder("seta", var_1, var_2, var_3);
		SymType t_typ;
		if (atab[tab[idpos].ref].eletyp == inttyp) {
			t_typ = SYM_INT;
		}
		else if (atab[tab[idpos].ref].eletyp == chartyp) {
			t_typ = SYM_ZF;
		}
		if (t_typ != print_lx) {
			error(42);
		}
	}
	else if (sym == SYM_BECOMES) {
		SymType t_typ;
		//cout << "赋值语句" << endl;
		getSYM();
		endflag = false;
		expression();
		strcpy(var_1, nowvar);
		if (tab[idpos].typ == inttyp) {
			t_typ = SYM_INT;
		}
		else if (tab[idpos].typ == chartyp) {
			t_typ = SYM_ZF;
		}
		if (t_typ != print_lx) {
			error(42);
		}
		if (endflag) {
			skip2semi();
		}
		buildOrder("=", var_1, "", var_3);
	}
	else {
		error(39);
		skip2semi();
	}
}
void condition() {
	char var_1[IDMAXLENGTH];
	char var_2[IDMAXLENGTH];
	char var_3[IDMAXLENGTH];
	strcpy(var_1, "0");
	strcpy(var_2, "0");
	strcpy(var_3, "0");
	SymType op;
	SymType con_1, con_2;
	//cout << "条件开始" << endl;
	endflag = false;
	expression();
	if (endflag) {
		skip2rparen();
	}
	strcpy(var_1, nowvar);
	//条件类型判断
	con_1 = print_lx;
	if (sym == SYM_EQU || sym == SYM_NEQ || sym == SYM_LES || sym == SYM_LEQ || sym == SYM_GTR || sym == SYM_GEQ) {
		op = sym;
		getSYM();
		endflag = false;
		expression();
		if (endflag) {
			skip2rparen();
		}
		//条件类型判断
		if (print_lx != con_1) {
			error(43);
		}
		strcpy(var_2, nowvar);
		strcpy(var_3, newvar());
		if (op == SYM_EQU) {//==
			buildOrder("==", var_1, var_2, var_3);
		}
		else if (op == SYM_NEQ) {//!=
			buildOrder("!=", var_1, var_2, var_3);
		}
		else if (op == SYM_LES) {//<
			buildOrder("<", var_1, var_2, var_3);
		}
		else if (op == SYM_LEQ) {//<=
			buildOrder("<=", var_1, var_2, var_3);
		}
		else if (op == SYM_GTR) {//>
			buildOrder(">", var_1, var_2, var_3);
		}
		else if (op == SYM_GEQ) {//>=
			buildOrder(">=", var_1, var_2, var_3);
		}
		ftab[Ftab_num - 1].size++;
	}
	else {
		if (con_1 != SYM_INT) {
			error(43);
		}
		strcpy(var_3, newvar());
		buildOrder("!=", var_1, var_2, var_3);//var_1!=0
		ftab[Ftab_num - 1].size++;
	}
	strcpy(nowvar, var_3);
	//cout << "条件结束" << endl;
}
void expression() {
	char var_1[IDMAXLENGTH];
	char var_2[IDMAXLENGTH];
	char var_3[IDMAXLENGTH];
	strcpy(var_1, "0");
	strcpy(var_2, "0");
	strcpy(var_3, "0");
	do {
		if (sym == SYM_PLUS || sym == SYM_MINUS) {
			strcpy(var_1, var_3);
			if (sym == SYM_PLUS) {//+
				getSYM();
				term();
				strcpy(var_2, nowvar);
				strcpy(var_3, newvar());
				buildOrder("plus", var_1, var_2, var_3);
			}
			else {//-
				getSYM();
				term();
				strcpy(var_2, nowvar);
				strcpy(var_3, newvar());
				buildOrder("minus", var_1, var_2, var_3);
			}
			ftab[Ftab_num - 1].size++;
			print_lx = SYM_INT;
		}
		else {
			term();
			strcpy(var_3, nowvar);
		}
	} while (sym == SYM_PLUS || sym == SYM_MINUS);
	strcpy(nowvar, var_3);
}
void term() {
	char var_1[IDMAXLENGTH];
	char var_2[IDMAXLENGTH];
	char var_3[IDMAXLENGTH];
	factor();
	strcpy(var_3, nowvar);
	while (sym == SYM_TIMES || sym == SYM_SLASH) {
		strcpy(var_1, var_3);
		if (sym == SYM_TIMES) {//  *
			getSYM();
			factor();
			strcpy(var_2, nowvar);
			strcpy(var_3, newvar());
			buildOrder("time", var_1, var_2, var_3);
		}
		else {//  /
			getSYM();
			factor();
			strcpy(var_2, nowvar);
			strcpy(var_3, newvar());
			buildOrder("div", var_1, var_2, var_3);
		}
		ftab[Ftab_num - 1].size++;
		print_lx = SYM_INT;
	}
	strcpy(nowvar, var_3);
}
void factor() {
	int idpos;
	char id_t[IDMAXLENGTH];
	char para[IDMAXLENGTH][IDMAXLENGTH];
	int para_num = 0;
	print_lx = SYM_INT;
	if (sym == SYM_IDENTIFIER) {
		idpos = findidpos(id);
		if (idpos == 0) {//未找到标识符
			error(30);
			endflag = true;
			return;
		}
		getSYM();
		if (tab[idpos].obj == constant) {
			strcpy(nowvar, tab[idpos].name);
			if (tab[idpos].typ == inttyp) {
				print_lx = SYM_INT;
			}
			else {
				print_lx = SYM_ZF;
			}
		}
		else if (tab[idpos].obj == variable) {
			if (sym != SYM_LBRACK) {
				if (tab[idpos].typ == inttyp) {
					print_lx = SYM_INT;
				}
				else {
					print_lx = SYM_ZF;
				}
				strcpy(nowvar,tab[idpos].name);
			}
			else {//[数组
				getSYM();
				if (tab[idpos].typ != arraytyp) {
					error(40);
					endflag = true;
					return;
				}
				expression();
				if (endflag) {
					skip2rbrack();
				}
				if (sym == SYM_RBRACK) {//]
					getSYM();
				}
				else {
					error(28);
				}
				if (is_num(nowvar)) {//检测数组越界
					if (tab[idpos].typ == arraytyp) {
						if (atab[tab[idpos].ref].size <= atoi(nowvar)) {
							error(31);
						}
					}
				}
				strcpy(id_t, nowvar);
				strcpy(nowvar, newvar());
				ftab[Ftab_num - 1].size++;
				buildOrder("geta", tab[idpos].name, id_t, nowvar);
			}
		}
		else if (tab[idpos].obj == function) {
			if (tab[idpos].typ == voidtyp) {
				error(32);
			}
			if (sym == SYM_LPAREN) {//有参数
				getSYM();
				para_num = v_para(para,idpos);
				if (sym == SYM_RPAREN) {
					getSYM();
					for (int i = 0; i < para_num; i++) {
						buildOrder("push", "", "", para[i]);
					}
					strcpy(nowvar, newvar());
					ftab[Ftab_num - 1].size++;
					buildOrder("call", tab[idpos].name, "", nowvar);					
				}
				else {
					for (int i = 0; i < para_num; i++) {
						buildOrder("push", "", "", para[i]);
					}
					error(13);
					endflag = true;
					return;
				}
			}
			else {//无参数
				strcpy(nowvar, newvar());
				ftab[Ftab_num - 1].size++;
				buildOrder("call", tab[idpos].name, "", nowvar);
			}
			if (tab[idpos].typ == inttyp) {
				print_lx = SYM_INT;
			}
			else {
				print_lx = SYM_ZF;
			}
		}
		//if (sym == SYM_LBRACK) {//[
		//	getSYM();
		//	expression();//]
		//	getSYM();
		//}
		//else if (sym == SYM_LPAREN) {//( 有返回值函数调用
		//	getSYM();
		//	v_para();//)
		//	getSYM();
		//}
	}
	else if (sym == SYM_PLUS || sym == SYM_MINUS || sym == SYM_NUMBER) {
		print_lx = SYM_INT;
		if (sym == SYM_PLUS) {
			getSYM();//123
			if (sym == SYM_NUMBER) {
				itoa(Num, nowvar, 10);
				getSYM();
			}
			else {
				error(35);
			}			
		}
		else if (sym == SYM_MINUS) {
			getSYM();//123
			if (sym == SYM_NUMBER) {
				itoa(-Num, nowvar, 10);
				getSYM();
			}
			else {
				error(35);
			}
		}
		else {//number
			itoa(Num, nowvar, 10);
			getSYM();
		}
	}
	else if (sym == SYM_ZF) {
		print_lx = SYM_ZF;
		itoa(Num, nowvar, 10);
		getSYM();
	}
	else if (sym == SYM_LPAREN) {
		getSYM();
		endflag = false;
		expression();//)
		if (endflag) {
			skip2rparen();
		}
		if (sym != SYM_RPAREN) {
			endflag = true;
			error(13);
			return;
		}
		else {
			getSYM();
		}
		print_lx = SYM_INT;
	}	
	else {
		endflag = true;
		error(35);
	}
}
int v_para(char (*para)[IDMAXLENGTH],int idpos) {
	int para_num = 0;
	for (int i = 0; i < tab[idpos].parnum; i++) {
		endflag = false;
		expression();
		if (endflag) {
			skip2rparen();
			break;
		}
		strcpy(para[para_num++], nowvar);
		if (sym == SYM_COMMA) {
			getSYM();
		}
		else {
			if (i != tab[idpos].parnum - 1) {
				error(34);
				skip2rparen();
				break;
			}
		}
	}
	return para_num;
	/*expression();
	while (sym == SYM_COMMA) {
		expression();
	}*/
}

void block() {
	if (sym == SYM_CONST) {
		constdec();
	}
	if (sym != SYM_INT && sym != SYM_CHAR && sym != SYM_VOID) {
		error(7);
		skip2func();
	}
	while (sym == SYM_INT || sym == SYM_CHAR) {//函数或者变量
		F_sym = sym;//int/char
		getSYM();
		if (sym != SYM_IDENTIFIER) {
			error(8);
			skip2lbrace();//跳到{
		}
		else {
			strcpy(F_id, id);
			getSYM();
			if (sym == SYM_LPAREN) {//(
				//cout << "定义有参数函数:" << F_id << endl;
				funbeginflag = true;
				returnflag = false;
				fundec(SYM_LPAREN);
				if(!returnflag){
					error(41);
				}
			}
			else if (sym == SYM_LBRACE) {//{
				//cout << "定义无参数函数:" << F_id << endl;
				funbeginflag = true;
				returnflag = false;
				fundec(SYM_LBRACE);
				if (!returnflag) {
					error(41);
				}
			}
			else{//变量声明
				if (!funbeginflag) {
					vardec(F_sym, F_id);
				}
				else {
					error(21);
				}
			}
		}
	}
	if (sym != SYM_INT && sym != SYM_CHAR && sym != SYM_VOID) {
		error(7);
		skip2func();
	}
	while (sym == SYM_INT || sym == SYM_CHAR || sym == SYM_VOID) {//函数定义
		if (sym == SYM_INT || sym == SYM_CHAR) {
			F_sym = sym;
			getSYM();
			if (sym == SYM_IDENTIFIER) {
				strcpy(F_id, id);
				getSYM();
				if (sym == SYM_LPAREN) {//(
					//cout << "有参数函数" << F_id << "{" << endl;
					returnflag = false;
					fundec(SYM_LPAREN);
					if (!returnflag) {
						error(41);
					}
					//cout << "}有参数函数" << F_id << endl;
				}
				else if (sym == SYM_LBRACE) {//{
					//cout << "无参数函数" << F_id << "{" << endl;
					returnflag = false;
					fundec(SYM_LBRACE);
					if (!returnflag) {
						error(41);
					}
					//cout << "}无参数函数" << F_id << endl;
				}
			}
		}
		else if (sym == SYM_VOID) {
			F_sym = sym;
			getSYM();
			if (sym == SYM_IDENTIFIER) {
				strcpy(F_id, id);
				getSYM();
				if (sym == SYM_LPAREN) {//(
					//cout << "有参数void函数" << F_id << "{" << endl;
					fundec(SYM_LPAREN);
					//cout << "}有参数void函数" << F_id << endl;
				}
				else if (sym == SYM_LBRACE) {//{
					//cout << "无参数void函数" << F_id << "{" << endl;
					fundec(SYM_LBRACE);
					//cout << "}无参数void函数" << F_id << endl;
				}
			}
			else if (sym == SYM_MAIN) {
				//cout << "main函数{" << endl;
				strcpy(F_id, id);
				getSYM();//(
				getSYM();//)
				getSYM();//{
				if (sym == SYM_LBRACE) {
					fundec(SYM_LBRACE);
				}
				//cout << "}main函数" << endl;
				break;
			}
		}
	}
}
//语法分析结束

//生成代码开始
void buildOrder(char op_t[],char var1_t[],char var2_t[],char var3_t[]) {
	strcpy(order[Order_num].op, op_t);
	strcpy(order[Order_num].var1, var1_t);
	strcpy(order[Order_num].var2, var2_t);
	strcpy(order[Order_num].var3, var3_t);
	fprintf(outfile, "(%s,%s,%s,%s)\n", op_t, var1_t, var2_t, var3_t);
	//cout << "(" << op_t << "," << var1_t << "," << var2_t << "," << var3_t << ")" << endl;
	Order_num++;
}
char* newlabel() {
	char label[10];
	sprintf(label, "lab%d", label_num++);
	return label;
}
char* newvar() {
	char var[10];
	sprintf(var, "$%d", var_num++);
	return var;
}
int findidpos(char id_t[]) {
	strcpy(tab[0].name, id_t);
	int idpos = Tab_num;
	while (strcmp(tab[idpos].name, id_t) != 0) {
		idpos = tab[idpos].link;
	}
	if (idpos != 0) {//找到了表中的标识符
		return idpos;
	}
	else {
		int idposFlink = ftab[Ftab_num - 1].ref;
		while (strcmp(tab[idposFlink].name, id_t) != 0) {
			idposFlink = tab[idposFlink].link;
		}
		return idposFlink;
	}
}
void enter(OBJtyp obj_t,Types typ_t,int w_Fun,char name_t[]) {
	int lastsitu = 0;
	if (Tab_num == Tmax) {
		exit(0);
	}
	strcpy(tab[0].name, name_t);
	if (w_Fun) {
		if (Ftab_num > 0) {
			lastsitu = ftab[Ftab_num - 1].ref;
		}
		else {
			lastsitu = Tab_num;
		}
	}
	else {
		if (tab[Tab_num].obj == function) {
			lastsitu = 0;
		}
		else {
			lastsitu = Tab_num;
		}
	}
	int temp = lastsitu;
	while (strcmp(tab[lastsitu].name, name_t) != 0) {
		lastsitu = tab[lastsitu].link;
	}
	if (lastsitu == 0) {
		Tab_num++;
		strcpy(tab[Tab_num].name, name_t);
		tab[Tab_num].link = temp;
		tab[Tab_num].obj = obj_t;
		tab[Tab_num].typ = typ_t;
		tab[Tab_num].ref = 0;
		tab[Tab_num].normal = 0;
		tab[Tab_num].addr = 0;
	}
	else {//变量名重复
		error(22);
	}
}
void enterF(Types typ_t, int ref_t, char name_t[]) {
	ftab[Ftab_num].returntyp = typ_t;
	ftab[Ftab_num].ref = ref_t;
	strcpy(ftab[Ftab_num].name, name_t);
	ftab[Ftab_num].offset = 4;
	Ftab_num++;
}
bool is_num(char id_t[]) {
	if (isdigit(id_t[0]) || id_t[0] == '-' || id_t[0] == '+') {
		return 1;
	}
	else {
		return 0;
	}
}

//生成代码结束

//记录函数中的变量使用以便全局寄存器分配
int findFvarpos(char id_t[],int funcpos) {
	int i = 0;
	while (i < 8 && i < ftab[funcpos].var_num) {
		if (strcmp(id_t, ftab[funcpos].var_info[i].name) == 0) {
			return i;
		}
		i++;
	}
	return -1;
}
void func_var_usingrecord() {
	int funcpos;
	for (int i = 0; i < Order_num; i++) {
		if (strcmp("func", order[i].op) == 0) {//统计声明的局部变量
			funcpos = findfuncpos(order[i].var3);
			if (funcpos == Ftab_num - 1) {
				for (int j = ftab[funcpos].ref + 1 + tab[ftab[funcpos].ref].parnum; j < Tab_num; j++) {
					if (tab[j].typ == arraytyp) {
						continue;
					}
					else {
						int temp = ftab[funcpos].var_num;
						/*ftab[funcpos].var_info[temp].using_num++;*/
						strcpy(ftab[funcpos].var_info[temp].name, tab[j].name);
						ftab[funcpos].var_num++;
					}
				}
			}
			else {
				for (int j = ftab[funcpos].ref + 1 + tab[ftab[funcpos].ref].parnum; j < ftab[funcpos + 1].ref; j++) {
					if (tab[j].typ == arraytyp) {
						continue;
					}
					else {
						int temp = ftab[funcpos].var_num;
						/*ftab[funcpos].var_info[temp].using_num++;*/
						strcpy(ftab[funcpos].var_info[temp].name, tab[j].name);
						ftab[funcpos].var_num++;
					}
				}
			}
			i++;
			while (strcmp("end", order[i].op) != 0) {
				add_var_usingtime(order[i].var1, funcpos);
				add_var_usingtime(order[i].var2, funcpos);
				add_var_usingtime(order[i].var3, funcpos);
				i++;
			}
			sort_func_var_usingtime(funcpos);
		}
	}
}
void sort_func_var_usingtime(int funcpos) {
	//ftab[funcpos].var_info[i].using_num从大到小排序
	Var_info_tab temp;
	int n = ftab[funcpos].var_num;
	for (int i = 0; i < n; i++) {
		int key = i;
		for (int j = i + 1; j < n; j++) {
			if (ftab[funcpos].var_info[j].using_num > ftab[funcpos].var_info[key].using_num) {
				key = j;
			}
		}
		if (key != i) {
			temp = ftab[funcpos].var_info[key];
			ftab[funcpos].var_info[key] = ftab[funcpos].var_info[i];
			ftab[funcpos].var_info[i] = temp;
		}
	}
}
void add_var_usingtime(char id_t[], int funcpos) {
	for (int i = 0; i < ftab[funcpos].var_num; i++) {
		if (strcmp(id_t, ftab[funcpos].var_info[i].name) == 0) {
			ftab[funcpos].var_info[i].using_num++;
			return;
		}
	}
	if (id_t[0] == '$') {//临时变量
		int temp = ftab[funcpos].var_num;
		ftab[funcpos].var_info[temp].using_num++;
		strcpy(ftab[funcpos].var_info[temp].name, id_t);
		ftab[funcpos].var_num++;
		return;
	}
}
int findfuncpos(char id_t[]) {
	for (int i = 0; i < Order_num; i++) {
		if (strcmp(id_t, ftab[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

//记录结束

//错误处理开始
void error(int n) {//打印错误位置和编号
	if (errpos == 0) {
		printf("*");
	}
	if (cc > errpos) {
		for (int i = 3; i < cc - errpos; i++) {
			printf(" ");
		}
		printf("^");
		errpos = cc + 1;
	}
	printf("line%d,%d,erromsg:%s\n", line, cc, err_msg[n]);
}
void endskip() {
	while (errpos < cc) {
		fprintf(outfile, "%c", "_");
		errpos++;
	}
	skipflag = false;
}
void skip() {
	skipflag = true;
}
void skip2semi() {
	skipflag = true;
	while (sym != SYM_SEMICOLON) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2func() {
	skipflag = true;
	while (sym != SYM_VOID && sym != SYM_INT && sym != SYM_CHAR) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2lbrace() {
	skipflag = true;
	while (sym != SYM_LBRACE) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2case() {
	skipflag = true;
	while (sym != SYM_CASE && sym != SYM_RBRACE) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2rbrace() {
	skipflag = true;
	while (sym != SYM_RBRACE) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2cich() {
	skipflag = true;
	while (sym != SYM_CONST && sym != SYM_INT && sym != SYM_CHAR) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2rbrack() {
	skipflag = true;
	while (sym != SYM_RBRACK) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skip2rparen() {
	skipflag = true;
	while (sym != SYM_RPAREN) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
void skipstatementlist() {
	skipflag = true;
	while (sym != SYM_DO && sym != SYM_IF && sym != SYM_SCANF && sym != SYM_PRINTF && sym != SYM_RETURN && sym != SYM_SWITCH && sym != SYM_LBRACE && sym != SYM_SEMICOLON && sym != SYM_RBRACE)
	{
		getSYM();
	}
	if (sym == SYM_SEMICOLON) {
		getSYM();
	}
	if (skipflag) {
		endskip();
	}
}
//错误处理结束

//生成mips汇编开始
char* newstring() {
	char strlab[10];
	sprintf(strlab, "str%d", str_num++);
	return strlab;
}
void buildMIPS() {
	out << ".data" << endl;
	//用于字符串
	buildData();
	out << ".text" << endl;
	buildText();
	out << "#accomplish generate mips code." << endl;
}
void buildData() {
	int count;
	char lab[10];
	out << "ENTER: .asciiz \"\\n\"" << endl;
	out << "SPACE: .asciiz \" \"" << endl;
	for (count = 0; count < Order_num; count++) {
		if (strcmp("prt", order[count].op) == 0) {
			if (strcmp("", order[count].var1) != 0) {
				strcpy(lab, newstring());
				enterString(order[count].var1, lab);
				//输出转义后的string
				out << lab << ": .asciiz \"";
				for (int i = 0; order[count].var1[i] != '\0'; i++) {
					if (order[count].var1[i] != '\\') {
						out << order[count].var1[i];
					}
					else {
						out << "\\\\";
					}
				}
				out << "\"" << endl;
			}
		}
	}
}
void enterString(char str[],char lab[]) {
	strcpy(strtab[Strtab_num].string, str);
	strcpy(strtab[Strtab_num].label, lab);
	Strtab_num++;
}
void buildText() {
	int globalsize = 0;
	int g_v_n = 0;
	//统计定义的全局变量和常量
	while (strcmp("const", order[g_v_n].op) == 0) {
		g_v_n++;
		globalsize = globalsize + 4;
	}
	while (strcmp("int", order[g_v_n].op) == 0 || strcmp("char", order[g_v_n].op) == 0 || strcmp("ints", order[g_v_n].op) == 0 || strcmp("chars", order[g_v_n].op) == 0) {
		if (strcmp("ints", order[g_v_n].op) == 0 || strcmp("chars", order[g_v_n].op) == 0) {//数组
			globalsize = globalsize + 4 * atoi(order[g_v_n].var2);
			g_v_n++;
		}
		else {//变量
			globalsize = globalsize + 4;
			g_v_n++;
		}
	}
	//移动栈指针以$t9记录全局变量底部
	out << "addi $sp, $sp, " << -globalsize << endl;
	out << "addu $t9, $sp, $0" << endl;
	//生成mips信息表
	for (R_orpos = 0; R_orpos < Order_num; R_orpos++) {//遍历中间代码
		//常量
		while (strcmp("const", order[R_orpos].op) == 0) {
			buildMtab(order[R_orpos].var1, order[R_orpos].var3, true);
			sw_var(order[R_orpos].var2);
			R_orpos++;
		}
		//变量
		while (strcmp("int", order[R_orpos].op) == 0 || strcmp("ints", order[R_orpos].op) == 0 || strcmp("char", order[R_orpos].op) == 0 || strcmp("chars", order[R_orpos].op) == 0) {
			buildMtab(order[R_orpos].op, order[R_orpos].var3, true);
			if (strcmp("", order[R_orpos].var2) == 0) {
				build_offset = build_offset + 4;
			}
			else {//数组空间
				build_offset = build_offset + 4 * atoi(order[R_orpos].var2);
			}
			R_orpos++;
		}
		out << "j main" << endl;
		//函数定义
		while (strcmp("func", order[R_orpos].op) == 0) {
			char funcname[IDMAXLENGTH];			
			strcpy(funcname, order[R_orpos].var3);
			out << funcname << ":" << endl;
			if (strcmp("main", funcname) == 0) {
				main_flag = true;
			}
			R_orpos++;
			M_funcdec(funcname);
		}
	}
}
void buildMtab(char typ_t[],char id_t[],bool global) {
	if (strcmp("int", typ_t) == 0 || strcmp("ints", typ_t) == 0) {
		mtab[Mtab_num].typ = SYM_INT;
	}
	else if (strcmp("char", typ_t) == 0 || strcmp("chars", typ_t) == 0) {
		mtab[Mtab_num].typ = SYM_CHAR;
	}
	strcpy(mtab[Mtab_num].name, id_t);
	mtab[Mtab_num].global = global;
	mtab[Mtab_num].addr = build_offset;
	Mtab_num++;
}
int findMvarpos(char id_t[]) {
	for (int i = Mtab_num - 1; i >= 0; i--) {
		if (strcmp(id_t, mtab[i].name) == 0)
			return i;
	}
	return -1;
}
void sw_var(char num_t[]) {
	out << "li $t0, " << atoi(num_t) << endl;
	out << "sw $t0, " << build_offset << "($sp)" << endl;
	build_offset = build_offset + 4;
}
void M_funcdec(char funcname[]) {
	int reg_no;
	int Or_fb_pos = R_orpos;
	int Mtab_num_temp = Mtab_num;
	M_funcpos = findfuncpos(funcname);
	build_offset = 4;
	//入栈begin	
	out << "addi $sp, $sp, -40" << endl;
	out << "sw $ra, 4($sp)" << endl;
	out << "sw $t8, 8($sp)" << endl;
	/*out << "sw $s0 12($sp)" << endl;
	out << "sw $s1 16($sp)" << endl;
	out << "sw $s2 20($sp)" << endl;
	out << "sw $s3 24($sp)" << endl;
	out << "sw $s4 28($sp)" << endl;
	out << "sw $s5 32($sp)" << endl;
	out << "sw $s6 36($sp)" << endl;
	out << "sw $s7 40($sp)" << endl;*/
	for (int i = 0; i < 8; i++) {
		out << "sw $s" << i << ", " << 12 + i * 4 << "($sp)" << endl;
	}
	out << "addu $t8, $sp, $0" << endl;//$t8用于恢复上一层函数的栈
	out << "addi $sp, $sp, " << -4 * ftab[M_funcpos].size << endl;
	//入栈end
	//参数
	while (strcmp("para", order[R_orpos].op) == 0) {
		M_para();
		R_orpos++;
	}
	//常量定义
	while (strcmp("const", order[R_orpos].op) == 0) {
		buildMtab(order[R_orpos].var1, order[R_orpos].var3, false);
		reg_no = findFvarpos(order[R_orpos].var3, M_funcpos);
		if (reg_no != -1) {//存入s?号寄存器
			out << "li $s" << reg_no << ", " << atoi(order[R_orpos].var2) << endl;
			build_offset = build_offset + 4;
		}
		else {//存入内存
			sw_var(order[R_orpos].var2);
		}
		R_orpos++;
	}
	//变量定义
	while (strcmp("int", order[R_orpos].op) == 0 || strcmp("ints", order[R_orpos].op) == 0 || strcmp("char", order[R_orpos].op) == 0 || strcmp("chars", order[R_orpos].op) == 0) {
		buildMtab(order[R_orpos].op, order[R_orpos].var3, false);
		if (strcmp("", order[R_orpos].var2) == 0) {
			build_offset = build_offset + 4;
		}
		else {//数组空间
			build_offset = build_offset + 4 * atoi(order[R_orpos].var2);
		}
		R_orpos++;
	}
	//将中间变量加入mtab
	while (strcmp("end", order[Or_fb_pos].op) != 0) {
		//var1
		if (order[Or_fb_pos].var1[0] == '$' && findMvarpos(order[Or_fb_pos].var1) == -1) {
			buildMtab("int", order[Or_fb_pos].var1, false);
			build_offset = build_offset + 4;
		}
		//var2
		if (order[Or_fb_pos].var2[0] == '$' && findMvarpos(order[Or_fb_pos].var2) == -1) {
			buildMtab("int", order[Or_fb_pos].var2, false);
			build_offset = build_offset + 4;
		}
		//var3
		if (order[Or_fb_pos].var3[0] == '$' && findMvarpos(order[Or_fb_pos].var3) == -1) {
			if (strcmp("geta", order[Or_fb_pos].op) == 0) {
				if (mtab[findMvarpos(order[Or_fb_pos].var1)].typ == SYM_CHAR) {
					buildMtab("char", order[Or_fb_pos].var3, false);
				}
				else {
					buildMtab("int", order[Or_fb_pos].var3, false);
				}
			}
			else if (strcmp("call", order[Or_fb_pos].op) == 0) {
				if (ftab[findfuncpos(order[Or_fb_pos].var1)].returntyp == chartyp) {
					buildMtab("char", order[Or_fb_pos].var3, false);
				}
				else {
					buildMtab("int", order[Or_fb_pos].var3, false);
				}
			}
			else {
				buildMtab("int", order[Or_fb_pos].var3, false);
			}
			build_offset = build_offset + 4;
		}
		Or_fb_pos++;
	}
	//语句
	while (strcmp("end", order[R_orpos].op) != 0) {
		if (strcmp("plus", order[R_orpos].op) == 0) {
			M_plus();
		}
		else if (strcmp("minus", order[R_orpos].op) == 0) {
			M_minus();
		}
		else if (strcmp("time", order[R_orpos].op) == 0) {
			M_time();
		}
		else if (strcmp("div", order[R_orpos].op) == 0) {
			M_div();
		}
		else if (strcmp("==", order[R_orpos].op) == 0) {
			M_equ();
		}
		else if (strcmp("!=", order[R_orpos].op) == 0) {
			M_neq();
		}
		else if (strcmp(">", order[R_orpos].op) == 0) {
			M_gtr();
		}
		else if (strcmp(">=", order[R_orpos].op) == 0) {
			M_geq();
		}
		else if (strcmp("<", order[R_orpos].op) == 0) {
			M_les();
		}
		else if (strcmp("<=", order[R_orpos].op) == 0) {
			M_leq();
		}
		else if (strcmp("lab:", order[R_orpos].op) == 0) {
			M_lab();
		}
		else if (strcmp("goto", order[R_orpos].op) == 0) {
			M_goto();
		}
		else if (strcmp("=", order[R_orpos].op) == 0) {
			M_ass();
		}
		else if (strcmp("seta", order[R_orpos].op) == 0) {
			M_seta();
		}
		else if (strcmp("geta", order[R_orpos].op) == 0) {
			M_geta();
		}
		else if (strcmp("scf", order[R_orpos].op) == 0) {
			M_scf();
		}
		else if (strcmp("prt", order[R_orpos].op) == 0) {
			M_prt();
		}
		else if (strcmp("push", order[R_orpos].op) == 0) {
			M_push();
		}
		else if (strcmp("call", order[R_orpos].op) == 0) {
			M_call();
		}
		else if (strcmp("ret", order[R_orpos].op) == 0) {
			M_ret(funcname);
		}
		
		R_orpos++;
	}

	//出栈begin
	out << "end_" << funcname << ":" << endl;
	out << "addu $sp, $t8, $0" << endl;
	out << "lw $ra, 4($sp)" << endl;
	out << "lw $t8, 8($sp)" << endl;
	/*out << "lw $s0 12($sp)" << endl;
	out << "lw $s1 16($sp)" << endl;
	out << "lw $s2 20($sp)" << endl;
	out << "lw $s3 24($sp)" << endl;
	out << "lw $s4 28($sp)" << endl;
	out << "lw $s5 32($sp)" << endl;
	out << "lw $s6 36($sp)" << endl;
	out << "lw $s7 40($sp)" << endl;*/
	for (int i = 0; i < 8; i++) {
		out << "lw $s" << i << ", " << 12 + i * 4 << "($sp)" << endl;
	}
	out << "addi $sp, $sp, 40" << endl;
	//出栈end
	if (main_flag) {
		out << "li $v0, 10" << endl;
		out << "syscall" << endl;
	}
	else {
		out << "jr $ra" << endl;
	}
	Mtab_num = Mtab_num_temp;
	R_orpos++;
}
//mips单条指令翻译
void M_para() {
	buildMtab(order[R_orpos].var1, order[R_orpos].var3, 0);
	build_offset = build_offset + 4;
}
void M_plus() {
	//plus,var1,var2,var3 var3=var1+var2
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//var3
	if (reg3 != -1) {//使用s(reg3)号寄存器
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "addu $s" << reg3 << ", $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "addu $s" << reg3 << ", $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "addu $s" << reg3 << ", $t0" << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "addu $s" << reg3 << ", $t0, $t1" << endl;
			}
		}
	}
	else {//$t3
		M_varpos = findMvarpos(order[R_orpos].var3);
		bool w_glo = false;
		w_glo = mtab[M_varpos].global;
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "addu $t3," << " $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "addu $t3," << " $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "addu $t3," << " $t0," << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "addu $t3," << " $t0, $t1" << endl;
			}
		}
		//存内存
		if (w_glo) {
			out << "sw $t3, " << mtab[M_varpos].addr << "($t9)" << endl;
		}
		else {
			out << "sw $t3, " << mtab[M_varpos].addr << "($sp)" << endl;
		}
	}
}
void M_minus() {
	//minus,var1,var2,var3 var3=var1-var2
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//var3
	if (reg3 != -1) {//使用s(reg3)号寄存器
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "sub $s" << reg3 << ", $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "sub $s" << reg3 << ", $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "sub $s" << reg3 << ", $t0" << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "sub $s" << reg3 << ", $t0, $t1" << endl;
			}
		}
	}
	else {//$t3
		M_varpos = findMvarpos(order[R_orpos].var3);
		bool w_glo = false;
		w_glo = mtab[M_varpos].global;
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "sub $t3, $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "sub $t3, $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "sub $t3," << " $t0, $s" << reg2 << endl;
			}
			else {//$t1
				out << "sub $t3," << " $t0, $t1" << endl;
			}
		}
		//存内存
		if (w_glo) {
			out << "sw $t3, " << mtab[M_varpos].addr << "($t9)" << endl;
		}
		else {
			out << "sw $t3, " << mtab[M_varpos].addr << "($sp)" << endl;
		}
	}
}
void M_time() {
	//time,var1,var2,var3 var3=var1*var2
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//var3
	if (reg3 != -1) {//使用s(reg3)号寄存器
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "mul $s" << reg3 << ", $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "mul $s" << reg3 << ", $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "mul $s" << reg3 << ", $t0" << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "mul $s" << reg3 << ", $t0, $t1" << endl;
			}
		}
	}
	else {//$t3
		M_varpos = findMvarpos(order[R_orpos].var3);
		bool w_glo = false;
		w_glo = mtab[M_varpos].global;
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "mul $t3," << " $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "mul $t3," << " $s" << reg1 << ", $t1" << endl;
			}
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "mul $t3," << " $t0," << " $s" << reg2 << endl;
			}
			else {//$t1
				out << "mul $t3, $t0, $t1" << endl;
			}
		}
		//存内存
		if (w_glo) {
			out << "sw $t3, " << mtab[M_varpos].addr << "($t9)" << endl;
		}
		else {
			out << "sw $t3, " << mtab[M_varpos].addr << "($sp)" << endl;
		}
	}
}
void M_div() {
	//div,var1,var2,var3 var3=var1/var2
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//var3
	if (reg3 != -1) {//使用s(reg3)号寄存器
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "div $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "div $s" << reg1 << ", $t1" << endl;
			}
			out << "mflo $s" << reg3 << endl;
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "div $t0," << " $s" << reg2 << endl;
			}
			else {//$t1
				out << "div $t0, $t1" << endl;
			}
			out << "mflo $s" << reg3 << endl;
		}
	}
	else {//$t3
		M_varpos = findMvarpos(order[R_orpos].var3);
		bool w_glo = false;
		w_glo = mtab[M_varpos].global;
		if (reg1 != -1) {//使用s(reg1)
			if (reg2 != -1) {//使用s(reg2)
				out << "div $s" << reg1 << ", $s" << reg2 << endl;
			}
			else {//$t1
				out << "div $s" << reg1 << ", $t1" << endl;
			}
			out << "mflo $t3" << endl;
		}
		else {//$t0
			if (reg2 != -1) {//使用s(reg2)
				out << "div $t0," << " $s" << reg2 << endl;
			}
			else {//$t1
				out << "div $t0, $t1" << endl;
			}
			out << "mflo $t3" << endl;
		}
		//存内存
		if (w_glo) {
			out << "sw $t3, " << mtab[M_varpos].addr << "($t9)" << endl;
		}
		else {
			out << "sw $t3, " << mtab[M_varpos].addr << "($sp)" << endl;
		}
	}
}
void M_equ() {
	//==,var1,var2,var3 var3=(var1==var2)
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "sub $t2," << " $s" << reg1 << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "sub $t2," << " $s" << reg1 << ", $t1" << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "sub $t2," << " $t0," << " $s" << reg2 << endl;
		}
		else {//$t1
			out << "sub $t2, $t0, $t1" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "beq $t2, $0, " << lab_temp << endl;
}
void M_neq() {
	//!=,var1,var2,var3 var3=(var1!=var2)
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "sub $t2," << " $s" << reg1 << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "sub $t2," << " $s" << reg1 << ", $t1" << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "sub $t2," << " $t0" << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "sub $t2," << " $t0, $t1" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "bne $t2, $0, " << lab_temp << endl;
}
void M_gtr() {
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $s" << reg2 << ", $s" << reg1 << endl;
		}
		else {//$t1
			out << "slt $t2, $t1," << " $s" << reg1 << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $s" << reg2 << ", $t0" << endl;
		}
		else {//$t1
			out << "slt $t2, $t1, $t0" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "bne $t2, $0, " << lab_temp << endl;
}
void M_geq() {
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $s" << reg1 << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "slt $t2," << " $s" << reg1 << ", $t1" << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $t0" << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "slt $t2, $t0, $t1" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "beq $t2, $0, " << lab_temp << endl;
}
void M_les() {
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $s" << reg1 << ", $s" << reg2 << endl;
		}
		else {//$t1
			out << "slt $t2," << " $s" << reg1 << ", $t1" << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2," << " $t0, $s" << reg2 << endl;
		}
		else {//$t1
			out << "slt $t2, $t0, $t1" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "bne $t2, $0, " << lab_temp << endl;
}
void M_leq() {
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {	//使用$t1	
		if (reg2 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var2);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t1, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t1, " << atoi(order[R_orpos].var2) << endl;
	}
	//$t2
	if (reg1 != -1) {//使用s(reg1)
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2, $s" << reg2 << ", $s" << reg1 << endl;
		}
		else {//$t1
			out << "slt $t2, $t1, $s" << reg1 << endl;
		}
	}
	else {//$t0
		if (reg2 != -1) {//使用s(reg2)
			out << "slt $t2, $s" << reg2 << ", $t0" << endl;
		}
		else {//$t1
			out << "slt $t2, $t1, $t0" << endl;
		}
	}
	R_orpos++;
	//bz lab?
	char lab_temp[IDMAXLENGTH];
	strcpy(lab_temp, order[R_orpos].var3);
	out << "beq $t2, $0, " << lab_temp << endl;
}
void M_lab() {
	out << order[R_orpos].var3 << ":" << endl;
}
void M_goto() {
	out << "j " << order[R_orpos].var3 << endl;
}
void M_ass() {
	int reg1, reg2, reg3;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			M_varpos = findMvarpos(order[R_orpos].var1);
			bool w_glo = false;
			w_glo = mtab[M_varpos].global;
			if (w_glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var3
	if (reg3 != -1) {//使用s(reg3)号寄存器
		if (reg1 != -1) {//使用s(reg1)
			out << "addu $s" << reg3 << ", $s" << reg1 << ", $0" << endl;
		}
		else {//$t0
			out << "addu $s" << reg3 << ", $t0, $0" << endl;
		}
	}
	else {//$t3
		M_varpos = findMvarpos(order[R_orpos].var3);
		bool w_glo = false;
		w_glo = mtab[M_varpos].global;
		if (reg1 != -1) {//使用s(reg1)
			//存内存
			if (w_glo) {
				out << "sw $s" << reg1 << ", " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "sw $s" << reg1 << ", " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
		else {//$t0
			  //存内存
			if (w_glo) {
				out << "sw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "sw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}	
	}
}
void M_seta() {
	//seta var1,var2,var3 var3[var2]=var1
	int M_varpos_1, M_varpos_2, M_varpos_3;
	bool w_glo_1, w_glo_2, w_glo_3;
	int reg1, reg2;
	reg1 = findFvarpos(order[R_orpos].var1, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	M_varpos_1 = findMvarpos(order[R_orpos].var1);
	M_varpos_2 = findMvarpos(order[R_orpos].var2);
	M_varpos_3 = findMvarpos(order[R_orpos].var3);
	w_glo_1 = mtab[M_varpos_1].global;
	w_glo_2 = mtab[M_varpos_2].global;
	w_glo_3 = mtab[M_varpos_3].global;
	int array_addr = mtab[M_varpos_3].addr;
	//var1
	if (!is_num(order[R_orpos].var1)) {//使用$t0
		if (reg1 == -1) {
			if (w_glo_1) {
				out << "lw $t0, " << mtab[M_varpos_1].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos_1].addr << "($sp)" << endl;
			}
		}
	}
	else {
		out << "li $t0, " << atoi(order[R_orpos].var1) << endl;
	}
	//var2
	if (!is_num(order[R_orpos].var2)) {//$t1
		if (reg2 == -1) {
			if (w_glo_2) {
				out << "lw $t1, " << mtab[M_varpos_2].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos_2].addr << "($sp)" << endl;
			}
			out << "mul $t1, $t1, 4" << endl;
			out << "addi $t1, $t1, " << array_addr << endl;
		}
		else {
			out << "mul $t1, $s" << reg2 << ", 4" << endl;
			out << "addi $t1, $t1, " << array_addr << endl;
		}
		if (w_glo_3) {
			out << "addu $t1, $t1, $t9" << endl;
		}
		else {
			out << "addu $t1, $t1, $sp" << endl;
		}
		if (reg1 == -1) {//$t0
			out << "sw $t0, ($t1)" << endl;
		}
		else {
			out << "sw $s" << reg1 << ", ($t1)" << endl;
		}
	}
	else {
		array_addr = array_addr + 4 * atoi(order[R_orpos].var2);
		if (reg1 == -1) {//$t0
			if (w_glo_3) {
				out << "sw $t0, " << array_addr << "($t9)" << endl;
			}
			else {
				out << "sw $t0, " << array_addr << "($sp)" << endl;
			}
		}
		else {
			if (w_glo_3) {
				out << "sw $s" << reg1 << ", " << array_addr << "($t9)" << endl;
			}
			else {
				out << "sw $s" << reg1 << ", " << array_addr << "($sp)" << endl;
			}
		}
	}
}
void M_geta() {
	//geta var1,var2,var3 var3=var1[var2]
	int M_varpos_1, M_varpos_2, M_varpos_3;
	bool w_glo_1, w_glo_2, w_glo_3;
	int reg3, reg2;
	reg3 = findFvarpos(order[R_orpos].var3, M_funcpos);
	reg2 = findFvarpos(order[R_orpos].var2, M_funcpos);
	M_varpos_1 = findMvarpos(order[R_orpos].var1);
	M_varpos_2 = findMvarpos(order[R_orpos].var2);
	M_varpos_3 = findMvarpos(order[R_orpos].var3);
	w_glo_1 = mtab[M_varpos_1].global;
	w_glo_2 = mtab[M_varpos_2].global;
	w_glo_3 = mtab[M_varpos_3].global;
	int array_addr = mtab[M_varpos_1].addr;
	//var2
	if (!is_num(order[R_orpos].var2)) {//$t1 = (var2*4+addr)+stackbegin $t0 = var1[var2]
		if (reg2 == -1) {
			if (w_glo_2) {
				out << "lw $t1, " << mtab[M_varpos_2].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t1, " << mtab[M_varpos_2].addr << "($sp)" << endl;
			}
			out << "mul $t1, $t1, 4" << endl;
			out << "addi $t1, $t1, " << array_addr << endl;
		}
		else {
			out << "mul $t1, $s" << reg2 << ", 4" << endl;
			out << "addi $t1, $t1, " << array_addr << endl;
		}
		if (w_glo_1) {
			out << "addu $t1, $t1, $t9" << endl;
		}
		else {
			out << "addu $t1, $t1, $sp" << endl;
		}
		//set $t0 and assign var3
		if (reg3 == -1) {//$t0
			out << "lw $t0, ($t1)" << endl;
			if (w_glo_3) {
				out << "sw $t0, " << mtab[M_varpos_3].addr << "($t9)" << endl;
			}
			else {
				out << "sw $t0, " << mtab[M_varpos_3].addr << "($sp)" << endl;
			}
		}
		else {//$s?
			out << "lw $s" << reg3 << ", ($t1)" << endl;
		}
	}
	else {//var2 = 1234
		array_addr = array_addr + 4 * atoi(order[R_orpos].var2);
		if (reg3 == -1) {//$t0 = var1[var2]
			if (w_glo_1) {
				out << "lw $t0, " << array_addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << array_addr << "($sp)" << endl;
			}
			if (w_glo_3) {
				out << "sw $t0, " << mtab[M_varpos_3].addr << "($t9)" << endl;
			}
			else {
				out << "sw $t0, " << mtab[M_varpos_3].addr << "($sp)" << endl;
			}
		}
		else {//$s? = var1[var2]
			if (w_glo_1) {
				out << "lw $s" << reg3 << ", " << array_addr << "($t9)" << endl;
			}
			else {
				out << "lw $s" << reg3 << ", " << array_addr << "($sp)" << endl;
			}
		}
	}
}
void M_scf() {
	//scf, ,var3
	M_varpos = findMvarpos(order[R_orpos].var3);
	int reg = findFvarpos(order[R_orpos].var3, M_funcpos);	
	SymType var_typ = mtab[M_varpos].typ;
	int var_addr = mtab[M_varpos].addr;
	bool var_glo = mtab[M_varpos].global;
	if (var_typ == SYM_INT) {
		out << "li $v0, 5" << endl;
		out << "syscall" << endl;
		if (reg == -1) {//sw
			if (var_glo) {
				out << "sw $v0, " << var_addr << "($t9)" << endl;
			}
			else {
				out << "sw $v0, " << var_addr << "($sp)" << endl;
			}
		}
		else {//$s?
			out << "addu $s" << reg << ", $v0, $0" << endl;
		}
	}
	else {//char
		out << "li $v0, 12" << endl;
		out << "syscall" << endl;
		if (reg == -1) {//sw
			if (var_glo) {
				out << "sw $v0, " << var_addr << "($t9)" << endl;
			}
			else {
				out << "sw $v0, " << var_addr << "($sp)" << endl;
			}
		}
		else {//$s?
			out << "addu $s" << reg << ", $v0, $0" << endl;
		}
	}
}
void M_prt() {
	//prt,string,表达式计算结果,表达式输出形式(int/char)
	char str_lab[10];
	//处理string输出
	if (strcmp("", order[R_orpos].var1) != 0) {
		for (int i = 0; i < Strtab_num; i++) {
			if (strcmp(strtab[i].string, order[R_orpos].var1) == 0) {
				strcpy(str_lab, strtab[i].label);
				break;
			}
		}
		out << "la $a0, " << str_lab << endl;
		out << "li $v0, 4" << endl;
		out << "syscall" << endl;

		//换行
		out << "la $a0, ENTER"  << endl;
		out << "li $v0, 4" << endl;
		out << "syscall" << endl;
	}
	//处理表达式输出
	int reg;
	bool glo;
	if (strcmp("", order[R_orpos].var2) != 0) {
		if (strcmp("int", order[R_orpos].var3) == 0) {
			if (is_num(order[R_orpos].var2)) {//prt int num
				out << "li $a0, " << atoi(order[R_orpos].var2) << endl;
				out << "li $v0, 1" << endl;
				out << "syscall" << endl;
			}
			else {
				reg = findFvarpos(order[R_orpos].var2, M_funcpos);
				M_varpos = findMvarpos(order[R_orpos].var2);
				glo = mtab[M_varpos].global;
				if (reg == -1) {
					if (glo) {
						out << "lw $a0, " << mtab[M_varpos].addr << "($t9)" << endl;
						out << "li $v0, 1" << endl;
						out << "syscall" << endl;
					}
					else {
						out << "lw $a0, " << mtab[M_varpos].addr << "($sp)" << endl;
						out << "li $v0, 1" << endl;
						out << "syscall" << endl;
					}
				}
				else {//$s?
					out << "addu $a0, $s" << reg << ", $0" << endl;
					out << "li $v0, 1" << endl;
					out << "syscall" << endl;
				}
			}
		}
		else {//char
			if (is_num(order[R_orpos].var2)) {//prt char
				out << "li $a0, " << atoi(order[R_orpos].var2) << endl;
				out << "li $v0, 11" << endl;
				out << "syscall" << endl;
			}
			else {
				reg = findFvarpos(order[R_orpos].var2, M_funcpos);
				M_varpos = findMvarpos(order[R_orpos].var2);
				glo = mtab[M_varpos].global;
				if (reg == -1) {
					if (glo) {
						out << "lw $a0, " << mtab[M_varpos].addr << "($t9)" << endl;
						out << "li $v0, 11" << endl;
						out << "syscall" << endl;
					}
					else {
						out << "lw $a0, " << mtab[M_varpos].addr << "($sp)" << endl;
						out << "li $v0, 11" << endl;
						out << "syscall" << endl;
					}
				}
				else {//$s?
					out << "addu $a0, $s" << reg << ", $0" << endl;
					out << "li $v0, 11" << endl;
					out << "syscall" << endl;
				}
			}
		}
		//换行
		out << "la $a0, ENTER" << endl;
		out << "li $v0, 4" << endl;
		out << "syscall" << endl;
	}
}
void M_push() {
	int t_no = R_orpos;
	while (strcmp("call", order[t_no].op) != 0) {
		t_no++;
	}
	int p_Fpos = findfuncpos(order[t_no].var1);
	int p_Fsize = ftab[p_Fpos].size;
	int p_Fofset = ftab[p_Fpos].offset;
	//var3
	int reg = findFvarpos(order[R_orpos].var3, M_funcpos);
	M_varpos = findMvarpos(order[R_orpos].var3);
	bool glo = mtab[M_varpos].global;
	//set $t0 = para(var3)
	if (is_num(order[R_orpos].var3)) {
		out << "li $t0, " << atoi(order[R_orpos].var3) << endl;
	}
	else {
		if (reg == -1) {
			if (glo) {
				out << "lw $t0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "lw $t0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
	}
	out << "addu $t1, $sp, $0" << endl;
	out << "addi $t1, $t1, " << -(p_Fsize * 4 + 40 - p_Fofset) << endl;
	if (reg == -1) {
		out << "sw $t0, ($t1)" << endl;
	}
	else {
		out << "sw $s" << reg << ", ($t1)" << endl;
	}
	ftab[p_Fpos].offset = ftab[p_Fpos].offset + 4;
}
void M_call() {
	//call,name,  ,(var)
	int reg;
	bool glo;
	out << "jal " << order[R_orpos].var1 << endl;
	if (strcmp("", order[R_orpos].var3) != 0) {
		reg = findFvarpos(order[R_orpos].var3, M_funcpos);
		M_varpos = findMvarpos(order[R_orpos].var3);
		glo = mtab[M_varpos].global;
		if (reg == -1) {
			if (glo) {
				out << "sw $v0, " << mtab[M_varpos].addr << "($t9)" << endl;
			}
			else {
				out << "sw $v0, " << mtab[M_varpos].addr << "($sp)" << endl;
			}
		}
		else {
			out << "addu $s" << reg << ", $v0, $0" << endl;
		}	
	}
	int f = findfuncpos(order[R_orpos].var1);
	if (f != -1) {
		ftab[f].offset = 4;
	}
}
void M_ret(char funcname[]) {
	//ret, , ,(var)
	int reg;
	bool glo;
	if (strcmp("", order[R_orpos].var3) != 0) {//set $v0
		if (!is_num(order[R_orpos].var3)) {
			reg = findFvarpos(order[R_orpos].var3, M_funcpos);
			M_varpos = findMvarpos(order[R_orpos].var3);
			glo = mtab[M_varpos].global;
			if (reg == -1) {
				if (glo) {
					out << "lw $v0, " << mtab[M_varpos].addr << "($t9)" << endl;
				}
				else {
					out << "lw $v0, " << mtab[M_varpos].addr << "($sp)" << endl;
				}
			}
			else {
				out << "addu $v0, $s" << reg << ", $0" << endl;
			}
		}
		else {
			out << "li $v0, " << atoi(order[R_orpos].var3) << endl;
		}
	}
	out << "j end_" << funcname << endl;
}
//生成mips汇编结束

//DAG图优化
//void DAGmap() {
//	int D_orpos;
//	//遍历中间代码
//	for (D_orpos = 0; D_orpos < Order_num; D_orpos++) {
//		//初始化节点表以及DAG图
//		memset(ntab, 0, NODATABMAX * sizeof(struct Nodetab));
//
//	}
//}
//DAG图END

//窥孔优化开始
//void kuikong() {
//
//}
//窥孔优化结束

void main() {
	char FileNamePath[1024] = { '\0' };
	cout << "输入文件名:" << endl;
	cin >> FileNamePath;
	infile = fopen(FileNamePath, "r");
	outfile = fopen("中间代码.txt", "w");
	out.open("mips.asm", ios::out);//优化前
	m_out.open("mips_F.asm", ios::out);//优化后
	if (infile == NULL) {
		cout << "file doesn't exit" << endl;
		getchar();
		exit(0);
	}
	//int type = getSYM();
	//do {
	//	fprintf(outfile, "%s  ", SYM_NAME[sym]);
	//	fprintf(outfile, "%s", id);
	//	fprintf(outfile, "%c", '\n');
	//	type = getSYM();
	//} while (type != 0);
	getSYM();
	block();
	func_var_usingrecord();
	//DAG图优化
	buildMIPS();
	//窥孔优化
	fclose(infile);
	fclose(outfile);
	out.close();
}