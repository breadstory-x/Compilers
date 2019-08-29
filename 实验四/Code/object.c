#include"node.h"
#include"semantic.h"
#include"intermediate.h"
#include"object.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

extern struct InterCodes* code_head;
int in_func=0;//表示当前是否在函数中
int param_reg_num=4;
char *cur_fun_name;

int last_changed_reg=0;

void print_mem_var()
{
	printf("-----------------print_mem_var----------------\n");
	struct VarDesc* p = var_head_in_mem;
	while(p!=NULL)
	{
		printf("%s\n",p->op->u.name);
		p=p->next;
	}
	printf("-----------------end----------------\n");	
}

void print_var()
{
	printf("\n-----------------print_var----------------\n");
	struct VarDesc* p = var_head;
	while(p!=NULL)
	{
		if(p->op->kind == OP_CONSTANT)
			printf("constant: ");
		else
			printf("other: ");
		printf("name: %s reg: %d %s\n",p->op->u.name,p->reg_num,reg_list[p->reg_num].name);
		p=p->next;
	}
	printf("-----------------end----------------\n\n");	
}

//写回内存
void add_var_desc_in_mem(Operand op)
{
	struct VarDesc* new_var=NULL;

	new_var = (struct VarDesc*)malloc(sizeof(struct VarDesc));
	new_var->op=op;
	new_var->next=NULL;
	if(var_head_in_mem==NULL)
	{
		var_head_in_mem = new_var;
		var_cur_in_mem=new_var;
	}
	else
	{
		var_cur_in_mem->next=new_var;
		var_cur_in_mem=new_var;
	}
}

void del_var_desc_in_mem(struct VarDesc* var)
{
	if(var == var_head_in_mem)
	{
		var_head_in_mem=var_head_in_mem->next;
	}
	else
	{
		struct VarDesc* p = var_head_in_mem;
		while(p!=NULL)
		{
			if(p->next==var)
				break;
			p=p->next;
		}
		if(var_cur_in_mem == var)
			var_cur_in_mem = p;
		p->next=var->next;
		var->next=NULL;
		free(var);
	}
}

void del_var_desc(struct VarDesc* var)
{
	//reg_list[var->reg_num].is_free=1;
	if(var == var_head)
	{
		var_head=var_head->next;
	}
	else
	{
		struct VarDesc* p = var_head;
		while(p!=NULL)
		{
			if(p->next==var)
				break;
			p=p->next;
		}
		if(var_cur == var)
			var_cur = p;
		p->next=var->next;
		var->next=NULL;
		//free(var);
	}
}

void add_var_desc(int reg_num, Operand op)
{

	struct VarDesc* new_var = (struct VarDesc*)malloc(sizeof(struct VarDesc));
	new_var->op=op;
	new_var->reg_num=reg_num;
	new_var->next=NULL;
	//reg_list[reg_num].is_free=0;
	if(var_head==NULL)
	{
		var_head = new_var;
		var_cur=new_var;
	}
	else
	{
		var_cur->next=new_var;
		var_cur=new_var;
	}
}

int allocate(Operand op)
{
	//若有空闲寄存器，则存进去
	for(int i=8;i<26;i++)
	{
		if(reg_list[i].is_free)
		{
			reg_list[i].is_free=0;
			add_var_desc(i,op);
			return i;
		}
	}

	//遍历变量描述符，立即数和临时变量直接释放
	struct VarDesc* p = var_head;
	while(p!=NULL)
	{
		if(p->op->kind==OP_CONSTANT && p->reg_num != last_changed_reg)
		{
			last_changed_reg = p->reg_num;
			//printf("溢出1  ");
			//printf("reg: %s, constant -> %s\n",reg_list[p->reg_num].name, op->u.name);
			int num = p->reg_num;
			del_var_desc(p);
			add_var_desc(num,op);
			return num;
		}
		p=p->next;
	}
	
	p = var_head;
	while(p!=NULL)
	{
		if(p->op->kind!=OP_CONSTANT)
		{
			if(p->op->u.name[0]=='t' && last_changed_reg != p->reg_num)
			{
				last_changed_reg = p->reg_num;
			//printf("溢出2  ");
				//printf("reg: %s,  %s -> %s\n",reg_list[p->reg_num].name, p->op->u.name, op->u.name);
				int num = p->reg_num;
				del_var_desc(p);
				add_var_desc(num,op);
				return num;
			}
		}
		p=p->next;
	}
}

int ensure(FILE* fp, Operand op)
{
	if(op->kind != OP_CONSTANT)
	{
		//若为变量，则查看变量描述符中是否已经存在，存在则直接返回寄存器，否则新分配一个寄存器
		struct VarDesc* p = var_head;
		while(p!=NULL)
		{
			if(p->op->kind != OP_CONSTANT && strcmp(p->op->u.name,op->u.name)==0)
				return p->reg_num;
			p=p->next;
		}
		int result = allocate(op);
		//printf("  lw %s, %s\n",reg_list[result].name,op->u.name);
		return result;
	}
	else
	{
		int result = allocate(op);
		fprintf(fp, "  li %s, %d\n", reg_list[result].name,op->u.value);
		return result;
	}
}

void pusha(FILE* fp)
{
	fprintf(fp, "  addi $sp, $sp, -72\n");
	for(int i=8;i<26;i++)
	{
		fprintf(fp, "  sw %s, %d($sp)\n", reg_list[i].name,(i-8)*4);
	}
}

void popa(FILE* fp)
{
	for(int i=8;i<26;i++)
	{
		fprintf(fp, "  lw %s, %d($sp)\n", reg_list[i].name,(i-8)*4);
	}
	fprintf(fp, "  addi $sp, $sp, 72\n");
}

void init_code(FILE* fp)
{
	var_head=NULL;
	var_cur=NULL;
	var_head_in_mem=NULL;
	var_cur_in_mem=NULL;
	for(int i=0;i<32;i++)
		reg_list[i].is_free=1;
	reg_list[0].name = "$zero";
	reg_list[1].name = "$at";	
	reg_list[2].name = "$v0";
	reg_list[3].name = "$v1";
	reg_list[4].name = "$a0";
	reg_list[5].name = "$a1";
	reg_list[6].name = "$a2";
	reg_list[7].name = "$a3";

	//$t0至$t9可任意使用,属于调用者保存的寄存器,在函数调用之前要先溢出到内存中。
	//$s0至$s7可任意使用,是被调用者保存的寄存器,函数内修改$s0至$s7,需在函数开头将原有数据压入栈,并在函数末尾恢复这些数据。

	/*for(int i=0;i<8;i++)
	{
		reg_list[8+i].name=(char*)malloc(4);
		sprintf(reg_list[8+i].name, "$t%d",i);
	}

	for(int i=0;i<8;i++)
	{
		reg_list[16+i].name=(char*)malloc(4);
		sprintf(reg_list[16+i].name, "$s%d",i);
	}*/

	reg_list[8].name = "$t0";
	reg_list[9].name = "$t1";
	reg_list[10].name = "$t2";
	reg_list[11].name = "$t3";
	reg_list[12].name = "$t4";
	reg_list[13].name = "$t5";
	reg_list[14].name = "$t6";
	reg_list[15].name = "$t7";

	reg_list[16].name = "$s0";
	reg_list[17].name = "$s1";
	reg_list[18].name = "$s2";
	reg_list[19].name = "$s3";
	reg_list[20].name = "$s4";
	reg_list[21].name = "$s5";
	reg_list[22].name = "$s6";
	reg_list[23].name = "$s7";		

	reg_list[24].name = "$t8";
	reg_list[25].name = "$t9";

	reg_list[26].name = "$k0";
	reg_list[27].name = "$k1";
	reg_list[28].name = "$gp";
	reg_list[29].name = "$sp";
	reg_list[30].name = "$fp";
	reg_list[31].name = "$ra";

	//数据段给数组分配空间
	fprintf(fp, ".data\n");
	fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(fp, "_ret: .asciiz \"\\n\"\n");
	fprintf(fp, ".globl main\n");
	struct InterCodes* x = code_head;
    while(x != NULL)
    {
    	if(x->code.kind==IR_DEC)
    	{
    		fprintf(fp, "%s: .word %d\n",x->code.u.dec.op->u.name, x->code.u.dec.size);
    	} 			
        x = x->next;
    }
	
	fprintf(fp, ".text\n");
	fprintf(fp, "read:\n");
	fprintf(fp, "  li $v0, 4\n");
	fprintf(fp, "  la $a0, _prompt\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  li $v0, 5\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  jr $ra\n\n");

	fprintf(fp, "write:\n");
	fprintf(fp, "  li $v0, 1\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  li $v0, 4\n");
	fprintf(fp, "  la $a0, _ret\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  move $v0, $0\n");
	fprintf(fp, "  jr $ra\n");//返回地址
}

void print_per_object_code(struct InterCodes* x, FILE* fp)
{
	//printf("%d\n",x->code.kind);
	if(x->code.kind == IR_LABEL)
    {
    	fprintf(fp, "%s:\n",x->code.u.one.op->u.name);
	}
	else if(x->code.kind == IR_FUNCTION)
	{
	   	fprintf(fp, "\n%s:\n",x->code.u.one.op->u.name);
	   	//进入新函数，寄存器重新变为可用状态
	   	for(int i=8;i<26;i++)
		{
			reg_list[i].is_free=1;
		}
		//清空变量描述符
		var_head=NULL;
		var_cur=NULL;
		var_head_in_mem=NULL;
		var_cur_in_mem=NULL;
		
		//执行IR_param
		if(strcmp(x->code.u.one.op->u.name,"main")==0)
		{
			in_func=0;
			cur_fun_name=NULL;
		}
		else
		{
			in_func=1;
			cur_fun_name=x->code.u.two.right->u.name;
			struct Symbol_table_item * temp = find_items_s_table(x->code.u.two.right->u.name);
			//将形参添加到变量描述符表
			int t=0;
			struct InterCodes* tmp = x->next;
			while(tmp != NULL && tmp->code.kind==IR_PARAM)
			{ 			
				if(t<4)
					add_var_desc(4+t, tmp->code.u.one.op);
				else
				{
					int reg_num = ensure(fp, tmp->code.u.one.op);
					fprintf(fp, "  lw %s, %d($fp)\n", reg_list[reg_num].name, (temp->f->type->u.function.paramNum-1-t)*4);
					add_var_desc(reg_num, tmp->code.u.one.op);
				}
				t++;
				tmp = tmp->next;
			}
		}
	}
	else if(x->code.kind == IR_ASSIGN)
    {
    	Operand left = x->code.u.two.left;
    	Operand right = x->code.u.two.right;  
    	if(right->kind == OP_CONSTANT)
    	{
    		int left_num = ensure(fp, left);
    		fprintf(fp, "  li %s, %d\n", reg_list[left_num].name, right->u.value);
    	} 	
    	else
    	{
    		int left_num = ensure(fp, left);
    		int right_num = ensure(fp, right);
    		fprintf(fp, "  move %s, %s\n", reg_list[left_num].name, reg_list[right_num].name);   		
    	}
	}
	else if(x->code.kind == IR_ADD)
    {
    	//print_var();
    	Operand result = x->code.u.three.result;
    	Operand op1 = x->code.u.three.op1;
    	Operand op2 = x->code.u.three.op2;
		if(op1->kind == OP_CONSTANT && op2->kind == OP_CONSTANT)
		{
			int res_num = ensure(fp, result);
			fprintf(fp, "  li %s, %d\n", reg_list[res_num].name, op1->u.value+op2->u.value);
		}   
    	else if(op1->kind != OP_CONSTANT && op2->kind == OP_CONSTANT)
    	{
 			int r1_num = ensure(fp, op1);
			int res_num = ensure(fp, result);
			fprintf(fp, "  addi %s, %s, %d\n",reg_list[res_num].name,reg_list[r1_num].name,op2->u.value);   		
    	}
    	else
    	{
			int r1_num = ensure(fp, op1);
			int r2_num = ensure(fp, op2);
			int res_num = ensure(fp, result);
			fprintf(fp, "  add %s, %s, %s\n",reg_list[res_num].name,reg_list[r1_num].name,reg_list[r2_num].name);
    	}
    		//print_var();
	}
	else if(x->code.kind == IR_SUB)
    {	
    	Operand result = x->code.u.three.result;
    	Operand op1 = x->code.u.three.op1;
    	Operand op2 = x->code.u.three.op2;   
		if(op1->kind == OP_CONSTANT && op2->kind == OP_CONSTANT)
		{
			int res_num = ensure(fp, result);
			fprintf(fp, "  li %s, %d\n", reg_list[res_num].name, op1->u.value-op2->u.value);
		}   
    	else if(op1->kind != OP_CONSTANT && op2->kind == OP_CONSTANT)
    	{
 			int r1_num = ensure(fp, op1);
			int res_num = ensure(fp, result);
			//print_var();
			//printf("%s %d\n", op1->u.name, r1_num);
			fprintf(fp, "  addi %s, %s, %d\n",reg_list[res_num].name,reg_list[r1_num].name,-op2->u.value);   		
    	}
    	else
    	{
			int r1_num = ensure(fp, op1);
			int r2_num = ensure(fp, op2);
			int res_num = ensure(fp, result);
			fprintf(fp, "  sub %s, %s, %s\n",reg_list[res_num].name,reg_list[r1_num].name,reg_list[r2_num].name);
    	}	
	}
	else if(x->code.kind == IR_MUL)
	{
	//print_var();
		Operand result = x->code.u.three.result;
		Operand op1 = x->code.u.three.op1;
		Operand op2 = x->code.u.three.op2; 
		if(op1->kind == OP_CONSTANT && op2->kind == OP_CONSTANT)
		{
			int res_num = ensure(fp, result);
			fprintf(fp, "  li %s, %d\n", reg_list[res_num].name, op1->u.value*op2->u.value);
		}  
		else
		{
			int r1_num = ensure(fp, op1);
			int r2_num = ensure(fp, op2);
			int res_num = ensure(fp, result);
			fprintf(fp, "  mul %s, %s, %s\n",reg_list[res_num].name,reg_list[r1_num].name,reg_list[r2_num].name);
		}	
			//print_var();	
	}
	else if(x->code.kind == IR_DIV)
	{
    	Operand result = x->code.u.three.result;
    	Operand op1 = x->code.u.three.op1;
    	Operand op2 = x->code.u.three.op2;   
		int r1_num = ensure(fp, op1);
		int r2_num = ensure(fp, op2);
		int res_num = ensure(fp, result);
		fprintf(fp, "  div %s, %s\n",reg_list[r1_num].name,reg_list[r2_num].name);	
		fprintf(fp, "  mflo %s\n", reg_list[res_num].name);	
	}
    else if(x->code.kind == IR_GET_ADDR)
    {
    	int left_num = ensure(fp, x->code.u.two.left);
    	int right_num = ensure(fp, x->code.u.two.right);
    	fprintf(fp, "  la %s, %s\n", reg_list[right_num].name, x->code.u.one.op->u.name);
    	fprintf(fp, "  move %s, %s\n", reg_list[left_num].name, reg_list[right_num].name);
    }
	else if(x->code.kind == IR_INTO_ADDR_RIGHT)
	{
	    Operand left = x->code.u.two.left;
    	Operand right = x->code.u.two.right;
		int left_num = ensure(fp, left);
    	int right_num = ensure(fp, right);
    	fprintf(fp, "  lw %s, 0(%s)\n", reg_list[left_num].name, reg_list[right_num].name);  		
	}
	else if(x->code.kind == IR_INTO_ADDR_LEFT)
	{
	    Operand left = x->code.u.two.left;
    	Operand right = x->code.u.two.right;
		int left_num = ensure(fp, left);
    	int right_num = ensure(fp, right);
    	fprintf(fp, "  sw %s, 0(%s)\n", reg_list[right_num].name, reg_list[left_num].name);  	
	}
	else if(x->code.kind == IR_GOTO)
	{
		fprintf(fp, "  j %s\n",x->code.u.one.op->u.name);
	}
	else if(x->code.kind == IR_IF_GOTO)
	{
		Operand op_x = x->code.u.if_goto.x;
		Operand op_relop = x->code.u.if_goto.relop;
		Operand op_y = x->code.u.if_goto.y;
		Operand op_z = x->code.u.if_goto.z;
		
		int x_num = ensure(fp, op_x);
		int y_num = ensure(fp, op_y);
		//print_var();
		if(strcmp(op_relop->u.name,"==")==0)
		{
			fprintf(fp, "  beq %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);
		}
		else if(strcmp(op_relop->u.name,"!=")==0)
		{
			fprintf(fp, "  bne %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);			
		}
		else if(strcmp(op_relop->u.name,">")==0)
		{
			fprintf(fp, "  bgt %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);			
		}
		else if(strcmp(op_relop->u.name,"<")==0)
		{
			fprintf(fp, "  blt %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);			
		}
		else if(strcmp(op_relop->u.name,">=")==0)
		{
			fprintf(fp, "  bge %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);			
		}
		else if(strcmp(op_relop->u.name,"<=")==0)
		{
			fprintf(fp, "  ble %s, %s, %s\n",reg_list[x_num].name, reg_list[y_num].name, op_z->u.name);			
		}				
	}
	else if(x->code.kind == IR_RETURN)
	{
		if(x->code.u.one.op->kind == OP_CONSTANT && x->code.u.one.op->u.value==0)
			fprintf(fp, "  move $v0, $0\n");
		else
		{
			int reg_num = ensure(fp, x->code.u.one.op);
			fprintf(fp, "  move $v0, %s\n", reg_list[reg_num].name);
		}
		fprintf(fp, "  jr $ra\n");
		
		/*if(in_func == 1)
		{
			//返回原函数，恢复寄存器使用状态
		   	for(int i=8;i<8+used_reg;i++)
			{
				reg_list[i].is_free=0;
			}
			used_reg=0;
			
			//恢复变量描述符
			if(var_stack_head != NULL)
			{
				var_head = var_stack_head->var_head;
				var_cur = var_stack_head->var_cur;
				var_stack_head=var_stack_head->next;
			}
			
		}*/

	}
	else if(x->code.kind == IR_DEC)
	{
		//已在init函数中写好
	}
	else if(x->code.kind == IR_CALL)
	{
		struct Symbol_table_item * temp = find_items_s_table(x->code.u.two.right->u.name);	
		param_reg_num=4;
		
		int left_num = ensure(fp, x->code.u.two.left);
		fprintf(fp, "  addi $sp, $sp, -4\n");
		fprintf(fp, "  sw $ra, 0($sp)\n");	
		pusha(fp);

		//保存形参至内存,并形参放入通用寄存器，然后删除原寄存器，以防止相互赋值
		//只需要保存前4个形参
		if(in_func)
		{
			fprintf(fp, "  addi $sp, $sp, -%d\n", temp->f->type->u.function.paramNum*4);
			struct Symbol_table_item * tmp = find_items_s_table(cur_fun_name);
			for(int i=0;i<temp->f->type->u.function.paramNum;i++)
			{
				if(i>=tmp->f->type->u.function.paramNum)
					break;
				if(i<4)
				{
					fprintf(fp, "  sw %s, %d($sp)\n", reg_list[4+i].name, i*4);
					struct VarDesc* p = var_head;
					while(p!=NULL)
					{
						if(p->op->kind != OP_CONSTANT && p->reg_num==4+i)
							break;
						p=p->next;
					}
					Operand op = p->op;
					del_var_desc(p);
					add_var_desc_in_mem(op);
					int reg_num = ensure(fp, op);
					fprintf(fp, "  move %s, %s\n", reg_list[reg_num].name, reg_list[4+i].name);
				}
			}
		}
		
		//IR_ARG
		struct InterCodes* tt = x->prev;
		while(tt!= NULL && tt->code.kind==IR_ARG)
		{
			int reg_num = ensure(fp, tt->code.u.one.op);
			if(param_reg_num<8)
			{
				fprintf(fp, "  move %s, %s\n", reg_list[param_reg_num].name, reg_list[reg_num].name);
				param_reg_num++;
			}
			else
			{
				//参数大于4个压栈
				int reg_num = ensure(fp, tt->code.u.one.op);
				fprintf(fp, "  addi $sp, $sp, -4\n");
				fprintf(fp, "  sw %s, 0($sp)\n", reg_list[reg_num].name);
				fprintf(fp, "  move $fp, $sp\n");
				param_reg_num++;
			}	
			tt=tt->prev;	
		}
		
		fprintf(fp, "  jal %s\n",x->code.u.two.right->u.name);	
		
		//恢复栈指针
		if(param_reg_num>8)
			fprintf(fp, "  addi $sp, $sp, %d\n", 4*(param_reg_num-8));
		//恢复形参,并将其添加到变量描述符
		if(in_func)
		{
			struct Symbol_table_item * tmp = find_items_s_table(cur_fun_name);
			for(int i=0;i<temp->f->type->u.function.paramNum;i++)
			{
				if(i>=tmp->f->type->u.function.paramNum)
					break;
				if(i<4)
				{
					fprintf(fp, "  lw %s, %d($sp)\n", reg_list[4+i].name, i*4);
					struct VarDesc* p = var_head;
					while(p!=NULL)
					{
						if(p->op->kind != OP_CONSTANT && strcmp(var_head_in_mem->op->u.name, p->op->u.name)==0)
							break;
						p=p->next;
					}
					if(p!=NULL)
					{
						reg_list[p->reg_num].is_free=1;
						p->reg_num = 4+i;
					}
					else
					{
						add_var_desc(4+i,var_head_in_mem->op);
					}
					del_var_desc_in_mem(var_head_in_mem);
				}
			}
			fprintf(fp, "  addi $sp, $sp, %d\n", temp->f->type->u.function.paramNum*4);

		}
		popa(fp);
		fprintf(fp, "  lw $ra, 0($sp)\n");
		fprintf(fp, "  addi $sp, $sp, 4\n");	

		fprintf(fp, "  move %s, $v0\n", reg_list[left_num].name);
	}
	else if(x->code.kind == IR_READ)
	{
		fprintf(fp, "  addi $sp, $sp, -4\n");
		fprintf(fp, "  sw $ra, 0($sp)\n");
		fprintf(fp, "  jal read\n");
		fprintf(fp, "  lw $ra, 0($sp)\n");
		fprintf(fp, "  addi $sp, $sp, 4\n");
		int reg_num = ensure(fp, x->code.u.one.op);
		fprintf(fp, "  move %s, $v0\n", reg_list[reg_num].name);
	}
	else if(x->code.kind == IR_WRITE)
	{
		if(in_func==0)
		{
			int reg_num = ensure(fp, x->code.u.one.op);
			fprintf(fp, "  move $a0, %s\n", reg_list[reg_num].name);
			fprintf(fp, "  addi $sp, $sp, -4\n");
			fprintf(fp, "  sw $ra, 0($sp)\n");
			fprintf(fp, "  jal write\n");
			fprintf(fp, "  lw $ra, 0($sp)\n");
			fprintf(fp, "  addi $sp, $sp, 4\n");
		}
		else
		{
			//函数内调用，先将a0压栈
			int reg_num = ensure(fp, x->code.u.one.op);
			fprintf(fp, "  addi $sp, $sp, -8\n");
			fprintf(fp, "  sw $a0, 0($sp)\n");
			fprintf(fp, "  sw $ra, 4($sp)\n");
			fprintf(fp, "  move $a0, %s\n", reg_list[reg_num].name);			
			fprintf(fp, "  jal write\n");
			fprintf(fp, "  lw $a0, 0($sp)\n");
			fprintf(fp, "  lw $ra, 4($sp)\n");			
			fprintf(fp, "  addi $sp, $sp, 8\n");		
		}
	} 	
}

void print_object_code(FILE* fp)
{
	init_code(fp);
    struct InterCodes* x = code_head;
    while(x != NULL)
    {
    	print_per_object_code(x, fp);  			
        x = x->next;
    }
}
