#include "list.h"
#include "token.h"

#include <stdio.h>

#include "ast.c"

#include "lexer.h"
#include "list.h"
#include "token.h"

/*
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" { <statement> } "}"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "int" <id> [ = <exp>] ";"
<exp> ::= <id> "=" <exp> | <logical-or-exp>
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<unary_op> ::= "!" | "~" | "-"
*/

expr_ast_t *parse_additive_expr(list *tokens);

// <factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
expr_ast_t *parse_factor(list *tokens) {
  token_t *tok = list_pop(tokens);
  if (tok->type == open_parenthesis) {
    // <factor> ::= "(" <exp> ")"
    expr_ast_t *expr_ast = parse_additive_expr(tokens);
    tok = list_pop(tokens);
    if (tok->type != close_parenthesis) {
      fprintf(stderr, "%s\n", "缺少 \")\"");
    }
    return expr_ast;
  } else if (tok->type == negation || tok->type == logical_negation ||
             tok->type == bitwise_complement) {
    // <factor> ::= <unary_op> <factor>
    char *op = tok->value;
    unary_operator_ast_t *unary_operator_ast =
        new_unary_operator(op, parse_factor(tokens));
    return new_expr_ast_w_unary(unary_operator_ast);
  } else if (tok->type == number) {
    // <factor> ::= <int>
    int num = (int)strtol(tok->value, NULL, 10);
    number_ast_t *number = new_number_ast(num);
    return new_expr_ast_w_number(number);
  } else if (tok->type == identifier) {
    // <facotr> ::= <id>
    identifier_ast_t *var = new_identifier_ast(tok->value);
    return new_expr_ast_w_identifier(var);
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

// <additive-exp> ::= <term> { ("+" | "-") <term> }
expr_ast_t *parse_additive_expr(list *tokens) {
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

// <relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=")
// <additive-exp> }
expr_ast_t *parse_relational_expr(list *tokens) {
  expr_ast_t *additive_expr = parse_additive_expr(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == less_than || tok->type == greater_than ||
         tok->type == less_than_or_equal ||
         tok->type == greater_than_or_equal) {
    list_pop(tokens);
    char *op = tok->value;
    expr_ast_t *next_additive_epxr = parse_additive_expr(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator(op, additive_expr, next_additive_epxr);
    additive_expr = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return additive_expr;
}

// <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
expr_ast_t *parse_equality_expr(list *tokens) {
  expr_ast_t *relational_expr = parse_relational_expr(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == not_equal || tok->type == equal) {
    list_pop(tokens);
    char *op = tok->value;
    expr_ast_t *next_relational_expr = parse_relational_expr(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator(op, relational_expr, next_relational_expr);
    relational_expr = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return relational_expr;
}

// <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
expr_ast_t *parse_logical_and_expr(list *tokens) {
  expr_ast_t *equality_expr = parse_equality_expr(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == and_k) {
    list_pop(tokens);
    expr_ast_t *next_equality_expr = parse_equality_expr(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator("&&", equality_expr, next_equality_expr);
    equality_expr = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return equality_expr;
}

// <logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
expr_ast_t *parse_logical_or_expr(list *tokens) {
  expr_ast_t *logical_and_expr = parse_logical_and_expr(tokens);
  token_t *tok = list_peek(tokens);
  while (tok->type == or_k) {
    list_pop(tokens);
    expr_ast_t *next_logical_and_expr = parse_logical_and_expr(tokens);
    binary_operator_ast_t *binary_operator_ast =
        new_binary_operator("||", logical_and_expr, next_logical_and_expr);
    logical_and_expr = new_expr_ast_w_binary(binary_operator_ast);
    tok = list_peek(tokens);
  }
  return logical_and_expr;
}

// <id> ::= string
identifier_ast_t *parse_identifier(list *tokens) {
  token_t *tok = list_pop(tokens);
  return new_identifier_ast(tok->value);
}

// <exp> ::= <id> "=" <exp> | <logical-or-exp>
expr_ast_t *parse_expr(list *tokens) {
  expr_ast_t *expr_ast;
  token_t *tok = list_peek(tokens);
  if (tok->type == identifier) {
    token_t *id = list_pop(tokens);
    token_t *ass = list_peek(tokens);
    list_push(tokens, id);
    if (ass->type == assign) { // <id> "=" <exp>
      identifier_ast_t *identifier_ast = parse_identifier(tokens);
      expr_ast = new_expr_ast_w_identifier(identifier_ast);
    } else { //// <logical-or-exp>
      expr_ast = parse_logical_or_expr(tokens);
    }
  } else { // <logical-or-exp>
    expr_ast = parse_logical_or_expr(tokens);
  }
  return expr_ast;
}

// <statement> ::= "return" <exp> ";"
//              | <exp> ";"
//              | "int" <id> [ = <exp>] ";"
statement_ast_t *parse_statement(list *tokens) {
  token_t *tok;
  statement_ast_t *statement = new_statement_ast();
  tok = list_peek(tokens);
  if (tok->type == return_k) { // "return" <exp>
    list_pop(tokens);          // pop "return"
    expr_ast_t *expr_ast = parse_expr(tokens);
    return_ast_t *return_ast = new_return_ast(expr_ast);
    statement_ast_init_w_return(statement, return_ast);
  } else if (tok->type == int_k) { // "int" <id> [ = <exp>]
    list_pop(tokens);              // pop "int"
    identifier_ast_t *identifier = parse_identifier(tokens); // <id>
    expr_ast_t *expr_ast = NULL;
    tok = list_peek(tokens);
    if (tok->type == assign) { //  [ = <exp>]
      list_pop(tokens);        // pop "="
      expr_ast = parse_expr(tokens);
    }
    assign_ast_t *assign_ast = new_assign_ast(identifier, expr_ast);
    statement_ast_init_w_assign(statement, assign_ast);
  } else { // <exp> ";"
    expr_ast_t *expr_ast = parse_expr(tokens);
    statement_ast_init_w_expr(statement, expr_ast);
  }
  tok = list_pop(tokens); // pop ";"
  if (tok->type != semicolon) {
    fprintf(stderr, "%s\n", "parse_statement: need ;");
  }
  return statement;
}

// <function> ::= "int" <id> "(" ")" "{" { <statement> } "}"
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
  tok = list_pop(tokens); // "{
  if (tok->type != open_brace) {
    fprintf(stderr, "%s\n", "缺少 \"{\"");
  }
  function_declaration_ast_t *function_declaration_ast =
      new_function_declaration_ast(identifier_ast);
  tok = list_peek(tokens);
  while (tok->type != close_brace) { // { <statement> }
    statement_ast_t *statement_ast = parse_statement(tokens);
    function_declaration_add_statement(function_declaration_ast, statement_ast);
    tok = list_peek(tokens);
  }
  tok = list_pop(tokens); // "}"

  return function_declaration_ast;
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
