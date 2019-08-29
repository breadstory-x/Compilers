#include<stdio.h>
#include "syntax.tab.h"
extern int isCorrect;
extern struct Node* root;
int main(int argc, char** argv)
{
	if(argc <= 1) 
	{
		yyparse();
		return 1;
	}
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if(isCorrect)
		print_tree(root,0);
	return 0; 
}
