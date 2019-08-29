%option yylineno
%{
#include "syntax.tab.h"
#include "node.h"
void setToken(char* name, char* text);
int isCorrect=1;

%}
letter_ [A-Za-z_]
digit 	[0-9]
INT 	0|[1-9]{digit}*
FLOAT	(({digit}*\.{digit}+)|({digit}+\.{digit}*))([Ee]?[+-]?{digit}+)?
ID	{letter_}({letter_}|{digit})*	
RELOP 	>|<|>=|<=|==|!=
%%
" "	{;}
"\t"	{;}
"\n"	{;}
"\r"	{;}
{INT}	{setToken("INT",yytext);return INT;}
{FLOAT}	{setToken("FLOAT",yytext);return FLOAT;}
";"	{setToken("SEMI",yytext);return SEMI;}
","	{setToken("COMMA",yytext);return COMMA;}
{RELOP}	{setToken("RELOP",yytext);return RELOP;}
"="	{setToken("ASSIGNOP",yytext);return ASSIGNOP;}
"+"	{setToken("PLUS",yytext);return PLUS;}
"-"	{setToken("MINUS",yytext);return MINUS;}
"*"	{setToken("STAR",yytext);return STAR;}
"/"	{setToken("DIV",yytext);return DIV;}
"&&"	{setToken("AND",yytext);return AND;}
"||"	{setToken("OR",yytext);return OR;}
"."	{setToken("DOT",yytext);return DOT;}
"!"	{setToken("NOT",yytext);return NOT;}
"("	{setToken("LP",yytext);return LP;}
")"	{setToken("RP",yytext);return RP;}
"["	{setToken("LB",yytext);return LB;}
"]"	{setToken("RB",yytext);return RB;}
"{"	{setToken("LC",yytext);return LC;}
"}"	{setToken("RC",yytext);return RC;}
"struct"	{setToken("STRUCT",yytext);return STRUCT;}
"return"	{setToken("RETURN",yytext);return RETURN;}
"if"	{setToken("IF",yytext);return IF;}
"else"	{setToken("ELSE",yytext);return ELSE;}
"while"	{setToken("WHILE",yytext);return WHILE;}
"int"	{setToken("TYPE",yytext);return TYPE;}
"float"	{setToken("TYPE",yytext);return TYPE;}
{ID}	{setToken("ID",yytext);return ID;}
.	{isCorrect=0;printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%

void setToken(char* name, char* text)
{
	//printf("step into setToken\n");
	yylval.node = (struct Node *)malloc(sizeof(struct Node));
	yylval.node->col = yylineno;
	yylval.node->isToken = 1;
	strcpy(yylval.node->name,name);
	strcpy(yylval.node->text,text);
	yylval.node->firstChild = NULL;
	yylval.node->nextSibling = NULL;
}

