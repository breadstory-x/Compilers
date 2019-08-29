%{
	#include<stdio.h>
	#include <stdarg.h>
	struct Node* addNode(char *name, int num,...);
	void print_tree(struct Node* this_node,int depth);
	struct Node* root;
	extern int isCorrect;
	# define YYERROR_VERBOSE 1
	int yyerror(const char* msg);
%}

/* declared types */
%union {
	struct Node* node;
}
/* declared tokens */
%token <node> INT
%token <node> FLOAT
%token <node> SEMI 
%token <node> COMMA 
%token <node> RELOP 
%token <node> ASSIGNOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND 
%token <node> OR 
%token <node> DOT 
%token <node> NOT 
%token <node> LP 
%token <node> RP 
%token <node> LB 
%token <node> RB 
%token <node> LC 
%token <node> RC
%token <node> STRUCT 
%token <node> RETURN 
%token <node> IF 
%token <node> ELSE 
%token <node> WHILE 
%token <node> TYPE
%token <node> ID
/* declared non-terminals */
%type <node> Program ExtDefList ExtDef ExtDecList
%type <node> Specifier StructSpecifier OptTag Tag
%type <node> VarDec FunDec VarList ParamDec
%type <node> CompSt StmtList Stmt 
%type <node> DefList Def DecList Dec
%type <node> Exp Args


%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
//7.1.2 High-level Definitions
Program: ExtDefList			{$$ = addNode("Program",1,$1);root = $$;}
	;
ExtDefList: ExtDef ExtDefList		{$$ = addNode("ExtDefList",2,$1,$2);}				
	| 				{$$ = NULL;}
	;
ExtDef : Specifier ExtDecList SEMI	{$$ = addNode("ExtDef",3,$1,$2,$3);}
	| Specifier SEMI		{$$ = addNode("ExtDef",2,$1,$2);}
	| Specifier FunDec CompSt	{$$ = addNode("ExtDef",3,$1,$2,$3);}
	| error SEMI			{isCorrect = 0;}
	;
ExtDecList : VarDec			{$$ = addNode("ExtDecList",1,$1);}
	| VarDec COMMA ExtDecList	{$$ = addNode("ExtDecList",3,$1,$2,$3);}
	;
//7.1.3 Specifiers
Specifier : TYPE			{$$ = addNode("Specifier",1,$1);}
	| StructSpecifier		{$$ = addNode("Specifier",1,$1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC	{$$ = addNode("StructSpecifier",5,$1,$2,$3,$4,$5);}
	| STRUCT Tag			{$$ = addNode("StructSpecifier",2,$1,$2);}
	//| error RC			{isCorrect = 0;}
	;
OptTag : ID				{$$ = addNode("OptTag",1,$1);}
	| 				{$$ = NULL;}
	;
Tag : ID				{$$ = addNode("Tag",1,$1);}
	;
//7.1.4 Declarators
VarDec : ID				{$$ = addNode("VarDec",1,$1);}
	| VarDec LB INT RB		{$$ = addNode("VarDec",4,$1,$2,$3,$4);}
	| error RB			{isCorrect=0;}
	;
FunDec : ID LP VarList RP		{$$ = addNode("FunDec",4,$1,$2,$3,$4);}
	| ID LP RP			{$$ = addNode("FunDec",3,$1,$2,$3);}
	| error RP			{isCorrect=0;}
	;
VarList : ParamDec COMMA VarList	{$$ = addNode("VarList",3,$1,$2,$3);}
	| ParamDec			{$$ = addNode("VarList",1,$1);}
	;
ParamDec : Specifier VarDec		{$$ = addNode("ParamDec",2,$1,$2);}
	;
//7.1.5 Statements
CompSt : LC DefList StmtList RC		{$$ = addNode("CompSt",4,$1,$2,$3,$4);}
	| error RC			{isCorrect=0;}
	;
StmtList : Stmt StmtList		{$$ = addNode("StmtList",2,$1,$2);}
	|				{$$ = NULL;}
	;
Stmt : Exp SEMI				{$$ = addNode("Stmt",2,$1,$2);}
	| error SEMI			{isCorrect=0;}
	| CompSt			{$$ = addNode("Stmt",1,$1);}
	| RETURN Exp SEMI		{$$ = addNode("Stmt",3,$1,$2,$3);}	
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE	{$$ = addNode("Stmt",5,$1,$2,$3,$4,$5);}
	| IF LP error RP Stmt ELSE Stmt	{isCorrect=0;}
	| IF LP Exp RP Stmt ELSE Stmt	{$$ = addNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	| WHILE LP Exp RP Stmt		{$$ = addNode("Stmt",5,$1,$2,$3,$4,$5);}
	| WHILE LP error RP Stmt	{isCorrect=0;}
	;
//7.1.6 Local Definitions
DefList : Def DefList			{$$ = addNode("DefList",2,$1,$2);}
	|				{$$ = NULL;}
	; 	
Def : Specifier DecList SEMI		{$$ = addNode("Def",3,$1,$2,$3);}
	//| error SEMI			{isCorrect=0;}
	;
DecList : Dec				{$$ = addNode("DecList",1,$1);}
	| Dec COMMA DecList		{$$ = addNode("DecList",3,$1,$2,$3);}
	;
Dec : VarDec				{$$ = addNode("Dec",1,$1);}
	| VarDec ASSIGNOP Exp		{$$ = addNode("Dec",3,$1,$2,$3);}
	;
//7.1.7 Expressions
Exp : Exp ASSIGNOP Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp AND Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp OR Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp RELOP Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp PLUS Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp MINUS Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp STAR Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp DIV Exp			{$$ = addNode("Exp",3,$1,$2,$3);}
	| LP Exp RP			{$$ = addNode("Exp",3,$1,$2,$3);}
	| MINUS Exp			{$$ = addNode("Exp",2,$1,$2);}
	| NOT Exp			{$$ = addNode("Exp",2,$1,$2);}
	| ID LP Args RP			{$$ = addNode("Exp",4,$1,$2,$3,$4);}
	| ID LP RP			{$$ = addNode("Exp",3,$1,$2,$3);}
	| Exp LB Exp RB			{$$ = addNode("Exp",4,$1,$2,$3,$4);}
	| Exp DOT ID			{$$ = addNode("Exp",3,$1,$2,$3);}
	| ID				{$$ = addNode("Exp",1,$1);}
	| INT				{$$ = addNode("Exp",1,$1);}
	| FLOAT				{$$ = addNode("Exp",1,$1);}
	;
Args : Exp COMMA Args			{$$ = addNode("Args",3,$1,$2,$3);}
	| Exp				{$$ = addNode("Args",1,$1);}
	;
%%
#include "lex.yy.c"
struct Node* addNode(char *name, int num,...)
{
	//printf("step into addNode\n");
	struct Node* cur = (struct Node*)malloc(sizeof(struct Node));
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));

	va_list valist;
	va_start(valist, num);
	temp = va_arg(valist, struct Node*);//返回struct Node*类型的参数

	cur->col = temp->col;
	cur->isToken = 0;
	strcpy(cur->name,name);
	strcpy(cur->text,"");

	cur->firstChild = temp;

	//printf("%s\n",cur->name);
	//printf("    %s\n",cur->firstChild->name);
	for(int i = 1;i<num;i++)
	{
		temp->nextSibling = va_arg(valist, struct Node*);
		
		if(temp->nextSibling != NULL)
		{
			//printf("    %s\n",temp->nextSibling->name);
			temp = temp->nextSibling;
		}
	}
	return cur;
}

void print_tree(struct Node* this_node,int depth)
{
	if(this_node == NULL)
		return;
	for(int i = 0;i<depth;i++)
		printf("  ");
	printf("%s",this_node->name);
	if(!this_node->isToken)
		printf(" (%d)",this_node->col);
	else if(strcmp(this_node->name,"ID")==0 || strcmp(this_node->name,"INT")==0||strcmp(this_node->name,"TYPE")==0)
		printf(":%s",this_node->text);
	else if(strcmp(this_node->name,"FLOAT")==0)
		printf(":%lf",atof(this_node->text));
	printf("\n");
	print_tree(this_node->firstChild,depth+1);
	print_tree(this_node->nextSibling,depth);
}

int yyerror(const char* msg)
{
	fprintf(stderr, "Error type B at line %d:%s\n",yylineno,msg);
}
