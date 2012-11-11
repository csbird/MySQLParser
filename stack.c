#include "stdlib.h"
#include "stack.h"
StackElementPtr top(StackElementPtr stack)
{
    return stack->next;
}

StackElementPtr pop(StackElementPtr stack)
{
    StackElementPtr top = stack->next;
    if(top)
    {
        stack->next = top->next;
    }
    return top;
}

void push(StackElementPtr stack, int value)
{
    StackElementPtr element = (StackElementPtr)malloc(sizeof(struct stack_element));
    element->type = value;
    element->next = stack->next;
    stack->next = element;
}

void destroy(StackElementPtr stack)
{
    StackElementPtr p = stack->next;
    StackElementPtr temp;
    while(p)
    {
        temp = p;
        p = temp->next;
        free(temp);
    }
    stack->next = NULL;
}

