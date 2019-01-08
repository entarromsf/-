//�ʷ�����
const int KEY_NUM = 15;
const int IDMAXLENGTH = 100;
const int STRMAXLENGTH = 100;
const int MAXLINE = 500;

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

	SYM_IDENTIFIER,//��ʶ��

	SYM_NUMBER,//����
			   //�������
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
			   SYM_COLON,//:
			   SYM_SEMICOLON,//;
			   SYM_PERIOD,//.
			   SYM_BECOMES,//=
			   SYS_QUOTE,//'
			   SYS_DQUOTE,//"
			   SYM_ZF,//zf
			   SYM_STRING,//string
			   SYM_NUL//null
};

SymType key_table[KEY_NUM] = { SYM_CONST,SYM_DO,SYM_WHILE,SYM_IF,SYM_ELSE,SYM_INT,SYM_CHAR,SYM_SCANF,SYM_PRINTF,SYM_MAIN,SYM_VOID,SYM_RETURN,SYM_SWITCH,SYM_CASE,SYM_DEFAULT };

char *SYM_NAME[] = {
	"CONST","DO","WHILE","IF","ELSE","INT","CHAR","SCANF","PRINTF","MAIN","VOID","RETURN","SWITCH","CASE","DEFAULT",
	"IDENTIFIER","NUMBER",
	"PLUS","MINUS","TIMES","SLASH","EQU","NEQ","LES","LEQ","GTR","GEQ","LPAREN","RPAREN","LBRACK","RBRACK","LBRACE","RBRACE","COMMA","COLON","SEMICOLON","PERIOD","BECOMES","QUOTE","DQUOTE","zifu","string","Somethingelse"
};

char* key[KEY_NUM] =
{
	"const","do","while","if","else","int","char","scanf","printf","main","void","return","switch","case","default"
};

//�﷨����
enum Types {voidtyp,inttyp,chartyp,arraytyp};
enum OBJtyp {constant,variable,function};

const int Tmax = 10000;
const int Amax = 100;
const int Fmax = 100;
const int Ordermax = 1000;

char *err_msg[] = {

	"�Ƿ��ַ�",			//0
	"�ַ����ݴ���",		//1
	"�ַ������ݴ���",	//2
	"�ַ�����1��",		//3
	"!���ɵ�������",	//4
	"ȱʧ;",			//5
	"��������ȱʧ����",	//6
	"ȱʧ�������������",//7
	"ȱʧ��ʶ��",		//8
	"������������",		//9
	"ȱʧ{",			//10
	"��䲻�Ϸ�",		//11
	"ȱʧ(",			//12
	"ȱʧ)",			//13
	"ȱʧelse���",		//14
	"ȱʧ:",			//15
	"case��������",		//16
	"ȱʧcase",			//17
	"ȱʧdefault",		//18
	"ȱʧ}",			//19
	"ȱʧ=",			//20
	"���������ں���֮��",//21
	"�������ظ�",		//22
	"������������",		//23
	"=����Ϊ==",		//24
	"������ʼ�����ʹ���",//25
	"������������",		//26
	"ȱʧ[",			//27
	"ȱʧ]",			//28
	"������������",		//29
	"δ�������",		//30
	"����Խ��",			//31
	"�������ô���",		//32
	"����������()ʱ�����޲���",//33
	"����������ƥ��",	//34
	"factor����",		//35
	"���ܸ��Ǳ�����ֵ",	//36
	"��������ֵ����",	//37
	"�������ɵ������û�ֵ",//38
	"��ֵ������",		//39
	"������Ϊ����",		//40
	"����ȱ�ٷ���ֵ",	//41
	"��ֵ���Ͳ�����",	//42
	"�������Ͳ�ƥ��",	//43
};


struct Tab {
	char name[IDMAXLENGTH];//����,����,����,��������
	int link;//ָ��ͬһ�ֳ������һ����ʶ����tab��λ��
	OBJtyp obj;//����Ϊconstant,����Ϊvariable,����Ϊfunction
	Types typ;//��������Ϊint/char/����Ϊarrays��������ֵΪint/char
	int ref; //������atab��λ��, ������ftab��λ��
	int normal;//�����β�Ϊ0,���ں���
	int addr;//ֵ��ascii
	int parnum;//��������
}tab[Tmax];
struct Atab {
	Types eletyp;//Ԫ������int/char
	//int low;//�����½�
	//int high;//�����Ͻ�
	int elesize;//Ԫ�ش�С
	int size;//�����С
}atab[Amax];
struct Var_info_tab {//���ڼ�¼һ����������ʹ�õı�������
	char name[IDMAXLENGTH];
	int using_num;
};
struct Ftab {
	char name[IDMAXLENGTH];//������
	int parnum;//��������
	int offset;//��һ������ƫ����
	int ref;//������tab��λ��
	int size;//������������
	Types returntyp;//����ֵ����
	Var_info_tab var_info[200];//���ڼ�¼һ����������ʹ�õı�������
	int var_num;//�������
}ftab[Fmax];
struct Order {
	char op[IDMAXLENGTH];
	char var1[IDMAXLENGTH];
	char var2[IDMAXLENGTH];
	char var3[IDMAXLENGTH];
}order[Ordermax];//�м����/ָ������

struct mipsOrder {
	char line[500];
	char op[100];
	char var1[100];
	char var2[100];
	char var3[100];
}mipsorder[5000];
int mipsorder_num = 0;

int Tab_num = 0;
int Atab_num = 0;
int Ftab_num = 0;
int Order_num = 0;
int label_num = 0;
int var_num = 0;
char nowvar[IDMAXLENGTH];
bool endflag = false;
bool returnflag = false;
SymType print_lx;
/////////////////////////////////////////////////////////////////////////////
const int MIPSTABMAX = 10000;
struct Mipstab {
	char name[IDMAXLENGTH];
	SymType typ;
	int addr;
	bool global;
}mtab[MIPSTABMAX];
int Mtab_num = 0;
int R_orpos = 0;
int build_offset = 4;
int M_funcpos = 0;
bool main_flag = false;
int M_varpos = 0;
struct Mstringtab {
	char string[STRMAXLENGTH];
	char label[IDMAXLENGTH];
}strtab[1000];
int Strtab_num = 0;
int str_num = 0;//���ڴ���str��label
//DAGͼ//////////////////////////////////////////////////////////////////////
const int NODATABMAX = 100;
struct Nodetab {
	char id[IDMAXLENGTH];
	int nodenum;
}ntab[NODATABMAX];
struct DAGtab {
	char op[IDMAXLENGTH];
	bool is_leaf;
	int in_order;
	int pi;
	int left_index;
	int right_index;
	int parent_index[100];
}*nodelist[100];
int Ntab_num = 0;
//Fucntion Begain////////////////////////////////////////////////////////////

//�ʷ�����
void getCh();
int key_match(char inword[]);
int getSYM();
//�﷨����
void test(SymType typ_t, int n);
void constdec();
void enterConst(SymType const_typ_t, char const_id_t[], int value_t);
void enterVar(SymType var_typ_t, char var_id_t[]);
void enterArray(SymType arry_typ_t, char array_id_t[]);
void enterAtab(SymType array_typ_t, int array_num_t);
void vardec(SymType typ_thisline, char firstid[]);
void fundec(SymType pre_sym);
void parameterlist();
void statementlist();
void statement();
void dowhilestatement();
void ifelsestatement();
void scanfstatement();
void printfstatement();
void returnstatement();
void switchstatement();
void situation_table(char casevar[]);
void assignstatement(int idpos);
void condition();
void expression();
void term();
void factor();
int v_para(char (*para)[IDMAXLENGTH], int idpos);
void block();
//���ɴ��뿪ʼ
void buildOrder(char op_t[], char var1_t[], char var2_t[], char var3_t[]);
char* newlabel();
char* newvar();
int findidpos(char id_t[]);
void enter(OBJtyp obj_t, Types typ_t, int w_Fun, char name_t[]);
void enterF(Types typ_t, int ref_t, char name_t[]);
bool is_num(char id_t[]);
//��¼�����еı���ʹ���Ա�ȫ�ּĴ�������
int findFvarpos(char id_t[], int funcpos);
void func_var_usingrecord();
void sort_func_var_usingtime(int funcpos);
void add_var_usingtime(char id_t[], int funcpos);
int findfuncpos(char id_t[]);
//������
void error(int n);
void endskip();
void skip();
void skip2semi();
void skip2func();
void skip2lbrace();
void skip2case();
void skip2rbrace();
void skip2cich();
void skip2rbrack();
void skip2rparen();
void skipstatementlist();
//����mips��࿪ʼ
char* newstring();
void buildMIPS();
void buildData();
void enterString(char str[], char lab[]);
void buildText();
void buildMtab(char typ_t[], char id_t[], bool global);
int findMvarpos(char id_t[]);
void sw_var(char num_t[]);
void M_funcdec(char funcname[]);
//mips����ָ���
void M_para();
void M_plus();
void M_minus();
void M_time();
void M_div();
void M_equ();
void M_neq();
void M_gtr();
void M_geq();
void M_les();
void M_leq();
void M_lab();
void M_goto();
void M_ass();
void M_seta();
void M_geta();
void M_scf();
void M_prt();
void M_push();
void M_call();
void M_ret(char funcname[]);
//Function End///////////////////////////////////////////////////////////////