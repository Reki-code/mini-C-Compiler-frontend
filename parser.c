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
<exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int>
<unary_op> ::= "!" | "~" | "-"
*/

expr_ast_t *parse_expr(list *tokens);

// <factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int>
expr_ast_t *parse_factor(list *tokens) {
  token_t *tok = list_pop(tokens);
  if (tok->type == open_parenthesis) {
    // <factor> ::= "(" <exp> ")"
    expr_ast_t *expr_ast = parse_expr(tokens);
    tok = list_pop(tokens);
    if (tok->type != close_parenthesis) {
      fprintf(stderr, "%s\n", "缺少 \")\"");
    }
    return expr_ast;
  } else if (tok->type == negation || tok->type == logical_negation ||
             tok->type == bitwise_complement) {
    //<factor> ::= <unary_op> <factor>
    char *op = tok->value;
    unary_operator_ast_t *unary_operator_ast =
        new_unary_operator(op, parse_factor(tokens));
    return new_expr_ast_w_unary(unary_operator_ast);
  } else if (tok->type == number) {
    //<factor> ::= <int>
    int num = (int)strtol(tok->value, NULL, 10);
    number_ast_t *number = new_number_ast(num);
    return new_expr_ast_w_number(number);
  }
  fprintf(stderr, "%s\n", "parse_expr: error");
  return NULL;
}

// <term> ::= <factor> { ("*" | "/") <factor> }
expr_ast_t *parse_term(list *tokens) {
  expr_ast_t *factor = parse_factor(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == multiplication || tok->type == division) {
    list_pop(tokens);
    char *op = tok->value;
    expr_ast_t *next_factor = parse_factor(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator(op, factor, next_factor);
    factor = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return factor;
}

// <exp> ::= <term> { ("+" | "-") <term> }
expr_ast_t *parse_expr(list *tokens) {
  expr_ast_t *term = parse_term(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == negation || tok->type == addition) {
    list_pop(tokens);
    char *op = tok->value;
    expr_ast_t *next_term = parse_term(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator(op, term, next_term);
    term = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return term;
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
  token_t *tok = list_pop(tokens); // pop "int"
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
  printf("%s\n", "parser finished");
  program_print(program);

  return 0;
}
