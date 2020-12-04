lexer: main.c lexer.c list.c token.c
	clang main.c lexer.c token.c list.c -o lexer

parser: parser.c list.c token.c lexer.c
	clang parser.c list.c token.c lexer.c -o parser
