#ifndef _STACK_H_
#define _STACK_H_

//栈的一个简单实现
struct stack_element
{
	//存放树节点类型
    int type;
    struct stack_element* next;
};

typedef struct stack_element* StackElementPtr;

//取得栈的第一个元素
StackElementPtr top(StackElementPtr stack);
//出栈
StackElementPtr pop(StackElementPtr stack);
//入栈
void push(StackElementPtr stack, int value);
//删除栈
void destroy(StackElementPtr stack);

#endif
