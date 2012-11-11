#include <antlr3.h>
#include "MySQLLexer.h"
#include "MySQLParser.h"
#include "stack.h"

//表类型，input表示读操作，output表示写操作
char* inputType = "Input";
char* outputType = "Output";
char* tableType = NULL;
//当前sql查询的默认数据库，用于支持use statement
char* defaultSchema = NULL;
//辅助栈用于保存路径信息
struct stack_element stack = {0, NULL};

/**
 * 深度优先遍历AST树，输出表信息。
 * 在确定表类型时，需要根据表节点的祖先节点类型来确定该表是在哪类型的语句中(select,delete,update等)
 * 从而得到该表的读写类型；由于antlr默认实现的ANTLR3_BASE_TREE::getParent方法无效，
 * 因此在遍历过程中使用了一个辅助栈来保存路径信息，以方便回溯祖先节点
 * @param tree 待遍历的AST树
 */
void traverse_tree(pANTLR3_BASE_TREE tree)
{
    if(tree)
    {
        //节点类型。语法定义的每一个token都有一个对应的数值(见MySQLLexer.h中的tokens宏定义)
        int nodeType = tree->getType(tree);
        StackElementPtr p = NULL;
        pANTLR3_BASE_TREE schemaNode = NULL;
        switch(nodeType)
        {
            //use语句
            case USE_SYM:
                //记录默认的数据库。如果后面的sql表没有schema信息则使用该默认值
                schemaNode = (pANTLR3_BASE_TREE)tree->getChild(tree, 0);
                defaultSchema = schemaNode->getText(schemaNode)->chars;
                //直接返回，无需再遍历use节点的子树
                return;

            //表
            case TABLE_REF:
                tableType = NULL;
                p = stack.next;
                //回溯该节点的祖先节点以确定表类型
                while(p)
                {
                    switch(p->type)
                    {
                        case SELECT:
                            //select语句，读类型
                            tableType = inputType;
                            break;
                        case DELETE_SYM:
                        case UPDATE:
                        case INSERT:
                        case LOAD:
                            //delete、update、insert、load语句，写类型
                            tableType = outputType;
                            break;
                        default:
                            //如需支持其他更复杂的语句，需要去重写该语句对应的语法规则
                            p = p->next;
                            break;
                    }
                    if(tableType)
                    {
                        break;
                    }
                }
                if(tableType)
                {
                    //如果TABLE_REF节点只含有一个子节点，则说明该表不带有schema信息，使用默认值填充
                    int c = tree->getChildCount(tree);
                    if(c == 1)
                    {

                        pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)tree->getChild(tree,0);
                        if(defaultSchema)
                            printf("%s=%s.%s\n", tableType, defaultSchema, child->getText(child)->chars);
                        else
                            printf("%s=%s\n", tableType, child->getText(child)->chars);
                            
                    }
                    else if(c == 2)
                    {
                        pANTLR3_BASE_TREE child1 = (pANTLR3_BASE_TREE)tree->getChild(tree,0);
                        pANTLR3_BASE_TREE child2 = (pANTLR3_BASE_TREE)tree->getChild(tree,1);
                        printf("%s=%s.%s\n",tableType, child1->getText(child1)->chars,child2->getText(child2)->chars);
                    }
                }
                //得到一个表信息后，无需再遍历其子树，直接返回
                return;
            default:
                //记录该节点类型，继续遍历其子树
                push(&stack, nodeType);
                break;
        }
        int i;
        //遍历子树
        for(i = 0; i < tree->getChildCount(tree); i++)
        {
            traverse_tree((pANTLR3_BASE_TREE)tree->getChild(tree,i));
        }
        //遍历完成，从栈中删除根节点
        pop(&stack);
    }
}

int main(int argc, char * argv[])
{
    //如何使用antlr自动生成的lexer、parser代码，请参考antlr c api文档
    //输入流，可以是字符串流或文件流
    pANTLR3_INPUT_STREAM           input;
    //mysql词法解析器
    pMySQLLexer               lex;
    pANTLR3_COMMON_TOKEN_STREAM    tokens;
    //mysql语法解析器
    pMySQLParser              parser;

    //使用一个文件作为输入
    //input  = antlr3FileStreamNew          ((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT);
    //这里使用的是字符串流
    input  = antlr3StringStreamNew          ((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT,strlen(argv[1]), (pANTLR3_UINT8)"testname");
    lex    = MySQLLexerNew                (input);
    tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
    parser = MySQLParserNew               (tokens);

    //解析需从某条起始规则开始，如这里的root_statement，调用相应函数
    //返回值包含3个信息：start token：解析的起始token；end token：结束token；tree：解析构造的AST树
    MySQLParser_root_statement_return ret = parser  ->root_statement(parser);
    //当前的解析状态
    pANTLR3_RECOGNIZER_SHARED_STATE state = parser->pParser->rec->state;
    pANTLR3_TOKEN_SOURCE ts = parser->pParser->tstream->tokenSource;

    //解析成功，无错误
    while(state->failed != ANTLR3_TRUE && state->errorCount == 0)
    {
        //遍历AST树，提取表信息
        traverse_tree(ret.tree);
        //删除辅助栈
        destroy(&stack);
        //如果结束token为分号则继续解析，实现多语句支持
        if(ret.stop && strcmp(ret.stop->getText(ret.stop)->chars,";") == 0)
        {
            //查看下一个节点是否为结束符，如果是则结束解析，否则继续。
            pANTLR3_COMMON_TOKEN token = parser->pParser->tstream->_LT(parser->pParser->tstream,1);
            if(token == &ts->eofToken)
            {
                break;
            }
            else
            {
                //开始下一条语句解析
                ret = parser->root_statement(parser);
                state = parser->pParser->rec->state;
            }
        }
        else
        {
            break;
        }
    }
    // Must manually clean up
    parser ->free(parser);
    tokens ->free(tokens);
    lex    ->free(lex);
    input  ->close(input);

    return 0;
}
