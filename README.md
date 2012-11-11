MySQLParser
===========

1、为什么做这个工具？
====
在一个数据仓库项目中，需要一个工具程序去自动解析提取sql中的表信息：库表名、读写类型。

已有的一些工具：HIVE中自带的lineage；功能强大的General SQL Parser则可以解析各种常见数据库SQL，

但是暂时不支持mysql的load data local语法，而且还是商业版的（有90天的试用版）；其他一些开源的也尝试过，

都不太符合项目需求。

2、如何实现？
====
本工具是基于antlr3来实现的mysql解析器。antlr主页上提供有mysql的antlr语法，通过对这个语法做小部分的规则重写（rewrite），

从而构造一个带表信息的AST（Abstract Syntax Tree）,最后遍历这个AST输出表信息。由于antlr提供的mysql语法只是支持单条sql

，本工具对其进行了扩展以支持带分号的多语句sql解析，并增加了use database语句的支持。

3、编译使用
====
1）下载antlr c runtime api，编译安装。库和头文件默认安装到/usr/local/include和/usr/local/lib

2）编译本项目。

3）简单测试：

    ./main "use mydb;select * from abc;delete from yy;"
执行输出如下：

    Input=mydb.abc
    Output=mydb.yy

4、关于解析器代码的生成
====
本工具使用的语法文件是在antlr提供的mysql parser grammar基础上修改得到的：去除源文件里对标志符（identifers）的输出动作
代码；重写部分规则以生成所需AST（语法文件中带->的内容即为重写的规则）；增加一个虚构token：TABLE_REF；增加use_statement。
然后使用antlr工具去生成解析代码MySQLLexer.*和MySQLParser.*。

如何遍历AST解析表信息，请参考main.c中的注释说明。

如何重写语法规则以及antlr更多详细使用，请参考antlr相关文档。
5、联系
====
gmail貌似挂了，暂时用zhimingzhang at foxmail.com