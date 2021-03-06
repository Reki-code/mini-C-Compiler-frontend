#include "parser.h"
#include "token.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

/*
<program> ::= { <function> }
<function> ::= "int" <id> "(" [ "int" <id> { "," "int" <id> } ] ")" ( "{" {
<block-item> } "}" | ";" )

<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp-option> ";"
              | <exp> ";"
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> }"}"
              | "for" "(" <exp-option> ";" <exp-option> ";" <exp-option> ")"
<statement>
              | "for" "(" <declaration> <exp-option> ";" <exp-option> ")"
<statement>
              | "while" "(" <exp> ")" <statement>
              | "do" <statement> "while" <exp> ";"
              | "break" ";"
              | "continue" ";"
*//*
<exp-option> ::= <exp> | ""
<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<function-call> ::= id "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!" | "~" | "-"
*/

expr_ast_t *parse_expr(list *tokens);

// <id> ::= string
identifier_ast_t *parse_identifier(list *tokens) {
  token_t *tok = list_pop(tokens);
  return new_identifier_ast(tok->value);
}

// <function-call> ::= <id> "(" [ <exp> { "," <exp> } ] ")"
function_call_ast_t *parse_function_call(list *tokens) {
  identifier_ast_t *function_name = parse_identifier(tokens); // pop <id>
  function_call_ast_t *function_call_ast = new_function_call_ast(function_name);
  token_t *tok = list_pop(tokens); // pop "("
  tok = list_peek(tokens);
  if (tok->type != close_parenthesis) {        // [ <exp> { "," <exp> } ]
    expr_ast_t *argument = parse_expr(tokens); // pop <exp>
    function_call_add_argument(function_call_ast, argument);
    tok = list_peek(tokens);
    while (tok->type == commas) {
      list_pop(tokens);              // pop ","
      argument = parse_expr(tokens); // pop <exp>
      function_call_add_argument(function_call_ast, argument);
      tok = list_peek(tokens);
    }
  }
  list_pop(tokens); // pop ")"
  return function_call_ast;
}

// <factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | <int> |
// <id>
expr_ast_t *parse_factor(list *tokens) {
  token_t *tok = list_pop(tokens);
  if (tok->type == open_parenthesis) {
    // <factor> ::= "(" <exp> ")"
    expr_ast_t *expr_ast = parse_expr(tokens);
    tok = list_pop(tokens);
    if (tok->type != close_parenthesis) {
      fprintf(stderr, "%s\n", "parse_factor need )");
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
    // <facotr> ::= <id> | <function-call>
    token_t *peek_tok = list_peek(tokens);
    if (peek_tok->type == open_parenthesis) { // <function-call>
      list_push(tokens, tok);                 // push <id>
      function_call_ast_t *func_call = parse_function_call(tokens);
      return new_expr_ast_w_function_call(func_call);
    } else {
      identifier_ast_t *var = new_identifier_ast(tok->value);
      return new_expr_ast_w_identifier(var);
    }
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

expr_ast_t *parse_expr(list *tokens);

// <conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
expr_ast_t *parse_conditional(list *tokens) {
  expr_ast_t *logical_or_expr = parse_logical_or_expr(tokens);
  token_t *tok = list_peek(tokens);
  if (tok->type ==
      question_mark) { // <logical-or-exp> "?" <exp> ":" <conditional-exp>
    list_pop(tokens);  // pop "?"
    expr_ast_t *if_expr = parse_expr(tokens); // if branch
    tok = list_pop(tokens);                   // pop ":"
    if (tok->type != colon) {
      fprintf(stderr, "%s\n", "parse_conditional: need :");
    }
    expr_ast_t *else_expr = parse_conditional(tokens); // else branch
    conditional_ast_t *conditional_ast =
        new_conditional_ast(logical_or_expr, if_expr, else_expr);
    return new_expr_ast_w_conditional(conditional_ast);
  } else { // <logical-or-exp>
    return logical_or_expr;
  }
}

// <exp> ::= <id> "=" <exp> | <conditional-exp>
expr_ast_t *parse_expr(list *tokens) {
  expr_ast_t *expr_ast;
  token_t *tok = list_pop(tokens);
  token_t *peek_tok = list_peek(tokens);
  if (peek_tok->type == assign) { // <id> "=" <exp>
    list_push(tokens, tok);
    identifier_ast_t *identifier_ast = parse_identifier(tokens); // <id>
    list_pop(tokens);                                            // pop "="
    expr_ast_t *assign_expr = parse_expr(tokens);
    assign_ast_t *assign_ast = new_assign_ast(identifier_ast, assign_expr);
    expr_ast = new_expr_ast_w_assign(assign_ast);
  } else { // <conditional-exp>
    list_push(tokens, tok);
    expr_ast = parse_conditional(tokens);
  }
  return expr_ast;
}

// <exp-option> ::= <exp> | ""
expr_ast_t *parse_optional_expr(list *tokens) {
  token_t *tok = list_peek(tokens);
  if (tok->type == close_parenthesis || tok->type == semicolon) { // ""
    return new_null_expr();
  } else {
    return parse_expr(tokens);
  }
}

block_item_ast_t *parse_block_item(list *tokens);
assign_ast_t *parse_declaration(list *tokens);

// <statement> ::= "return" <exp> ";"
//              | <exp-option> ";"
//              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
//              | "{" { <block-item> } "}
//              | "for" "(" <exp-option> ";" <exp-option> ";" <exp-option> ")"
//              <statement> | "for" "(" <declaration> <exp-option> ";"
//              <exp-option> ")" <statement> | "while" "(" <exp> ")" <statement>
//              | "do" <statement> "while" <exp> ";"
//              | "break" ";"
//              | "continue" ";"
statement_ast_t *parse_statement(list *tokens) {
  token_t *tok;
  statement_ast_t *statement = new_statement_ast();
  tok = list_peek(tokens);
  if (tok->type == return_k) { // "return" <exp>
    list_pop(tokens);          // pop "return"
    expr_ast_t *expr_ast = parse_expr(tokens);
    return_ast_t *return_ast = new_return_ast(expr_ast);
    statement_ast_init_w_return(statement, return_ast);
    tok = list_pop(tokens); // pop ";"
    if (tok->type != semicolon) {
      fprintf(stderr, "%s\n", "parse_statement: need ;");
    }
  } else if (tok->type ==
             if_k) {  // "if" "(" <exp> ")" <statement> [ "else" <statement> ]
    list_pop(tokens); // pop "if"
    tok = list_pop(tokens); // pop "("
    if (tok->type != open_parenthesis) {
      fprintf(stderr, "%s\n", "parse_statement: need (");
    }
    expr_ast_t *expr_ast = parse_expr(tokens);
    tok = list_pop(tokens); // pop ")"
    if (tok->type != close_parenthesis) {
      fprintf(stderr, "%s\n", "parse_statement: need (");
    }
    // parse if branch
    statement_ast_t *if_statement = parse_statement(tokens);
    if_ast_t *if_ast = new_if_ast(expr_ast, if_statement);
    tok = list_peek(tokens);
    if (tok->type == else_k) {
      list_pop(tokens);
      statement_ast_t *else_statement = parse_statement(tokens);
      if_ast_add_else_branch(if_ast, else_statement);
    }
    statement_ast_init_w_if(statement, if_ast);
  } else if (tok->type == open_brace) { // "{" { <block-item> } "}"
    list_pop(tokens);                   // pop "{"
    compound_ast_t *compound_ast = new_compound_ast();
    tok = list_peek(tokens);
    while (tok->type != close_brace) {
      block_item_ast_t *block_item_ast = parse_block_item(tokens);
      compound_add_block_item(compound_ast, block_item_ast);
      tok = list_peek(tokens);
    }
    list_pop(tokens); // pop "}"
    statement_ast_init_w_compound(statement, compound_ast);
  } else if (tok->type == for_k) {
    list_pop(tokens); // pop "for"
    list_pop(tokens); // pop "("
    tok = list_peek(tokens);
    if (tok->type ==
        int_k) { // <declaration> <exp-option> ";" <exp-option> ")" <statement>
      assign_ast_t *initial = parse_declaration(tokens);
      expr_ast_t *condition = parse_optional_expr(tokens);
      list_pop(tokens); // pop ";"
      expr_ast_t *post = parse_optional_expr(tokens);
      list_pop(tokens); // pop ")"
      statement_ast_t *body = parse_statement(tokens);
      for_decl_ast_t *for_decl_ast = new_for_decl_ast();
      for_decl_set_initial(for_decl_ast, initial);
      for_decl_set_condition(for_decl_ast, condition);
      for_decl_set_post(for_decl_ast, post);
      for_decl_set_body(for_decl_ast, body);
      statement_ast_init_w_for_decl(statement, for_decl_ast);
    } else { // <exp-option> ";" <exp-option> ";" <exp-option> ")" <statement>
      expr_ast_t *initial = parse_optional_expr(tokens);
      list_pop(tokens); // pop ";"
      expr_ast_t *condition = parse_optional_expr(tokens);
      list_pop(tokens); // pop ";"
      expr_ast_t *post = parse_optional_expr(tokens);
      list_pop(tokens); // ")"
      statement_ast_t *body = parse_statement(tokens);
      for_ast_t *for_ast = new_for_ast();
      for_ast_set_initial(for_ast, initial);
      for_ast_set_condition(for_ast, condition);
      for_ast_set_post(for_ast, post);
      for_ast_set_body(for_ast, body);
      statement_ast_init_w_for(statement, for_ast);
    }
  } else if (tok->type == while_k) { // "while" "(" <exp> ")" <statement>
    list_pop(tokens);                // pop "while"
    list_pop(tokens);                // pop "("
    expr_ast_t *condition = parse_expr(tokens);
    list_pop(tokens); // pop ")"
    statement_ast_t *body = parse_statement(tokens);
    while_ast_t *while_ast = new_while_ast();
    while_ast_set_condition(while_ast, condition);
    while_ast_set_body(while_ast, body);
    statement_ast_init_w_while(statement, while_ast);
  } else if (tok->type == do_k) { // "do" <statement> "while" <exp> ";"
    list_pop(tokens);             // pop "do"
    statement_ast_t *body = parse_statement(tokens);
    list_pop(tokens); // pop "while"
    expr_ast_t *condition = parse_expr(tokens);
    do_ast_t *do_ast = new_do_ast();
    do_ast_set_body(do_ast, body);
    do_ast_set_condition(do_ast, condition);
    statement_ast_init_w_do(statement, do_ast);
    if (tok->type != semicolon) {
      fprintf(stderr, "%s\n", "parse_statement: need ;");
    }
  } else if (tok->type == break_k) { // "break" ";"
    list_pop(tokens);
    statement_ast_init_w_break(statement);
    tok = list_pop(tokens);
    if (tok->type != semicolon) {
      fprintf(stderr, "%s\n", "parse_statement: need ;");
    }
  } else if (tok->type == continue_k) { // "continue" ";"
    list_pop(tokens);
    statement_ast_init_w_continue(statement);
    tok = list_pop(tokens);
    if (tok->type != semicolon) {
      fprintf(stderr, "%s\n", "parse_statement: need ;");
    }
  } else { // <exp-option> ";"
    expr_ast_t *expr_ast = parse_optional_expr(tokens);
    statement_ast_init_w_expr(statement, expr_ast);
    tok = list_pop(tokens); // pop ";"
    if (tok->type != semicolon) {
      fprintf(stderr, "%s\n", "parse_statement: need ;");
    }
  }

  return statement;
}

// <declaration> ::= "int" <id> [ = <exp> ] ";"
assign_ast_t *parse_declaration(list *tokens) {
  token_t *tok = list_pop(tokens);                         // pop "int"
  identifier_ast_t *identifier = parse_identifier(tokens); // <id>
  expr_ast_t *expr_ast = NULL;
  tok = list_peek(tokens);
  if (tok->type == assign) { //  [ = <exp>]
    list_pop(tokens);        // pop "="
    expr_ast = parse_expr(tokens);
  }
  assign_ast_t *assign_ast = new_assign_ast(identifier, expr_ast);
  tok = list_pop(tokens); // pop ";"
  if (tok->type != semicolon) {
    fprintf(stderr, "%s\n", "parse_declaration: need ;");
  }
  return assign_ast;
}

// <block-item> ::= <statement> | <declaration>
block_item_ast_t *parse_block_item(list *tokens) {
  token_t *tok = list_peek(tokens);
  block_item_ast_t *block_item_ast = new_block_item_ast();
  if (tok->type == int_k) { // <declaration>
    assign_ast_t *assign_ast = parse_declaration(tokens);
    block_item_init_w_assign(block_item_ast, assign_ast);
  } else { // <statement>
    statement_ast_t *statement_ast = parse_statement(tokens);
    block_item_init_w_statement(block_item_ast, statement_ast);
  }
  return block_item_ast;
}

// <function> ::= "int" <id> "(" [ "int" <id> { "," "int" <id> } ] ")" ( "{" {
// <block-item> } "}" | ";" )
function_declaration_ast_t *parse_function_declaration(list *tokens) {
  token_t *tok = list_pop(tokens); // pop "int"
  if (tok->type != int_k) {
    fprintf(stderr, "%s\n", "返回值只能为int");
  }
  identifier_ast_t *function_name = parse_identifier(tokens); // <id>
  function_declaration_ast_t *function_declaration_ast =
      new_function_declaration_ast(function_name);
  tok = list_pop(tokens); // "("
  if (tok->type != open_parenthesis) {
    fprintf(stderr, "%s\n", "parse_function_declaration need (");
  }
  tok = list_peek(tokens);
  if (tok->type == int_k) {
    list_pop(tokens); // pop "int"
    identifier_ast_t *parameter1 = parse_identifier(tokens);
    function_declaration_add_parameter(function_declaration_ast, parameter1);
    tok = list_peek(tokens);
    while (tok->type == commas) {
      list_pop(tokens); // pop ","
      list_pop(tokens); // pop "int"
      identifier_ast_t *parameter = parse_identifier(tokens);
      function_declaration_add_parameter(function_declaration_ast, parameter);
      tok = list_peek(tokens);
    }
  }
  tok = list_pop(tokens); // ")"
  if (tok->type != close_parenthesis) {
    fprintf(stderr, "%s\n", "parse_function_declaration need )");
  }
  // "{" { <block-item> } "}" | ";"
  tok = list_peek(tokens);
  if (tok->type == semicolon) { // ";"
    list_pop(tokens);
  } else {                  // "{" { <block-item> } "}"
    tok = list_pop(tokens); // "{
    if (tok->type != open_brace) {
      fprintf(stderr, "%s\n", "parse_function_declaration need {");
    }
    tok = list_peek(tokens);
    while (tok->type != close_brace) { // { <block-item> }
      block_item_ast_t *block_item_ast = parse_block_item(tokens);
      function_declaration_add_block_item(function_declaration_ast,
                                          block_item_ast);
      tok = list_peek(tokens);
    }
    tok = list_pop(tokens); // "}"
  }

  return function_declaration_ast;
}

// <program> ::= { <function> }
program_ast_t *parse_program(list *tokens) {
  function_declaration_ast_t *function_declaration_ast =
      parse_function_declaration(tokens);
  program_ast_t *program_ast = new_program_ast(function_declaration_ast);
  token_t *tok = list_peek(tokens);
  while (tok->type == int_k) {
    function_declaration_ast = parse_function_declaration(tokens);
    program_add_function_declarartion(program_ast, function_declaration_ast);
    tok = list_peek(tokens);
  }
  return program_ast;
}

program_ast_t *parser(list *tokens) { return parse_program(tokens); }
