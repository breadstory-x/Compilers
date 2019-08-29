#ifndef _NODE_H_
#define _NODE_H_

struct Node
{
	int col;//当前节点所在行数
	int isToken;//是否为词法单元（即终结符）
	char name[100];//名称(词法单元语法单元名)
	char text[100];//值（int float type的具体数值）
	struct Node *firstChild;//第一个子节点
	struct Node *nextSibling;//下一个兄弟节点
};


#endif
