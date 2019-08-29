#include"node.h"
#include"semantic.h"
#include"intermediate.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct InterCodes* cur_inter_code = NULL;
struct InterCodes* code_head=NULL;

extern struct Symbol_table_item *s_table[HASH_TABLE_SIZE];
extern struct Symbol_table_item *struct_table[HASH_TABLE_SIZE];

int label_num=1;
int temp_var_num=1;
int translate_correct=1;
char *array_name = NULL;

void print_op(Operand op)
{
	switch(op->kind)
	{
		case OP_VARIABLE:
			printf("%s",op->u.name);break;
		case OP_CONSTANT:
			printf("#%d",op->u.value);break;
		case OP_ADDRESS:
			printf("%s",op->u.name);break;
		case OP_LABEL:
			printf("%s",op->u.name);break;
		case OP_FUNCTION:
		case OP_RELOP:
			printf("%s",op->u.name);break;
	}
}
void print_intercode()
{
    struct InterCodes* x = code_head;
    //int num=1;
    while(x != NULL)
    {
    	//printf("%d  ",num);
    	//num++;
    	switch(x->code.kind)
    	{
    		case IR_LABEL:
    			printf("LABEL ");
    			print_op(x->code.u.one.op);
    			printf(" :");
    			break;
    		case IR_FUNCTION:
    		    printf("FUNCTION ");
    			print_op(x->code.u.one.op);
    			printf(" :");
    			break;
    		case IR_ASSIGN:
    		    print_op(x->code.u.two.left);
		        printf(" := ");
				print_op(x->code.u.two.right);
    			break;
    		case IR_ADD:
    			print_op(x->code.u.three.result);
		        printf(" := ");
				print_op(x->code.u.three.op1);
				printf(" + ");
				print_op(x->code.u.three.op2);
				break;
    		case IR_SUB:
    			print_op(x->code.u.three.result);
		        printf(" := ");
				print_op(x->code.u.three.op1);
				printf(" - ");
				print_op(x->code.u.three.op2);
				break;
    		case IR_MUL:
    			print_op(x->code.u.three.result);
		        printf(" := ");
				print_op(x->code.u.three.op1);
				printf(" * ");
				print_op(x->code.u.three.op2);
				break;
    		case IR_DIV:
    			print_op(x->code.u.three.result);
		        printf(" := ");
				print_op(x->code.u.three.op1);
				printf(" / ");
				print_op(x->code.u.three.op2);
				break;
			case IR_GET_ADDR:
    		    print_op(x->code.u.two.left);
		        printf(" := &");
				print_op(x->code.u.two.right);
    			break;				
			case IR_INTO_ADDR_RIGHT:
			   	print_op(x->code.u.two.left);
		        printf(" := *");
				print_op(x->code.u.two.right);
    			break;			
			case IR_INTO_ADDR_LEFT:
				printf("*");
    		    print_op(x->code.u.two.left);
		        printf(" := ");
				print_op(x->code.u.two.right);
    			break;
    		case IR_GOTO:
    			printf("GOTO ");
    			print_op(x->code.u.one.op);
    			break;
    		case IR_IF_GOTO:
    			printf("IF ");
    			print_op(x->code.u.if_goto.x);
    			printf(" ");
     			print_op(x->code.u.if_goto.relop);
     			printf(" ");
     			print_op(x->code.u.if_goto.y);
     			printf(" GOTO ");
    			print_op(x->code.u.if_goto.z);
    			break;  
    		case IR_RETURN:
    			printf("RETURN ");
    			print_op(x->code.u.one.op);
    			break;
    		case IR_DEC:
    			printf("DEC ");
    			print_op(x->code.u.dec.op);
    			printf(" ");
    			printf("%d",x->code.u.dec.size);
    			break;
    		case IR_ARG:
    			printf("ARG ");
    			print_op(x->code.u.one.op);
    			break;
    		case IR_CALL:
    			print_op(x->code.u.two.left);
    			printf(" := CALL ");
    			print_op(x->code.u.two.right);
    			break;
    		case IR_PARAM:
    			printf("PARAM ");
    			print_op(x->code.u.one.op);
    			break;
    		case IR_READ:
    			printf("READ ");
    			print_op(x->code.u.one.op);
    			break;    			
    		case IR_WRITE:
    			printf("WRITE ");
    			print_op(x->code.u.one.op);
    			break;    			
    			      			 			
    	}
    	printf("\n");
        x = x->next;
    }
    //printf("\n");
}

void print_op_to_file(FILE* fp, Operand op)
{
	switch(op->kind)
	{
		case OP_VARIABLE:
			fprintf(fp, "%s",op->u.name);break;
		case OP_CONSTANT:
			fprintf(fp, "#%d",op->u.value);break;
		case OP_ADDRESS:
			fprintf(fp, "%s",op->u.name);break;
		case OP_LABEL:
			fprintf(fp, "%s",op->u.name);break;
		case OP_FUNCTION:
		case OP_RELOP:
			fprintf(fp, "%s",op->u.name);break;
	}
}

void print_intercode_to_file(FILE* fp)
{
    struct InterCodes* x = code_head;
    while(x != NULL)
    {
    	switch(x->code.kind)
    	{
    		case IR_LABEL:
    			fprintf(fp, "LABEL ");
    			print_op_to_file(fp, x->code.u.one.op);
    			fprintf(fp, " :");
    			break;
    		case IR_FUNCTION:
    		    fprintf(fp, "FUNCTION ");
    			print_op_to_file(fp, x->code.u.one.op);
    			fprintf(fp, " :");
    			break;
    		case IR_ASSIGN:
    		    print_op_to_file(fp, x->code.u.two.left);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.two.right);
    			break;
    		case IR_ADD:
    			print_op_to_file(fp, x->code.u.three.result);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.three.op1);
				fprintf(fp, " + ");
				print_op_to_file(fp, x->code.u.three.op2);
				break;
    		case IR_SUB:
    			print_op_to_file(fp, x->code.u.three.result);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.three.op1);
				fprintf(fp, " - ");
				print_op_to_file(fp, x->code.u.three.op2);
				break;
    		case IR_MUL:
    			print_op_to_file(fp, x->code.u.three.result);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.three.op1);
				fprintf(fp, " * ");
				print_op_to_file(fp, x->code.u.three.op2);
				break;
    		case IR_DIV:
    			print_op_to_file(fp, x->code.u.three.result);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.three.op1);
				fprintf(fp, " / ");
				print_op_to_file(fp, x->code.u.three.op2);
				break;
			case IR_GET_ADDR:
    		    print_op_to_file(fp, x->code.u.two.left);
		        fprintf(fp, " := &");
				print_op_to_file(fp, x->code.u.two.right);
    			break;				
			case IR_INTO_ADDR_RIGHT:
			   	print_op_to_file(fp, x->code.u.two.left);
		        fprintf(fp, " := *");
				print_op_to_file(fp, x->code.u.two.right);
    			break;			
			case IR_INTO_ADDR_LEFT:
				fprintf(fp, "*");
    		    print_op_to_file(fp, x->code.u.two.left);
		        fprintf(fp, " := ");
				print_op_to_file(fp, x->code.u.two.right);
    			break;
    		case IR_GOTO:
    			fprintf(fp, "GOTO ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;
    		case IR_IF_GOTO:
    			fprintf(fp, "IF ");
    			print_op_to_file(fp, x->code.u.if_goto.x);
    			fprintf(fp, " ");
     			print_op_to_file(fp, x->code.u.if_goto.relop);
     			fprintf(fp, " ");
     			print_op_to_file(fp, x->code.u.if_goto.y);
     			fprintf(fp, " GOTO ");
    			print_op_to_file(fp, x->code.u.if_goto.z);
    			break;  
    		case IR_RETURN:
    			fprintf(fp, "RETURN ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;
    		case IR_DEC:
    			fprintf(fp, "DEC ");
    			print_op_to_file(fp, x->code.u.dec.op);
    			fprintf(fp, " ");
    			fprintf(fp, "%d",x->code.u.dec.size);
    			break;
    		case IR_ARG:
    			fprintf(fp, "ARG ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;
    		case IR_CALL:
    			print_op_to_file(fp, x->code.u.two.left);
    			fprintf(fp, " := CALL ");
    			print_op_to_file(fp, x->code.u.two.right);
    			break;
    		case IR_PARAM:
    			fprintf(fp, "PARAM ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;
    		case IR_READ:
    			fprintf(fp, "READ ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;    			
    		case IR_WRITE:
    			fprintf(fp, "WRITE ");
    			print_op_to_file(fp, x->code.u.one.op);
    			break;    			
    			      			 			
    	}
    	fprintf(fp, "\n");
        x = x->next;
    }
}

void add_intercode(struct InterCodes * new_code)
{
	if(cur_inter_code == NULL)
	{
		code_head = new_code;
		cur_inter_code = new_code;
	}	
	else
	{
		cur_inter_code->next = new_code;
		new_code->prev=cur_inter_code;
		cur_inter_code=new_code;
	}
}

Operand new_temp()
{
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_VARIABLE;
	char *str = (char*)malloc(100);
	sprintf(str, "t%d", temp_var_num);
	op->u.name = str;
	op->u.is_addr=0;
	temp_var_num++;
	return op;
}

Operand new_label()
{
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = OP_LABEL;
	char *str = (char*)malloc(100);
	sprintf(str, "label%d", label_num);
	op->u.name = str;
	label_num++;
	return op;
}

//IR_LABEL, IR_FUNCTION, IR_GOTO, IR_RETURN, IR_ARG, IR_PARAM, IR_READ, IR_WRITE
void create_one_intercode(Operand op, int kind)
{
	if(op->kind == OP_ADDRESS && op->u.is_addr==0)
	{
		Operand t1 = new_temp();
		create_two_intercode(t1, op, IR_INTO_ADDR_RIGHT);
		op = t1;
	}
	struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	new_code->next=NULL;
	new_code->prev=NULL;
	new_code->code.kind=kind;
	new_code->code.u.one.op = op;
	add_intercode(new_code);
}

/*
IR_ASSIGN,
IR_GET_ADDR,IR_INTO_ADDR_RIGHT,IR_INTO_ADDR_LEFT,
IR_CALL
*/
void create_two_intercode(Operand op1,Operand op2, int kind)
{
	if(kind == IR_ASSIGN && (op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS))
	{
		if(op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
			create_two_intercode(op1, op2, IR_INTO_ADDR_LEFT);
		else if(op2->kind == OP_ADDRESS && op1->kind != OP_ADDRESS)
			create_two_intercode(op1, op2, IR_INTO_ADDR_RIGHT);
		else
		{
			Operand t1 = new_temp();
			create_two_intercode(t1, op2, IR_INTO_ADDR_RIGHT);
			create_two_intercode(op1, t1, IR_INTO_ADDR_LEFT);						
		}
	}
	else if(kind == IR_GET_ADDR && op2->kind == OP_ADDRESS)
	{
		struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
		new_code->next=NULL;
		new_code->prev=NULL;
		new_code->code.kind=IR_ASSIGN;
		new_code->code.u.two.left = op1;
		new_code->code.u.two.right = op2;
		add_intercode(new_code);
	}
	else
	{
		struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
		new_code->next=NULL;
		new_code->prev=NULL;
		new_code->code.kind=kind;
		new_code->code.u.two.left = op1;
		new_code->code.u.two.right = op2;
		add_intercode(new_code);
	}

}

//IR_ADD,IR_SUB,IR_MUL,IR_DIV,
void create_three_intercode(Operand result, Operand op1,Operand op2, int kind)
{
	if(op1->kind == OP_ADDRESS && op1->u.is_addr==0)
	{
		Operand t1 = new_temp();
		create_two_intercode(t1, op1, IR_INTO_ADDR_RIGHT);
		op1 = t1;
	}
	if(op2->kind == OP_ADDRESS && op1->u.is_addr==0)
	{
		Operand t1 = new_temp();
		create_two_intercode(t1, op2, IR_INTO_ADDR_RIGHT);
		op2 = t1;
	}
	
	struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
	new_code->next=NULL;
	new_code->prev=NULL;
	new_code->code.kind=kind;
	new_code->code.u.three.result = result;
	new_code->code.u.three.op1 = op1;
	new_code->code.u.three.op2 = op2;
	add_intercode(new_code);
}


void translate_Exp(struct Node* exp, Operand place)
{
    	/*	Exp -> 
			//基本表达式
			  Exp ASSIGNOP Exp			1
			| ID						1
			| INT						1		
			| Exp PLUS Exp				1
			| Exp MINUS Exp				1
			| Exp STAR Exp				1
			| Exp DIV Exp				1		
			| MINUS Exp					1

			//条件表达式
			| Exp AND Exp				1
			| Exp OR Exp				1	
			| Exp RELOP Exp				1
			| NOT Exp					1

			//函数调用
			| ID LP Args RP				1	
			| ID LP RP					1

			| Exp LB Exp RB	//数组		1
			| Exp DOT ID	//结构体		1

			| LP Exp RP					1				
	*/
	//printf("exp\n");
	//Exp -> INT
	if(translate_correct==0)
		return;
	if(strcmp(exp->firstChild->name,"LP")==0)
		translate_Exp(exp->firstChild->nextSibling, place);
    if(strcmp(exp->firstChild->name,"INT")==0)
    {
        /*Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = OP_CONSTANT;
        op->u.value = atoi(exp->firstChild->text);

		create_two_intercode(place, op, IR_ASSIGN);*/
		temp_var_num--;
		place->kind = OP_CONSTANT;
		//place->u.name=NULL;
		place->u.value = atoi(exp->firstChild->text);
    }
	//Exp -> ID
    else if(strcmp(exp->firstChild->name,"ID")==0 && exp->firstChild->nextSibling == NULL)
    {
    	/*Operand op = (Operand)malloc(sizeof(struct Operand_));
		op->kind = OP_VARIABLE;
		op->u.name = exp->firstChild->text;
		create_two_intercode(place, op, IR_ASSIGN);*/

		struct Symbol_table_item* item = find_items_s_table(exp->firstChild->text);
		if(item->f->is_param==1&&item->f->type->kind==STRUCTURE)
			place->kind = OP_ADDRESS;
		else
			place->kind = OP_VARIABLE;
		temp_var_num--;
		place->u.name = exp->firstChild->text;	
    }
	//Exp -> Exp ASSIGNOP Exp
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "ASSIGNOP")==0)
	{
		//不考虑结构体和数组
		/*Operand exp1 = (Operand)malloc(sizeof(struct Operand_));
		exp1->kind = OP_VARIABLE;
		exp1->u.name = exp->firstChild->firstChild->text;*/
		Operand t2 = new_temp();
		translate_Exp(exp->firstChild, t2);

		Operand t1 = new_temp();
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t1);


		create_two_intercode(t2, t1, IR_ASSIGN);
	}
	//Exp -> Exp PLUS Exp
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "PLUS")==0)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();

		translate_Exp(exp->firstChild,t1);
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t2);
		
		create_three_intercode(place, t1, t2, IR_ADD);
	}
	//Exp -> Exp MINUS Exp
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "MINUS")==0)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();

		translate_Exp(exp->firstChild,t1);
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t2);
		
		create_three_intercode(place, t1, t2, IR_SUB);
	}
		//Exp -> Exp STAR Exp
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "STAR")==0)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();

		translate_Exp(exp->firstChild,t1);
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t2);
		
		create_three_intercode(place, t1, t2, IR_MUL);
	}
	//Exp -> Exp DIV Exp
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "DIV")==0)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();

		translate_Exp(exp->firstChild,t1);
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t2);
		
		create_three_intercode(place, t1, t2, IR_DIV);
	}
	//Exp -> MINUS Exp 
	else if(strcmp(exp->firstChild->name, "MINUS")==0)
	{
		Operand t1 = new_temp();
		translate_Exp(exp->firstChild->nextSibling,t1);
		
		Operand t2 = (Operand)malloc(sizeof(struct Operand_));
		t2->kind = OP_CONSTANT;
		t2->u.value = 0;

		create_three_intercode(place, t2, t1, IR_SUB);
	}
	else if(strcmp(exp->firstChild->name, "NOT")==0 || (strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "RELOP")==0)||(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "AND")==0)||(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "OR")==0))
	{	
		Operand label1 = new_label();
		Operand label2 = new_label();
		
		Operand t1 = (Operand)malloc(sizeof(struct Operand_));
		t1->kind = OP_CONSTANT;
		t1->u.value = 0;
		
		Operand t2 = (Operand)malloc(sizeof(struct Operand_));
		t2->kind = OP_CONSTANT;
		t2->u.value = 1;
		
		create_two_intercode(place, t1, IR_ASSIGN);
		translate_Cond(exp, label1, label2);
		create_one_intercode(label1, IR_LABEL);	
		create_two_intercode(place, t2, IR_ASSIGN);
		create_one_intercode(label2, IR_LABEL);	
	}
	//Exp -> ID LB (Args) RB
	else if(strcmp(exp->firstChild->name, "ID")==0 && exp->firstChild->nextSibling != NULL)
	{
	//printf("%s\n",exp->firstChild->text);
		Operand t = (Operand)malloc(sizeof(struct Operand_));
		t->kind = OP_FUNCTION;
		t->u.name = exp->firstChild->text;
		if(strcmp(exp->firstChild->nextSibling->nextSibling->name,"RP")==0)
		{
			if(strcmp(exp->firstChild->text, "read")==0)
			{
				create_one_intercode(place, IR_READ);
			}
			else
			{
				if(place != NULL)
					create_two_intercode(place, t, IR_CALL);
				else
				{
					Operand t1 = new_temp();
					create_two_intercode(t1, t, IR_CALL);
				}
			}
		}
		else
		{
			//printf("xwj\n");
		//这里由于要在函数中实现改变arg_list的值，因此创建了指针的指针作为参数传入
		//用单向连表存储参数列表，每新增一个参数，将其插入链表头部，以实现实参倒序传入
			struct Arg_list *arg_list = NULL;
			struct Arg_list **arg_list_p = &arg_list;
			translate_Args(exp->firstChild->nextSibling->nextSibling, arg_list_p);
			if(strcmp(exp->firstChild->text, "write")==0)
			{
			
				create_one_intercode(arg_list->op, IR_WRITE);
			}
			else
			{
				struct Arg_list *temp = arg_list;
				while(temp != NULL)
				{
					struct Symbol_table_item *item = find_items_s_table(temp->op->u.name);
					//结构体作为参数，传引用
					if(item!= NULL && item->f->type->kind==STRUCTURE){
						Operand t1 = new_temp();
						create_two_intercode(t1, temp->op, IR_GET_ADDR);
						t1->kind = OP_ADDRESS;
						t1->u.is_addr=1;
						create_one_intercode(t1, IR_ARG);
					}
					else
						create_one_intercode(temp->op, IR_ARG);
					temp=temp->next;
				}
				if(place != NULL)
					create_two_intercode(place, t, IR_CALL);
				else
				{
					Operand t1 = new_temp();
					create_two_intercode(t1, t, IR_CALL);
				}
			}
		}
	}
	//Exp -> Exp1 LB Exp2 RB
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "LB")==0)
	{
		/*	translate_Exp(Exp2, t1);
			t2 = t1*4;
			translate_Exp(Exp1, ID);//这里默认左侧Exp为ID
			t3 = &ID;
			place = t3+t2;
			不为id的情况：结构体变量，此时应返回地址类型变量
		*/
		Operand t1 = new_temp();
		Operand t2 = new_temp();
		Operand t3 = new_temp();
		Operand id = new_temp();
		translate_Exp(exp->firstChild->nextSibling->nextSibling,t1);
		translate_Exp(exp->firstChild, id);
		
		struct Symbol_table_item *item = find_items_s_table(id->u.name);
		if(item == NULL)
		{
			item = find_items_struct_table(id->u.name);
			if(item == NULL)
				printf("数组调用处发生段错误\n");
		}
		Operand t = (Operand)malloc(sizeof(struct Operand_));
		t->kind = OP_CONSTANT;
		t->u.value = get_size(item->f->type->u.array.elem);
		
		create_three_intercode(t2,t1,t,IR_MUL);
		create_two_intercode(t3,id, IR_GET_ADDR);
		create_three_intercode(place,t3,t2,IR_ADD);
		place->kind = OP_ADDRESS;
		//place->u.name = id->u.name;
		array_name = id->u.name;
		//printf("%s\n",id->u.name);
		//create_two_intercode(place, t4, IR_INTO_ADDR_RIGHT);
	}
	//Exp -> Exp1 DOT ID2 
	else if(strcmp(exp->firstChild->name, "Exp")==0 && strcmp(exp->firstChild->nextSibling->name, "DOT")==0)
	{
		/*	translate_Exp(Exp1, ID);
			t1 = &ID;
			t2 = offset;
			place = t1+t2;
		*/

		Operand id = new_temp();
		translate_Exp(exp->firstChild, id);
		Operand t1;
		if(id->kind != OP_ADDRESS)
		{
			t1 = new_temp();		
			create_two_intercode(t1,id, IR_GET_ADDR);
		}
		else
		{
			t1 = id;
			t1->u.is_addr=1;
		}			
		
		Operand id2 = (Operand)malloc(sizeof(struct Operand_));
		id2->kind = OP_VARIABLE;
		id2->u.name = exp->firstChild->nextSibling->nextSibling->text;

		int offset=0;
		struct Symbol_table_item *item = find_items_s_table(id->u.name);
		if(item == NULL)
		{
			//说明此变量为结构体成员变量
			item = find_items_struct_table(id->u.name);
			if(item == NULL)
			{
				//结构体数组
				item = find_items_s_table(array_name);
				if(item == NULL)
				{
					item = find_items_struct_table(array_name);
					if(item == NULL)
						printf("结构体调用处发生段错误\n");
				}
			}
		}
		
		FieldList tmp;
		if(item->f->type->kind == ARRAY)//结构体数组
			tmp = item->f->type->u.array.elem->u.structure_.structure;
		else
			tmp = item->f->type->u.structure_.structure;
			
		while(tmp != NULL)
		{
			if(strcmp(tmp->name,id2->u.name)==0)
				break;
			else
				offset=offset+get_size(tmp->type);
			tmp=tmp->tail;
		}
		Operand t2 = (Operand)malloc(sizeof(struct Operand_));
		t2->kind = OP_CONSTANT;
		t2->u.value = offset;

		create_three_intercode(place,t1,t2,IR_ADD);
		place->kind = OP_ADDRESS;
		place->u.name = id2->u.name;
		//printf("%s %s\n",place->u.name,id2->u.name);
			
	}
}

void translate_Args(struct Node* node, struct Arg_list ** arg_list_p)
{
	if(translate_correct==0)
		return;
	//Exp
	if(node->firstChild->nextSibling == NULL)
	{
		struct Arg_list *temp = (struct Arg_list *)malloc(sizeof(struct Arg_list));
		temp->op = new_temp();
		translate_Exp(node->firstChild, temp->op);
		
		if(temp->op->kind == OP_VARIABLE)
		{
			struct Symbol_table_item *temp_item = find_items_s_table(temp->op->u.name);
			if(temp_item != NULL && temp_item->f->type->kind == ARRAY)
			{
				translate_correct = 0;
				printf("Cannot translate: Code contains parameters of array type.\n");
				return;
			}
		}
		
		
		if(*arg_list_p == NULL)
			*arg_list_p = temp;
		else
		{
			temp->next = *arg_list_p;
			*arg_list_p = temp;
		}
	}
	//Exp COMMA Args
	else
	{
		struct Arg_list *temp = (struct Arg_list *)malloc(sizeof(struct Arg_list));
		temp->op = new_temp();
		translate_Exp(node->firstChild, temp->op);
		
		if(temp->op->kind == OP_VARIABLE)
		{
			struct Symbol_table_item *temp_item = find_items_s_table(temp->op->u.name);
			if(temp_item != NULL && temp_item->f->type->kind == ARRAY)
			{
				translate_correct = 0;
				printf("Cannot translate: Code contains parameters of array type.\n");
				return;
			}
		}
		
		if(*arg_list_p == NULL)
			*arg_list_p = temp;
		else
		{
			temp->next = *arg_list_p;
			*arg_list_p = temp;
		}
		translate_Args(node->firstChild->nextSibling->nextSibling, arg_list_p);	
	}
}


void translate_Cond(struct Node* node, Operand label_true, Operand label_false)
{
	if(translate_correct==0)
		return;
	if(strcmp(node->firstChild->name,"NOT")==0)
	{
		translate_Cond(node->firstChild, label_false, label_true);
	}
	else if(strcmp(node->firstChild->nextSibling->name,"RELOP")==0)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();
		translate_Exp(node->firstChild, t1);
		translate_Exp(node->firstChild->nextSibling->nextSibling, t2);
		
		Operand relop1 = (Operand)malloc(sizeof(struct Operand_));
		relop1->kind = OP_RELOP;
		relop1->u.name = node->firstChild->nextSibling->text;
		//if_goto语句，对地址类型变量取值
		if(t1->kind == OP_ADDRESS)
		{
			Operand t100 = new_temp();
			create_two_intercode(t100, t1, IR_INTO_ADDR_RIGHT);
			t1 = t100;
		}
	
		if(t2->kind == OP_ADDRESS)
		{
			Operand t100 = new_temp();
			create_two_intercode(t100, t2, IR_INTO_ADDR_RIGHT);
			t2 = t100;
		}
		
		struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
		new_code->next=NULL;
		new_code->prev=NULL;
		new_code->code.kind=IR_IF_GOTO;
		new_code->code.u.if_goto.x = t1;
		new_code->code.u.if_goto.relop = relop1;
		new_code->code.u.if_goto.y = t2;
		new_code->code.u.if_goto.z = label_true;
		add_intercode(new_code);
		create_one_intercode(label_false, IR_GOTO);	
	}
	else if(strcmp(node->firstChild->nextSibling->name,"AND")==0)
	{
		Operand label1 = new_label();
		translate_Cond(node->firstChild, label1, label_false);
		create_one_intercode(label1, IR_LABEL);		
		translate_Cond(node->firstChild->nextSibling->nextSibling, label_true, label_false);		
	}
	else if(strcmp(node->firstChild->nextSibling->name,"OR")==0)
	{
		Operand label1 = new_label();
		translate_Cond(node->firstChild, label_true, label1);
		create_one_intercode(label1, IR_LABEL);		
		translate_Cond(node->firstChild->nextSibling->nextSibling, label_true, label_false);		
	}
	else
	{
		Operand t1 = new_temp();
		translate_Exp(node, t1);
		
		Operand t2 = (Operand)malloc(sizeof(struct Operand_));
		t2->kind = OP_CONSTANT;
		t2->u.value = 0;
		
		Operand relop1 = (Operand)malloc(sizeof(struct Operand_));
		relop1->kind = OP_RELOP;
		relop1->u.name = "!=";
		
		if(t1->kind == OP_ADDRESS)
		{
			Operand t100 = new_temp();
			create_two_intercode(t100, t1, IR_INTO_ADDR_RIGHT);
			t1 = t100;
		}
	
		if(t2->kind == OP_ADDRESS)
		{
			Operand t100 = new_temp();
			create_two_intercode(t100, t2, IR_INTO_ADDR_RIGHT);
			t2 = t100;
		}
		
		struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
		new_code->next=NULL;
		new_code->prev=NULL;
		new_code->code.kind=IR_IF_GOTO;
		new_code->code.u.if_goto.x = t1;
		new_code->code.u.if_goto.relop = relop1;
		new_code->code.u.if_goto.y = t2;
		new_code->code.u.if_goto.z = label_true;
		add_intercode(new_code);
		
		create_one_intercode(label_false, IR_GOTO);
	}
}

int get_size(Type type)
{
	if(type == NULL)
		return 0;
	else if(type->kind == BASIC)
	{
		return 4;
	}
	else if(type->kind == ARRAY)
	{
		return type->u.array.size*get_size(type->u.array.elem);
	}
	else if(type->kind == STRUCTURE)
	{
		int size=0;
		FieldList temp = type->u.structure_.structure;
		while(temp != NULL)
		{
			if(temp->type->kind == BASIC)
			{
				size=size+4;
			}
			else if(temp->type->kind == ARRAY)
			{
				size = size+temp->type->u.array.size*get_size(temp->type->u.array.elem);
			}
			else if(temp->type->kind == STRUCTURE)
			{
				size = size+get_size(temp->type);
			}
			temp = temp->tail;
		}
		return size;
	}
	return 0;
}

void translate_VarDec(struct Node* node, Operand place)
{
	if(translate_correct==0)
		return;
	if(strcmp(node->firstChild->name,"ID")==0)
	{
		struct Symbol_table_item *temp = find_items_s_table(node->firstChild->text);
		if(temp->f->type->kind == BASIC)
		{
			if(place != NULL)
			{
			//定义变量时进行初始化
				temp_var_num--;
				place->kind = OP_VARIABLE;
				place->u.name = temp->f->name;
			}
		}
		else if(temp->f->type->kind == ARRAY)
		{
			//这里不考虑数组用另一个数组进行初始化
			if(temp->f->type->u.array.elem->kind == ARRAY)
			{
				translate_correct = 0;
				printf("Cannot translate: Code contains variables of multi-dimensional array type.\n");
				return;
			}
			else
			{
				Operand op = (Operand)malloc(sizeof(struct Operand_));
				op->kind = OP_VARIABLE;
				op->u.name = temp->f->name;
				
				struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
				new_code->next=NULL;
				new_code->prev=NULL;
				new_code->code.kind=IR_DEC;
				new_code->code.u.dec.op = op;
				new_code->code.u.dec.size = get_size(temp->f->type);
				add_intercode(new_code);
			}
		}
		else if(temp->f->type->kind == STRUCTURE)
		{
		//TODO:选做1部分
				Operand op = (Operand)malloc(sizeof(struct Operand_));
				op->kind = OP_VARIABLE;
				op->u.name = temp->f->name;
				
				//printf("%d\n",temp->f->type->u.structure_.structure->type->u.array.elem->kind);
				
				struct InterCodes * new_code = (struct InterCodes*)malloc(sizeof(struct InterCodes));
				new_code->next=NULL;
				new_code->prev=NULL;
				new_code->code.kind=IR_DEC;
				new_code->code.u.dec.op = op;
				new_code->code.u.dec.size = get_size(temp->f->type);
				add_intercode(new_code);
		}
	}
	else
		translate_VarDec(node->firstChild, place);
}

void translate_Dec(struct Node* node)
{
	if(translate_correct==0)
		return;
	if(node->firstChild->nextSibling != NULL)
	{
		Operand t1 = new_temp();
		Operand t2 = new_temp();		
		translate_VarDec(node->firstChild, t1);
		translate_Exp(node->firstChild->nextSibling->nextSibling, t2);
		create_two_intercode(t1,t2,IR_ASSIGN);
	}
	else
		translate_VarDec(node->firstChild, NULL);

}

void translate_DecList(struct Node* node)
{
	if(translate_correct==0)
		return;
	translate_Dec(node->firstChild);
	if(node->firstChild->nextSibling != NULL)
		translate_DecList(node->firstChild->nextSibling->nextSibling);
}

void translate_Def(struct Node* node)
{
	if(translate_correct==0)
		return;
	translate_DecList(node->firstChild->nextSibling);
}

void translate_DefList(struct Node* node)
{
	if(translate_correct==0)
		return;
	if(node == NULL)
		return;
	if(strcmp(node->firstChild->name,"Def")==0)
	{
		translate_Def(node->firstChild);
		translate_DefList(node->firstChild->nextSibling);
	}	
}

void translate_CompSt(struct Node* node)
{
	if(translate_correct==0)
		return;
	struct Node* CompSt = node;
	struct Node* StmtList;
	if(strcmp(CompSt->firstChild->nextSibling->name,"StmtList") == 0)
		StmtList = CompSt->firstChild->nextSibling;
	else
	{
		StmtList = CompSt->firstChild->nextSibling->nextSibling;
		translate_DefList(CompSt->firstChild->nextSibling);
	}
	translate_StmtList(StmtList);
}
void translate_Stmt(struct Node* node)
{
	if(translate_correct==0)
		return;
	//printf("Stmt\n");
	//Exp SEMI
	if(strcmp(node->firstChild->name,"Exp")==0)
	{
		//printf("xwj\n");
		translate_Exp(node->firstChild, NULL);
	}
	//CompSt
	else if(strcmp(node->firstChild->name,"CompSt")==0)
	{
		translate_CompSt(node->firstChild);
	}
	//RETURN Exp SEMI
	else if(strcmp(node->firstChild->name,"RETURN")==0)
	{
		Operand t1 = new_temp();
		translate_Exp(node->firstChild->nextSibling, t1);
		
		create_one_intercode(t1, IR_RETURN);
	}
	//IF LP Exp RP Stmt (ELSE Stmt)
	else if(strcmp(node->firstChild->name,"IF")==0)
	{
		Operand label1 = new_label();
		Operand label2 = new_label();
		struct Node* Exp = node->firstChild->nextSibling->nextSibling;
		struct Node* Stmt1 = Exp->nextSibling->nextSibling;
		translate_Cond(Exp, label1, label2);
		create_one_intercode(label1, IR_LABEL);
		translate_Stmt(Stmt1);
		if(Stmt1->nextSibling == NULL)
		{
			create_one_intercode(label2, IR_LABEL);	
		}
		else
		{
			Operand label3 = new_label();
			struct Node* Stmt2 = Stmt1->nextSibling->nextSibling;
			create_one_intercode(label3, IR_GOTO);	
			create_one_intercode(label2, IR_LABEL);
			translate_Stmt(Stmt2);
			create_one_intercode(label3, IR_LABEL);		
		}
	}
	//WHILE LP Exp RP Stmt
	else if(strcmp(node->firstChild->name,"WHILE")==0)
	{
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand label3 = new_label();
		
		create_one_intercode(label1, IR_LABEL);	
		translate_Cond(node->firstChild->nextSibling->nextSibling, label2, label3);	
		create_one_intercode(label2, IR_LABEL);	
		translate_Stmt(node->firstChild->nextSibling->nextSibling->nextSibling->nextSibling);	
		create_one_intercode(label1, IR_GOTO);	
		create_one_intercode(label3, IR_LABEL);
	}	
}

void translate_StmtList(struct Node* node)
{
	if(translate_correct==0)
		return;
	if(node == NULL)
		return;
	if(strcmp(node->firstChild->name,"Stmt")==0)
	{
		//printf("StmtList\n");
		translate_Stmt(node->firstChild);
		translate_StmtList(node->firstChild->nextSibling);
	}
}

void translate_FunDec(struct Node* node)
{
	if(translate_correct==0)
		return;
	Operand t = (Operand)malloc(sizeof(struct Operand_));
	t->kind = OP_FUNCTION;
	t->u.name = node->firstChild->text;
	create_one_intercode(t, IR_FUNCTION);
	
	struct Symbol_table_item *fun = find_items_s_table(t->u.name);
	if(fun->f->type->u.function.paramNum != 0)
	{
		FieldList temp = fun->f->type->u.function.params;
		while(temp != NULL)
		{
			Operand tt = (Operand)malloc(sizeof(struct Operand_));
			tt->kind = OP_VARIABLE;
			tt->u.name = temp->name;
			temp->is_param=1;
			create_one_intercode(tt, IR_PARAM);
			temp = temp->tail;
		}
	}
}
void translate_ExtDef(struct Node* node)
{
	if(translate_correct==0)
		return;
	if(strcmp(node->firstChild->nextSibling->name,"FunDec")==0)
	{
		//printf("FunDec\n");
		struct Node* FunDec = node->firstChild->nextSibling;
		translate_FunDec(FunDec);
		struct Node* CompSt = FunDec->nextSibling;
		translate_CompSt(CompSt);
	}
}

void translate_ExtDefList(struct Node* node)
{
	if(translate_correct==0)
		return;
	if(node == NULL)
		return;
	if(strcmp(node->firstChild->name,"ExtDef")==0)
	{
		translate_ExtDef(node->firstChild);
		translate_ExtDefList(node->firstChild->nextSibling);
	}
}
void traverse_gen_intercode(struct Node* this_node)
{
	if(translate_correct==0)
		return;
	if(this_node == NULL)
		return;
	if(strcmp(this_node->name,"ExtDefList")==0)
	{
		translate_ExtDefList(this_node);
    }
	else
    {
        traverse_gen_intercode(this_node->firstChild);
	    traverse_gen_intercode(this_node->nextSibling);
    }
    
}
