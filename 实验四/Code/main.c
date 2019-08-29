#include<stdio.h>
#include<string.h>
#include "syntax.tab.h"
#include "semantic.h"
#include "intermediate.h"
#include "object.h"
extern int isCorrect;
extern int translate_correct;
extern struct Node* root;
extern void yyrestart(FILE *);
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
	FILE* fp = fopen(argv[2], "wt+");
	if (!fp)
	{
		perror(argv[2]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	if(isCorrect)
	{	
		//print_tree(root,0);
		//printf("----------------------\n");
		init_table();
		traverse(root,0);
		traverse_gen_intercode(root);
		if(translate_correct)
			//if(strcmp(argv[2],"1")==0)
			//	print_intercode();
			//print_intercode_to_file(fp);
			//else 
				print_object_code(fp);
	}
	//print_hash_table();
	return 0; 
}
