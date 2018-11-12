#include "lexical.h"
#include <iostream>
using namespace std;

FILE *infile, *outfile;
SymType sym;//����������
char id[IDMAXLENGTH];//�����ʶ��
char Str[STRMAXLENGTH];//�����ַ���
int Num = 0;//��������
char ch = ' ';//���ַ�
int line = 0;//����

void getCh() {
	ch = fgetc(infile);
	if (ch >= 'A' && ch <= 'Z') {
		ch = ch + 'a' - 'A';
	}
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
int getSYM() {//ȡһ����
	char inword[IDMAXLENGTH];
	//��ʼ����ʶ������
//	cout << "getsym" << endl;
	for (int i = 0; i < IDMAXLENGTH; i++) {
		id[i] = '\0';
	}
	//����հ׺ͻ���
	while (ch == ' ' || ch == '\n' || ch == '\t') {
		if (ch == '\n')
			line++;
		getCh();
	}
	//������
	if (ch == EOF)
		return 0;
	//ʶ���ʶ��
	if (isalpha(ch) || ch == '_') {
		int idl = 0;
		do {
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
	//����
	else if (isdigit(ch))
	{
		int numl = 0;
		sym = SYM_NUMBER;
		do {
			Num = 10 * Num + ch - '0';
			numl++;
			getCh();
		} while (isdigit(ch));
		//��������λ��
		//if (numl > 100000000){}
		return 2;
	}
	//�ַ�3
	else if (ch == '\'') {
		sym = SYM_ZF;
		getCh();
		id[0] = ch;
		do {
			getCh();
			if (ch != '\'') {
				//����
			}
		} while (ch != '\'');
		getCh();
		return 3;
	}
	//�ַ���4
	else if (ch == '\"') {
		sym = SYM_STRING;
		int i = 0;
		do {
			getCh();
			if (ch != '\"') {
				id[i] = ch;
				i++;
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
			//δʶ��
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
		getCh();
	}
	return 5;
}


void main() {
	char FileNamePath[1024] = { '\0' };
	cout << "�����ļ���:" << endl;
	cin >> FileNamePath;
	infile = fopen(FileNamePath, "r");
	outfile = fopen("16231046_result.txt", "w");
	if (infile == NULL) {
		cout << "file doesn't exit" << endl;
		getchar();
		exit(0);
	}
	int type = getSYM();
	do {
		fprintf(outfile, "%s  ", SYM_NAME[sym]);
		fprintf(outfile, "%s", id);
		fprintf(outfile, "%c", '\n');
		type = getSYM();
	} while (type != 0);
	fclose(infile);
	fclose(outfile);
}