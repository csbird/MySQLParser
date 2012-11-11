objects=main.o MySQLLexer.o MySQLParser.o stack.o
main: $(objects)
	gcc -o main -L/usr/local/lib -lantlr3c $(objects)
main.o: main.c
	gcc -c -I/usr/local/include main.c
MySQLLexer.o: MySQLLexer.c MySQLLexer.h
	gcc -c -I/usr/local/include MySQLLexer.c
MySQLParser.o: MySQLParser.c MySQLParser.h
	gcc -c -I/usr/local/include MySQLParser.c
stack.o: stack.c stack.h
	gcc -c stack.c
.PHONY: clean
clean:
	rm main $(objects)
