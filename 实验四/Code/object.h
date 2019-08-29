#ifndef _OBJECT_H_
#define _OBJECT_H_

struct RegisterDesc
{
	int is_free;//是否空闲
	char *name;//寄存器名称
};

struct VarDesc
{
	int reg_num;//该变量存放的寄存器编号
	Operand op;//变量信息
	struct VarDesc* next;
};

struct RegisterDesc reg_list[32];

struct VarDesc* var_head;		//变量描述符表
struct VarDesc* var_cur;
struct VarDesc* var_head_in_mem;	//函数嵌套调用时存放前一个函数的形参
struct VarDesc* var_cur_in_mem;


void print_object_code();
#endif
