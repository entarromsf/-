#include "lexical.h"
FILE *infile, *outfile;
SymType sym;
char id[IDMAXLENGTH];
int Num = 0;

void getSYM() {//È¡Ò»¸ö´Ê
	char inword[IDMAXLENGTH];

}

int key_match(char inword[]) {
	int temp = 0;
	while (temp < KEY_NUM) {
		if (strcmp(inword, key[temp]) == 0)
			return temp;
	}
	return -1;
}

void main() {
	
}