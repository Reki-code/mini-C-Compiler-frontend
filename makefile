CC = clang
lexer: main_lexer.c lexer.o list.o token.o
	$(CC) $^ -o $@

parser: main_parser.c parser.o list.o token.o lexer.o ast.o
	$(CC) $^ -o $@

list.o: list.c
token.o: token.c
lexer.o: lexer.c

ast.o: ast.c
parser.o: parser.c

.PHONY: clean
clean:
	rm *.o lexer parser
