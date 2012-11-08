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

，本工具对其进行了扩展以支持带分号的多语句sql解析，并增加入了use database语句的支持。