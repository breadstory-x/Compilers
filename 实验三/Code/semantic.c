#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"node.h"
#include"semantic.h"

struct Symbol_table_item *s_table[HASH_TABLE_SIZE] = {NULL};
struct Symbol_table_item *struct_table[HASH_TABLE_SIZE] = {NULL};
struct Symbol_table_item *stack[STACK_DEEP] = {NULL};
struct Symbol_table_item *cur_depth_symbol[STACK_DEEP] = {NULL};
int cur_stack_deep=0;

char anonymous_struct_name[100] = "xwj";

void print_hash_table()
{
	printf("---------------hash_table----------------\n");
	for(int i=0;i<HASH_TABLE_SIZE;i++)
	{
		if(s_table[i] != NULL)
		{
			printf("[%d]  ",i);
			struct Symbol_table_item *temp=s_table[i];
			int num=0;
			while(temp!=NULL)
			{
				if(num == 10)
				 	break;
				printf("%s %d -> ",temp->f->name, temp->symbol_depth);
				temp=temp->next_hash_item;
				num++;
			}
			printf("\n");
		}
	}
	printf("---------------end----------------\n");
}	

unsigned int hash_pjw(char* name)
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if (i = val & ~0x3fff) 
			val = (val ^ (i >> 12)) & 0x3fff;
	}
	return val%HASH_TABLE_SIZE;
}

//查找表项
struct Symbol_table_item *find_items_s_table(char *name)
{
	int index = hash_pjw(name);
	if(s_table[index] == NULL)
		return NULL;
	else
	{
		struct Symbol_table_item* temp = s_table[index];
		while(temp != NULL)
		{
			if(strcmp(temp->f->name,name)==0)
				return temp;				
			temp = temp->next_hash_item;
		}
		return NULL;
	}
}

struct Symbol_table_item *find_items_struct_table(char *name)
{
	int index = hash_pjw(name);
	if(struct_table[index] == NULL)
		return NULL;
	else
	{
		struct Symbol_table_item* temp = struct_table[index];
		while(temp != NULL)
		{
			if(strcmp(temp->f->name,name)==0)
				return temp;				
			temp = temp->next_hash_item;
		}
		return NULL;
	}
}

//查看是否有冲突,查重定义
int search_s_table(struct Symbol_table_item *p)
{
	int index = hash_pjw(p->f->name);
	if(s_table[index] == NULL)
		return 0;
	else
	{
		struct Symbol_table_item* temp = s_table[index];
		while(temp != NULL)
		{
			if(strcmp(temp->f->name,p->f->name)==0 && temp->symbol_depth == cur_stack_deep)
			{
				if(temp->f->type->kind == FUNCTION && p->f->type->kind == FUNCTION)
					return 1;
				else if((temp->f->type->kind>=0 && temp->f->type->kind<=2) &&
						(p->f->type->kind>=0 && p->f->type->kind<=2))
					return 1;
			}	
			else if(strcmp(temp->f->name,p->f->name)==0 && temp->symbol_depth != cur_stack_deep)			
			{
				if(temp->f->type->kind == STRUCTURE)
					return 1;
			}
			temp = temp->next_hash_item;
		}
		return 0;
	}
}

//填表，默认没有重名
void add_s_table(struct Symbol_table_item *p)
{

	int index = hash_pjw(p->f->name);

	//十字连表，当前作用域指针
	if(stack[cur_stack_deep] == NULL)
	{
		cur_depth_symbol[cur_stack_deep] = p;
		stack[cur_stack_deep] = p;
	}
	else
	{
		cur_depth_symbol[cur_stack_deep]->next_symbol = p;
		cur_depth_symbol[cur_stack_deep] = p;
	}

	//填表，有重复键值则插入头部
	if(s_table[index] == NULL)
		s_table[index]=p;
	else
	{
		//printf("重复\n");
		struct Symbol_table_item *temp = s_table[index];
		p->next_hash_item = temp;
		s_table[index]=p;
	}
}

int search_struct_table(struct Symbol_table_item *p)
{
	int index = hash_pjw(p->f->name);
	if(struct_table[index] == NULL)
		return 0;
	else
	{
		struct Symbol_table_item* temp = struct_table[index];
		while(temp != NULL)
		{
			if(strcmp(temp->f->name,p->f->name)==0)
			{
				if(temp->f->type->kind == FUNCTION && p->f->type->kind == FUNCTION)
					return 1;
				else if((temp->f->type->kind>=0 && temp->f->type->kind<=2) &&
						(p->f->type->kind>=0 && p->f->type->kind<=2))
					return 1;
			}				
			temp = temp->next_hash_item;
		}
		return 0;
	}
}

void add_struct_table(struct Symbol_table_item *p)
{
	int index = hash_pjw(p->f->name);
	
	if(struct_table[index] == NULL)
		struct_table[index]=p;
	else
	{
		//printf("重复\n");
		struct Symbol_table_item *temp = struct_table[index];
		p->next_hash_item = temp;
		struct_table[index]=p;
	}
}
void del_s_table(struct Symbol_table_item *p)
{

	int index = hash_pjw(p->f->name);
	struct Symbol_table_item *cur=s_table[index];
	struct Symbol_table_item *last=s_table[index];

	while(cur != p)
	{
		last = cur;
		cur = cur->next_hash_item;
	}
	last->next_hash_item = cur->next_hash_item;
	if(cur != s_table[index])
		free(cur);
	else if(cur == s_table[index] && cur->next_hash_item == NULL)
		s_table[index] = NULL;
	else if(cur == s_table[index] && cur->next_hash_item != NULL)
		s_table[index] = cur->next_hash_item;
}

//初始化符号表：添加read、write函数
//其中read函数没有任何参数,返回值为int型(即读入的整数值),write
//函数包含一个int类型的参数(即要输出的整数值),返回值也为int型(固定返回0)。
void init_table()
{
	Type spe_type = (Type)malloc(sizeof(struct Type_));
	spe_type->kind=BASIC;
	spe_type->u.basic=VAR_INT;
	//添加read函数
	struct Symbol_table_item *p = (struct Symbol_table_item *)malloc(sizeof(struct Symbol_table_item));
	p->next_hash_item = NULL;
	p->next_symbol = NULL;
	p->symbol_depth = cur_stack_deep;
	p->f = (FieldList)malloc(sizeof(struct FieldList_));
	p->f->name="read";

	Type t = (Type)malloc(sizeof(struct Type_));
	t->kind = FUNCTION;
	t->u.function.funcType = spe_type;
	t->u.function.paramNum=0;
	t->u.function.params=NULL;
	p->f->type=t;
	add_s_table(p);
	//添加write函数
	struct Symbol_table_item *p2 = (struct Symbol_table_item *)malloc(sizeof(struct Symbol_table_item));
	p2->next_hash_item = NULL;
	p2->next_symbol = NULL;
	p2->symbol_depth = cur_stack_deep;
	p2->f = (FieldList)malloc(sizeof(struct FieldList_));
	p2->f->name="write";

	Type t2 = (Type)malloc(sizeof(struct Type_));
	t2->kind = FUNCTION;
	t2->u.function.funcType = spe_type;
	t2->u.function.paramNum=1;
	t2->u.function.params=(FieldList)malloc(sizeof(struct FieldList_));

	t2->u.function.params->name="params1";
	t2->u.function.params->type=(Type)malloc(sizeof(struct Type_));
	t2->u.function.params->type->kind=BASIC;
	t2->u.function.params->type->u.basic=VAR_INT;
	p2->f->type=t2;
	add_s_table(p2);
}

int check_type(Type type1, Type type2)//正确1  错误0
{
	if(type1 == NULL && type2 == NULL)
		return 1;
	else if(type1 == NULL || type2 == NULL)
	{
		//printf("NULL\n");
		return 0;
	}
	if(type1->kind != type2->kind)
		return 0;
	else
	{
		if(type1->kind == BASIC)//均为基本类型
		{
			if(type1->u.basic != type2->u.basic)
				return 0;
			else
				return 1;
		}
		if(type1->kind == ARRAY)
		{
			if(check_type(type1->u.array.elem,type2->u.array.elem)==0)
				return 0;
			else return 1;
		}
		if(type1->kind == STRUCTURE)
		{
			if(strcmp(type1->u.structure_.name, type2->u.structure_.name)==0)
				return 1;
			else return 0;
		}
		/*if(type1->kind == FUNCTION)
		{
		
		}
		*/
	}
}


struct Symbol_table_item * Dec_in_structure(struct Node* node, Type type)
{
	struct Node* child_1 = node->firstChild;//VarDec
	struct Symbol_table_item *p = VarDec(child_1, type);
	p->symbol_depth=STACK_DEEP;//结构体成员变量单独一张表
	if(child_1->nextSibling != NULL)
	{
		printf("Error type 15 at Line %d: illegal initialization.\n",node->col);
		//结构体变量不允许初始化
	}
	
	return p;
}


FieldList Def_in_structure(struct Node* node/*, FieldList temp_field, FieldList head_field*/)
{
	/*
		Def -> Specifier DecList SEMI
	*/
	Type spe_type = Specifier(node->firstChild);
	struct Node* Dec_List = node->firstChild->nextSibling;
	/*
		DecList -> Dec
		DecList -> Dec COMMA DecList
	*/
	struct Symbol_table_item *p = NULL;
	FieldList temp_field = NULL,head_field = NULL;
	//有逗号有变量则循环填表
	while(Dec_List->firstChild->nextSibling != NULL)
	{
		p=Dec_in_structure(Dec_List->firstChild,spe_type);
		if(search_struct_table(p))
			printf("Error type 15 at Line %d: Redefined field \"%s\".\n",node->col,p->f->name);
		else
		{
			//printf("[Def]添加结构体变量\n");
			add_struct_table(p);
			//print_hash_table();
		} 
		//与普通def的区别在于：需要把这些结构体变量串起来，类似函数形参
		if(head_field == NULL)
		{
			temp_field = p->f;
			head_field = p->f;
		}
		else
		{
			temp_field->tail = p->f; 
			temp_field = p->f;
		}
		Dec_List = Dec_List->firstChild->nextSibling->nextSibling;
	}
	
	
	p=Dec_in_structure(Dec_List->firstChild,spe_type);
	if(search_struct_table(p))
		printf("Error type 15 at Line %d: Redefined field \"%s\".\n",node->col,p->f->name);
	else 
	{
		//printf("[Def]添加结构体变量\n");
		add_struct_table(p);
		//print_hash_table();
	} 
	
	if(head_field == NULL)
	{
		temp_field = p->f;
		head_field = p->f;
	}
	else
	{
		temp_field->tail = p->f; 
		temp_field = p->f;
	}
	return head_field;
}


Type Specifier(struct Node* node)
{
	Type spe_type=(Type)malloc(sizeof(struct Type_));
	if(strcmp(node->firstChild->name,"TYPE")==0){//TYPE
        spe_type->kind=BASIC;
        if(strcmp(node->firstChild->text,"int")==0)
            spe_type->u.basic=VAR_INT;
        else spe_type->u.basic=VAR_FLOAT;
        return spe_type;
    }
    else
    {
    	/*	
    		StructSpecifier -> STRUCT OptTag LC DefList RC
    		StructSpecifier -> STRUCT Tag
    	*/
    	/*
    		Tag -> ID
    	*/
    	spe_type->kind=STRUCTURE;
    	struct Node* struct_specifier = node->firstChild;
    	if(strcmp(struct_specifier->firstChild->nextSibling->name,"Tag")==0)
    	{
    		struct Symbol_table_item *p = find_items_s_table(struct_specifier->firstChild->nextSibling->firstChild->text);
    		if(p == NULL || p->f->type->kind != STRUCTURE)
    		{
    			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", node->col, struct_specifier->firstChild->nextSibling->firstChild->text);
				return NULL;
    		}
    		else if(p->f->type != NULL)
    			return p->f->type;
    		spe_type->u.structure_.structure = NULL;
    		return spe_type;
    	}
    	else
    	{
    		struct Node* def_list = NULL;
    		struct Symbol_table_item *p = (struct Symbol_table_item *)malloc(sizeof(struct Symbol_table_item));
			p->next_hash_item = NULL;
			p->next_symbol = NULL;
			p->symbol_depth = cur_stack_deep;
			p->f = (FieldList)malloc(sizeof(struct FieldList_));
			p->f->tail= NULL;
    		spe_type->u.structure_.structure = NULL;
    		if(strcmp(struct_specifier->firstChild->nextSibling->name, "OptTag")==0)
    		{
    			p->f->name = struct_specifier->firstChild->nextSibling->firstChild->text;
    			spe_type->u.structure_.name = p->f->name;
    			p->f->type = spe_type;
    			if(search_s_table(p))
    			{
    				printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",node->col,struct_specifier->firstChild->nextSibling->firstChild->text);
    				return NULL;
    			}
    			def_list = struct_specifier->firstChild->nextSibling->nextSibling->nextSibling;
    			
    		}
    		else
    		{
    			def_list = struct_specifier->firstChild->nextSibling->nextSibling;
				
				strcat(anonymous_struct_name,"1");
    			p->f->name = anonymous_struct_name;
    			spe_type->u.structure_.name = anonymous_struct_name;
    		}
    		
			//空结构体
			if(strcmp(def_list->name,"DefList")!=0)
			{
				spe_type->u.structure_.structure = NULL;
			}
			else
			{
				FieldList temp_field = NULL;
				FieldList head_field = NULL;
				while(def_list != NULL)
				{
					FieldList ret_field = Def_in_structure(def_list->firstChild);
					if(temp_field == NULL)
					{
						temp_field = ret_field;
						head_field = ret_field;
					}
					else
					{
						while(temp_field->tail!= NULL)
							temp_field = temp_field->tail;
						temp_field->tail = ret_field;
						temp_field = ret_field;
					}
					def_list = def_list->firstChild->nextSibling;
				}
				
				spe_type->u.structure_.structure = head_field;
			}

			
			p->f->type = spe_type;
			add_s_table(p);
			//printf("%s\n",p->f->name);
			/*FieldList test = p->f->type->u.structure_.structure;
			if(test == NULL)
				printf("aaaaaaaaaaaaaaaaaaaaaaaaa\n");
			while(test != NULL)
			{
				printf("%s\n", test->name);
				test=test->tail;
			}*/
			
			//printf("[Specifier]添加结构体\n");
			//print_hash_table();
			return spe_type;
    	}
    }

}


struct Symbol_table_item *VarDec(struct Node* node, Type type)
{
	//printf("VarDec\n");
	struct Symbol_table_item *p = (struct Symbol_table_item *)malloc(sizeof(struct Symbol_table_item));
	p->next_hash_item = NULL;
	p->next_symbol = NULL;
	p->symbol_depth = cur_stack_deep;
	p->f = (FieldList)malloc(sizeof(struct FieldList_));
	p->f->tail = NULL;
	//Type top = (Type)malloc(sizeof(struct Type_));
	//top->kind = BASIC;
	//top->u.basic = type;
	//先获取该变量名称
	struct Node* VarDec_child_1 = node->firstChild;

	while(VarDec_child_1->firstChild != NULL)
	{
		VarDec_child_1 = VarDec_child_1->firstChild;
	}
	p->f->name=VarDec_child_1->text;
	VarDec_child_1 = node->firstChild;

	//基本类型
	if(strcmp(VarDec_child_1->name, "ID")==0)
	{
//printf("%d\n",type->u.structure_.structure->type->u.array.elem->kind);
//printf("%s\n",p->f->name);
		p->f->type = type;
		//printf("exit VarDec\n");
		return p;
	}
	
	//printf("%d\n",type->kind);
	//数组类型
	Type type_temp[100] = {NULL};
	int next = 0;
	while(VarDec_child_1->firstChild != NULL)
	{
		Type var1=(Type)malloc(sizeof(struct Type_));
		var1->kind = ARRAY;
		var1->u.array.size = atoi(VarDec_child_1->nextSibling->nextSibling->text);
		if(next==0)
		{
			var1->u.array.elem = type;
		}
		else
			var1->u.array.elem = type_temp[next-1];
		type_temp[next] = var1;

		next++;
		VarDec_child_1 = VarDec_child_1->firstChild;
		//if(VarDec_child_1->firstChild == NULL)
		//	var1->u.array.elem = type;
	}
	p->f->type = type_temp[next-1];
	
	/*Type xx = p->f->type;
	while(xx != NULL)
	{
		printf("%d\n", xx->kind);
		if(xx->kind == ARRAY)
			xx =xx->u.array.elem;
		else
			break;
	}*/
	//printf("exit VarDec\n");
	return p;
		
}


Type Exp(struct Node* node)
{
	/*	Exp -> Exp ASSIGNOP Exp		
			| Exp AND Exp			
			| Exp OR Exp			
			| Exp RELOP Exp			
			| Exp PLUS Exp			
			| Exp MINUS Exp			
			| Exp STAR Exp				
			| Exp DIV Exp			
			| LP Exp RP				
			| MINUS Exp				
			| NOT Exp					
			| ID LP Args RP			
			| ID LP RP				
			| Exp LB Exp RB			
			| Exp DOT ID
			| ID					
			| INT					
			| FLOAT					
	*/
	//printf("[Exp]\n");
	if(strcmp(node->firstChild->name, "Exp")==0)
	{
		Type type_1 = Exp(node->firstChild);
		if(strcmp(node->firstChild->nextSibling->name, "ASSIGNOP")==0)
		{
			Type type_3 = Exp(node->firstChild->nextSibling->nextSibling);
			
			//左值三种情况：ID、Exp LB Exp RB、Exp DOT ID
			if(node->firstChild->firstChild->nextSibling == NULL && strcmp(node->firstChild->firstChild->name,"ID")!=0)			// 1个子节点
			{
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->col);
				return NULL;	
			}
			else if(node->firstChild->firstChild->nextSibling != NULL && node->firstChild->firstChild->nextSibling->nextSibling != NULL && node->firstChild->firstChild->nextSibling->nextSibling->nextSibling == NULL) //3个子节点
			{
				if(!(strcmp(node->firstChild->firstChild->name,"Exp")==0 && strcmp(node->firstChild->firstChild->nextSibling->name,"DOT")==0 && strcmp(node->firstChild->firstChild->nextSibling->nextSibling->name,"ID")==0))
				{
					printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->col);
					return NULL;
				}
			}
			else if(node->firstChild->firstChild->nextSibling != NULL && node->firstChild->firstChild->nextSibling->nextSibling != NULL &&  node->firstChild->firstChild->nextSibling->nextSibling->nextSibling != NULL && node->firstChild->firstChild->nextSibling->nextSibling->nextSibling->nextSibling == NULL) //4个子节点
			{
				if(!(strcmp(node->firstChild->firstChild->name,"Exp")==0 && 
					strcmp(node->firstChild->firstChild->nextSibling->name,"LB")==0 && 
					strcmp(node->firstChild->firstChild->nextSibling->nextSibling->name,"Exp")==0 && 
					strcmp(node->firstChild->firstChild->nextSibling->nextSibling->nextSibling->name,"RB")==0))
				{
					printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", node->col);
					return NULL;				
				}
			}
			if(check_type(type_1, type_3)==0)
			{
				printf("Error type 5 at Line %d: Type mismatched for assignment.\n", node->col);
				return NULL;
			}
			else
				return type_1;
		}
		else if(strcmp(node->firstChild->nextSibling->name, "AND")==0 ||
			strcmp(node->firstChild->nextSibling->name, "OR")==0 ||
			strcmp(node->firstChild->nextSibling->name, "RELOP")==0)
		{
			Type type_3 = Exp(node->firstChild->nextSibling->nextSibling);
			if(check_type(type_1, type_3)==0)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->col);
				return NULL;
			}
			else
			{
				Type type=(Type)malloc(sizeof(struct Type_));
				type->kind=BASIC;
				type->u.basic=VAR_INT;
		  	    return type;
	    	}
		}
		else if(strcmp(node->firstChild->nextSibling->name, "PLUS")==0 ||
			strcmp(node->firstChild->nextSibling->name, "MINUS")==0 ||
			strcmp(node->firstChild->nextSibling->name, "STAR")==0 ||
			strcmp(node->firstChild->nextSibling->name, "DIV")==0)
		{
			Type type_3 = Exp(node->firstChild->nextSibling->nextSibling);
			if(check_type(type_1, type_3)==0)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", node->col);
				return NULL;
			}
			else
				return type_1;
		}
		else if(strcmp(node->firstChild->nextSibling->name, "LB")==0)
		{
			
			struct Node* child_1 = node->firstChild;
			struct Node* child_3 = child_1->nextSibling->nextSibling;
			Type child_1_type = Exp(child_1);
			Type child_3_type = Exp(child_3);
			//printf("%d\n",child_1_type->kind);
			if(child_1_type == NULL)
				return NULL;
			if(child_1_type != NULL && child_1_type->kind != ARRAY)
			{
			    printf("Error type 10 at Line %d: Illegal use of \"[]\"\n",node->col);
				return NULL;
			}
			if(child_3_type->kind != BASIC || child_3_type->u.basic == VAR_FLOAT)
			{
				printf("Error type 12 at Line %d: Array index is not an integer.\n",node->col);
				return NULL;
			}
			
			return child_1_type->u.array.elem;
		}
		else if(strcmp(node->firstChild->nextSibling->name, "DOT")==0)
		{
			Type stru = type_1;
			
			if(stru == NULL)
				return NULL;
			if(stru->kind != STRUCTURE)
			{
				printf("Error type 13 at Line %d: Illegal use of \".\".\n",node->col);
				return NULL;
			}
			struct Node* id = node->firstChild->nextSibling->nextSibling;
			char *id_name = id->text;
			
			FieldList temp=stru->u.structure_.structure;
		    while(temp!=NULL)
		    {
		        if(strcmp(temp->name,id_name)==0)
		            return temp->type;
		        temp=temp->tail;
		    }
			printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",node->col,id_name);
        	return NULL;
		}
		
	}
	else
	{
		if(strcmp(node->firstChild->name, "LP")==0 || strcmp(node->firstChild->name, "MINUS")==0 ||strcmp(node->firstChild->name, "NOT")==0)
		{
			return Exp(node->firstChild->nextSibling);
		}
		if(strcmp(node->firstChild->name, "ID")==0 && node->firstChild->nextSibling == NULL)
		{
			struct Symbol_table_item *temp = find_items_s_table(node->firstChild->text);
			if(temp == NULL)
			{
			    printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",node->col,node->firstChild->text);
				return NULL;
			}
			else
				return temp->f->type;
		}
		if(strcmp(node->firstChild->name, "INT")==0)
		{
			Type type = (Type)malloc(sizeof(struct Type_));
			type->kind = BASIC;
			type->u.basic = VAR_INT;
			return type;
		}
		if(strcmp(node->firstChild->name, "FLOAT")==0)
		{
			Type type = (Type)malloc(sizeof(struct Type_));
			type->kind = BASIC;
			type->u.basic = VAR_FLOAT;
			return type;
		}
		if(strcmp(node->firstChild->name, "ID")==0 && node->firstChild->nextSibling != NULL)
		{
			//函数调用
			//printf("%s\n",node->name);
			struct Node* id = node->firstChild;
			struct Symbol_table_item *func_item = find_items_s_table(id->text);
			if(func_item == NULL)
			{
			    printf("Error type 2 at Line %d: Undefined function \"%s\".\n",id->col,id->text);
				return NULL;
			}
			if(func_item->f->type->kind != FUNCTION)
			{
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n",id->col, id->text);
				return NULL;
			}
			
			//获取参数列表
			FieldList real_args_type = func_item->f->type->u.function.params;
			if(strcmp(id->nextSibling->nextSibling->name, "Args")==0)
			{
				struct Node* args = id->nextSibling->nextSibling;
				int param_num=1;
				while(args->firstChild->nextSibling != NULL)
				{
					param_num++;
					Type args_type = Exp(args->firstChild);
					if(real_args_type != NULL && !check_type(args_type,real_args_type->type))
					{
						printf("Error type 9 at Line %d: Type mismatched for arguments.\n",node->col);	
						return NULL;
					}		
					args = args->firstChild->nextSibling->nextSibling;
					real_args_type = real_args_type==NULL?NULL:real_args_type->tail;
				}
		
				Type args_type = Exp(args->firstChild);
				if(real_args_type != NULL && !check_type(args_type,real_args_type->type))
				{
					printf("Error type 9 at Line %d: Type mismatched for arguments.\n",node->col);
					return NULL;
				}	
				
				if(param_num != func_item->f->type->u.function.paramNum)
				{
					printf("Error type 9 at Line %d: The number of arguments is wrong.\n",node->col);
					return NULL;
				}		
			}
			return func_item->f->type->u.function.funcType;
		}
	}	
	return NULL;
}


struct Symbol_table_item *Dec(struct Node* node, Type type)
{
	//printf("[Dec]\n");
	struct Node* child_1 = node->firstChild;//VarDec
	struct Symbol_table_item *p = VarDec(child_1, type);

	if(child_1->nextSibling != NULL)
	{
		struct Node* child_3 = child_1->nextSibling->nextSibling;//Exp
		Type exp_type = Exp(child_3);
		if(check_type(p->f->type, exp_type)==0)
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", node->col);
		//判断Exp和VarDec类型相等
	}
	
	return p;
}

void Def(struct Node* node)
{
	/*
		Def -> Specifier DecList SEMI
	*/
	Type spe_type = Specifier(node->firstChild);
	struct Node* Dec_List = node->firstChild->nextSibling;
	/*
		DecList -> Dec
		DecList -> Dec COMMA DecList
	*/
	struct Symbol_table_item *p = NULL;
	//有逗号有变量则循环填表
	while(Dec_List->firstChild->nextSibling != NULL)
	{
		p=Dec(Dec_List->firstChild,spe_type);
		if(search_s_table(p))
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
		else
		{
			//printf("[Def]添加局部变量: %s\n",p->f->name);
			add_s_table(p);
			//print_hash_table();
		} 
		    
		Dec_List = Dec_List->firstChild->nextSibling->nextSibling;
	}
	
	p=Dec(Dec_List->firstChild,spe_type);
	//printf("%d\n",p->symbol_depth);
	if(search_s_table(p))
		printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
	else 
	{
		//printf("[Def]添加局部变量: %s\n",p->f->name);
		//printf("%d\n",p->f->type->u.structure_.structure->type->u.array.elem->kind);
		add_s_table(p);
		//print_hash_table();
	} 
}


void Stmt(struct Node* node, Type function_type)
{
	if(strcmp(node->firstChild->name,"Exp")==0)
		Exp(node->firstChild);
	else if(strcmp(node->firstChild->name,"CompSt")==0)
		CompSt(node->firstChild, function_type);
	else if(strcmp(node->firstChild->name,"RETURN")==0)
	{
		Type ret_type = Exp(node->firstChild->nextSibling);
		
		if(!check_type(function_type, ret_type))
			printf("Error type 8 at Line %d: Type mismatched for return.\n",node->col);
	}
	else if(strcmp(node->firstChild->name,"IF")==0)
	{
		struct Node* exp = node->firstChild->nextSibling->nextSibling;
		struct Node* stmt = exp->nextSibling->nextSibling;
		Exp(exp);
		Stmt(stmt, function_type);	
		if(stmt->nextSibling != NULL)
			Stmt(stmt->nextSibling->nextSibling, function_type);
	}
	else if(strcmp(node->firstChild->name,"WHILE")==0)
	{
		struct Node* exp = node->firstChild->nextSibling->nextSibling;
		struct Node* stmt = exp->nextSibling->nextSibling;
		Exp(exp);
		Stmt(stmt, function_type);
	
	}
}


void CompSt(struct Node* node, Type function_type)
{
	/*
		CompSt -> LC DefList StmtList RC
	*/
	struct Node* CompSt_child_1 = node->firstChild;//LC
	//cur_stack_deep++;
	
	struct Node* CompSt_child_2 = CompSt_child_1->nextSibling;//DefList
	struct Node* CompSt_child_3 = CompSt_child_2->nextSibling;//StmtList
	if(strcmp(CompSt_child_2->name,"DefList")==0)//DefList不为空
	{
		if(strcmp(CompSt_child_3->name,"StmtList")!=0)
			CompSt_child_3 = NULL;
	}
	else
	{
		CompSt_child_3 = CompSt_child_2;
		CompSt_child_2 = NULL;
		if(strcmp(CompSt_child_3->name,"StmtList")!=0)
			CompSt_child_3 = NULL;
	}

	/*
		DefList -> Def DefList
		DefList -> ε
	*/
		
	while(CompSt_child_2 != NULL)
	{
		Def(CompSt_child_2->firstChild);
		CompSt_child_2 = CompSt_child_2->firstChild->nextSibling;
	}
	/*
		StmtList -> Stmt StmtList
		StmtList -> ε
	*/

	while(CompSt_child_3 != NULL)
	{
		Stmt(CompSt_child_3->firstChild, function_type);
		CompSt_child_3 = CompSt_child_3->firstChild->nextSibling;
	}

	//退出大括号，删除该作用域的全部符号
	/*struct Symbol_table_item *temp = stack[cur_stack_deep];

	while(temp != NULL)
	{
		struct Symbol_table_item *t = temp;
		temp = temp->next_symbol;
		del_s_table(t);
	}

	stack[cur_stack_deep] = NULL;
	cur_stack_deep--;*/
}



void ExtDef(struct Node* node)
{
	/*
		ExtDef -> Specifier ExtDecList SEMI
	  	ExtDef -> Specifier SEMI
	  	ExtDef -> Specifier FunDec CompSt
	*/
	//printf("ExtDef\n");
	struct Node* child_1 = node->firstChild; //Specifier
	Type spe_type =Specifier(child_1);
	struct Node* child_2 = child_1->nextSibling;//ExtDecList或FunDec
	if(strcmp(child_2->name,"ExtDecList")==0)//全局变量定义
	{
		//char *type_name[100];
		//strcpy(type_name, child_1->firstChild->text);
		
		struct Node* ExtDec_List = child_2;
		struct Symbol_table_item *p = NULL;
		//有逗号有变量则循环填表
		while(ExtDec_List->firstChild->nextSibling != NULL)
		{
			p=VarDec(ExtDec_List->firstChild,spe_type);
			if(search_s_table(p))
				printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
		    else
		    {
		    	//printf("[ExtDef]添加全局变量\n");
		    	add_s_table(p);
		    	//print_hash_table();
		    } 
		    	
		    ExtDec_List = ExtDec_List->firstChild->nextSibling->nextSibling;
		}
		
		p=VarDec(ExtDec_List->firstChild,spe_type);
		if(search_s_table(p))
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
        else 
        {
		    //printf("[ExtDef]添加全局变量\n");
		    add_s_table(p);
		    //print_hash_table();
		} 
	}	
	else if(strcmp(child_2->name,"FunDec")==0)//函数定义
	{
		/*
			FunDec -> ID LP VarList RP
			FunDec -> ID LP RP
		*/
		//处理函数头
		struct Symbol_table_item *p = (struct Symbol_table_item *)malloc(sizeof(struct Symbol_table_item));
		p->next_hash_item = NULL;
		p->next_symbol = NULL;
		p->symbol_depth = cur_stack_deep;
		p->f = (FieldList)malloc(sizeof(struct FieldList_));
		
		p->f->name = child_2->firstChild->text;
		Type t = (Type)malloc(sizeof(struct Type_));
		t->kind = FUNCTION;
		t->u.function.funcType = spe_type;
		t->u.function.paramNum=0;
		t->u.function.params=NULL;
		
		if(strcmp(child_2->firstChild->nextSibling->nextSibling->name, "VarList")==0)
		{
			struct Node *Var_List = child_2->firstChild->nextSibling->nextSibling;
			
			//cur_stack_deep++;
			
			struct Symbol_table_item *pp = NULL;
			//有多个参数则循环填表
			int par_num=1;
			FieldList field_root = NULL;
			FieldList temp_field = NULL;
			while(Var_List->firstChild->nextSibling != NULL)
			{
				par_num++;
				struct Node *Param_Dec = Var_List->firstChild;
				Type spe_type_1 = Specifier(Param_Dec->firstChild);
				pp=VarDec(Param_Dec->firstChild->nextSibling,spe_type_1);
				if(search_s_table(pp))
					printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
				else 
				{
					//printf("[ExtDef]添加函数形参\n");
					if(temp_field == NULL)
					{
						field_root = pp->f;
						temp_field = pp->f;
					}	
					else
					{
						temp_field -> tail = pp->f;
						temp_field = pp->f;
					}
					add_s_table(pp);
					//print_hash_table();
				} 
				Var_List = Var_List->firstChild->nextSibling->nextSibling;
			}
			

			struct Node *Param_Dec = Var_List->firstChild;
			Type spe_type_1 = Specifier(Param_Dec->firstChild);
			pp=VarDec(Param_Dec->firstChild->nextSibling,spe_type_1);
			if(search_s_table(pp))
				printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->col,p->f->name);
			else 
			{
				//printf("[ExtDef]添加函数形参\n");
				add_s_table(pp);
				if(temp_field == NULL)
				{
					temp_field = pp->f;
					field_root = pp->f;
				}
				else
				{
					temp_field -> tail = pp->f;
					temp_field = pp->f;
				}
				//print_hash_table();
			} 
				
			//由于函数体还要用到，因此不删除此层符号表
			//cur_stack_deep--;
			t->u.function.params = field_root;
			t->u.function.paramNum=par_num;
		}
		
		p->f->type = t;
		if(search_s_table(p))
			printf("Error type 4 at Line %d: Redefined function \"%s\".\n",node->col,p->f->name);
        else 
        {
			//printf("[ExtDef]添加全局函数\n");
			add_s_table(p);
			//print_hash_table();
		} 
        	
        //处理函数体
        //printf("处理函数体\n");
        struct Node* child_3 = child_2->nextSibling;//CompSt
        CompSt(child_3, p->f->type->u.function.funcType);//传入函数返回值类型
		
	}
	else //无意义或结构体定义
	{

	}
}

void traverse(struct Node* this_node,int depth)
{
	if(this_node == NULL)
		return;

	if(strcmp(this_node->name,"ExtDef")==0)
		ExtDef(this_node);
	
	traverse(this_node->firstChild,depth+1);
	traverse(this_node->nextSibling,depth);
}
