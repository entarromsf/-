//词法分析
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

//语法分析
enum Types {voidtyp,inttyp,chartyp,arraytyp};
enum OBJtyp {constant,variable,function};

const int Tmax = 10000;
const int Amax = 100;
const int Fmax = 100;
const int Ordermax = 1000;

char *err_msg[] = {

	"非法字符",			//0
	"字符内容错误",		//1
	"字符串内容错误",	//2
	"字符超过1个",		//3
	"!不可单独出现",	//4
	"缺失;",			//5
	"常量声明缺失类型",	//6
	"缺失函数或变量声明",//7
	"缺失标识符",		//8
	"函数参数错误",		//9
	"缺失{",			//10
	"语句不合法",		//11
	"缺失(",			//12
	"缺失)",			//13
	"缺失else语句",		//14
	"缺失:",			//15
	"case常量出错",		//16
	"缺失case",			//17
	"缺失default",		//18
	"缺失}",			//19
	"缺失=",			//20
	"变量定义在函数之后",//21
	"变量名重复",		//22
	"常量声明出错",		//23
	"=错输为==",		//24
	"常量初始化类型错误",//25
	"数组声明出错",		//26
	"缺失[",			//27
	"缺失]",			//28
	"变量声明出错",		//29
	"未定义变量",		//30
	"数组越界",			//31
	"函数调用错误",		//32
	"函数定义有()时不可无参数",//33
	"参数个数不匹配",	//34
	"factor出错",		//35
	"不能给非变量赋值",	//36
	"函数返回值错误",	//37
	"常量不可单独调用或赋值",//38
	"赋值语句出错",		//39
	"变量不为数组",		//40
	"函数缺少返回值",	//41
	"赋值类型不符合",	//42
	"条件类型不匹配",	//43
};


struct Tab {
	char name[IDMAXLENGTH];//常量,变量,数组,函数名称
	int link;//指向同一分程序的上一个标识符在tab中位置
	OBJtyp obj;//常量为constant,变量为variable,函数为function
	Types typ;//常量变量为int/char/数组为arrays函数返回值为int/char
	int ref; //数组在atab中位置, 函数在ftab中位置
	int normal;//变量形参为0,用于函数
	int addr;//值或ascii
	int parnum;//参数个数
}tab[Tmax];
struct Atab {
	Types eletyp;//元素类型int/char
	//int low;//数组下界
	//int high;//数组上界
	int elesize;//元素大小
	int size;//数组大小
}atab[Amax];
struct Var_info_tab {//用于记录一个函数块中使用的变量数据
	char name[IDMAXLENGTH];
	int using_num;
};
struct Ftab {
	char name[IDMAXLENGTH];//函数名
	int parnum;//参数个数
	int offset;//下一个参数偏移量
	int ref;//函数在tab中位置
	int size;//函数变量个数
	Types returntyp;//返回值类型
	Var_info_tab var_info[200];//用于记录一个函数块中使用的变量数据
	int var_num;//用于填表
}ftab[Fmax];
struct Order {
	char op[IDMAXLENGTH];
	char var1[IDMAXLENGTH];
	char var2[IDMAXLENGTH];
	char var3[IDMAXLENGTH];
}order[Ordermax];//中间代码/指令序列

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
int str_num = 0;//用于创建str的label
//DAG图//////////////////////////////////////////////////////////////////////
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

//词法分析
void getCh();
int key_match(char inword[]);
int getSYM();
//语法分析
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
//生成代码开始
void buildOrder(char op_t[], char var1_t[], char var2_t[], char var3_t[]);
char* newlabel();
char* newvar();
int findidpos(char id_t[]);
void enter(OBJtyp obj_t, Types typ_t, int w_Fun, char name_t[]);
void enterF(Types typ_t, int ref_t, char name_t[]);
bool is_num(char id_t[]);
//记录函数中的变量使用以便全局寄存器分配
int findFvarpos(char id_t[], int funcpos);
void func_var_usingrecord();
void sort_func_var_usingtime(int funcpos);
void add_var_usingtime(char id_t[], int funcpos);
int findfuncpos(char id_t[]);
//错误处理
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
//生成mips汇编开始
char* newstring();
void buildMIPS();
void buildData();
void enterString(char str[], char lab[]);
void buildText();
void buildMtab(char typ_t[], char id_t[], bool global);
int findMvarpos(char id_t[]);
void sw_var(char num_t[]);
void M_funcdec(char funcname[]);
//mips单条指令翻译
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