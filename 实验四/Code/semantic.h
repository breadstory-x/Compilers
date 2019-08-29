#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#define HASH_TABLE_SIZE 65536
#define STACK_DEEP 200
#define VAR_INT 100
#define VAR_FLOAT 101

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Symbol_table_item
{
	FieldList f;
	struct Symbol_table_item* next_hash_item;
	struct Symbol_table_item* next_symbol;
	int symbol_depth;
};


struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE, FUNCTION} kind;
	union
	{
		// 基本类型
		int basic;
		// 数组类型信息包括元素类型与数组大小构成
		struct { Type elem; int size; } array;
		// 结构体类型信息是一个链表
		struct { char* name; FieldList structure;}structure_;

		struct{
			FieldList params;//函数的参数
			Type funcType;//返回值类型
			int paramNum;//参数数量
		}function;
	} u;
};
struct FieldList_
{
	char* name; // 域的名字
	Type type; // 域的类型
	FieldList tail; // 下一个域
	int is_param;
};

void print_hash_table();
unsigned int hash_pjw(char* name);
struct Symbol_table_item *find_items_s_table(char *name);
struct Symbol_table_item *find_items_struct_table(char *name);
int search_s_table(struct Symbol_table_item *p);
void add_s_table(struct Symbol_table_item *p);
void del_s_table(struct Symbol_table_item *p);
int check_type(Type type1, Type type2);
void init_table();

Type Specifier(struct Node* node);
struct Symbol_table_item *VarDec(struct Node* node, Type type);
Type Exp(struct Node* node);
struct Symbol_table_item *Dec(struct Node* node, Type type);
void Def(struct Node* node);
void CompSt(struct Node* node, Type function_type);
void ExtDef(struct Node* node);

void traverse(struct Node* this_node,int depth);

#endif

