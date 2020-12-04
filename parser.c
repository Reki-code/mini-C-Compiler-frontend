#include "list.h"
#include "token.h"

#include <stdio.h>

#include "ast.c"

#include "lexer.h"
#include "list.h"
#include "token.h"

/*
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <int> ";" | "int" <id> "=" <int> ";"
<exp> ::= <int>
*/

// <exp> ::= <int>
expr_ast_t *parse_expr(list *tokens) {
    token_t *tok = list_pop(tokens);
    int num = (int) strtol(tok->value, NULL, 10);
    number_ast_t *number = new_number_ast(num);
    return new_expr_ast(number);
}

// <id> ::= string
identifier_ast_t *parse_identifier(list *tokens) {
    token_t *tok = list_pop(tokens);
    return new_identifier_ast(tok->value);
}

// <statement> ::= "return" <int> ";" | "int" <id> "=" <int> ";"
statement_ast_t *parse_statement(list *tokens) {
    token_t *tok = list_pop(tokens);
    statement_ast_t *statement = new_statement_ast();
    if (tok->type == return_k) { // "return" expr ";"
        expr_ast_t *expr_ast = parse_expr(tokens);
        return_ast_t *return_ast = new_return_ast(expr_ast);
        statement_ast_init_w_return(statement, return_ast);
    } else if (tok->type == int_k) { // "int" <id> "=" expr ";"
        identifier_ast_t *identifier_ast = parse_identifier(tokens);
        tok = list_pop(tokens); // pop "="
        if (tok->type != assign) {
            fprintf(stderr, "%s\n", "缺少 \"=\"");
        }
        expr_ast_t *expr_ast = parse_expr(tokens);
        tok = list_pop(tokens); // pop ";"
        if (tok->type != semicolon) {
            fprintf(stderr, "%s\n", "缺少 \";\"");
        }
        assign_ast_t *assign_ast = new_assign_ast(identifier_ast, expr_ast);
        statement_ast_init_w_assign(statement, assign_ast);
    }
    return statement;
}

// <function> ::= "int" <id> "(" ")" "{" <statement> "}"
function_declaration_ast_t *parse_function_declaration(list *tokens) {
    token_t *tok = list_pop(tokens);                             // pop "int"
    if (tok->type != int_k) {
        fprintf(stderr, "%s\n", "返回值只能为int");
    }
    identifier_ast_t *identifier_ast = parse_identifier(tokens); // <id>
    tok = list_pop(tokens);                                      // "("
    if (tok->type != open_parenthesis) {
        fprintf(stderr, "%s\n", "缺少 \"(\"");
    }
    tok = list_pop(tokens); // ")"
    if (tok->type != close_parenthesis) {
        fprintf(stderr, "%s\n", "缺少 \")\"");
    }
    tok = list_pop(tokens); // "{"
    if (tok->type != open_brace) {
        fprintf(stderr, "%s\n", "缺少 \"{\"");
    }
    statement_ast_t *statement_ast = parse_statement(tokens);
    tok = list_pop(tokens); // "}"
    if (tok->type != close_brace) {
        fprintf(stderr, "%s\n", "缺少 \"{\"");
    }

    return new_function_declaration_ast(identifier_ast, statement_ast);
}

// <program> ::= <function>
program_ast_t *parse_program(list *tokens) {
    function_declaration_ast_t *function_declaration_ast =
            parse_function_declaration(tokens);
    return new_program_ast(function_declaration_ast);
}

program_ast_t *parser(list *tokens) { return parse_program(tokens); }

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        fprintf(stderr, "%s\n", "请输入源代码文件");
        return 0;
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "%s\n", "文件不存在");
        return -1;
    }
    list *token_list = lexer(fp);
    list_print(token_list, print_token);

    printf("%s\n", "start parser");
    program_ast_t *program = parser(token_list);
    program_print(program);
    printf("%s\n", "parser finished");

    return 0;
}
