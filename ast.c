#include <stdio.h>
#include <stdlib.h>

void print_space(int level) {
  for (size_t i = 0; i < level; i++) {
    putchar(' ');
    putchar(' ');
  }
}

// number literals
typedef struct number_ast {
  int val;
} number_ast_t;

void number_ast_init(number_ast_t *number_ast, int value) {
  number_ast->val = value;
}
number_ast_t *new_number_ast(int value) {
  number_ast_t *number_ast = malloc(sizeof(number_ast_t));
  number_ast_init(number_ast, value);
  return number_ast;
}
void number_print(number_ast_t *number_ast, int level) {
  print_space(level);
  printf("整数常量：%d\n", number_ast->val);
}

// identifier
typedef struct identifier_ast {
  char *name;
} identifier_ast_t;

void identifier_ast_init(identifier_ast_t *identifier_ast, char *name) {
  identifier_ast->name = name;
}
identifier_ast_t *new_identifier_ast(char *name) {
  identifier_ast_t *identifier_ast = malloc(sizeof(identifier_ast_t));
  identifier_ast_init(identifier_ast, name);
  return identifier_ast;
}
void identifier_print(identifier_ast_t *identifier_ast, int level) {
  print_space(level);
  printf("标识符：%s\n", identifier_ast->name);
}

// expr  = Expr(int)
typedef struct expr_ast {
  number_ast_t *number_ast;
} expr_ast_t;

void expr_ast_init(expr_ast_t *expr_ast, number_ast_t *number_ast) {
  expr_ast->number_ast = number_ast;
}
expr_ast_t *new_expr_ast(number_ast_t *number_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast_init(expr_ast, number_ast);
  return expr_ast;
}
void expr_print(expr_ast_t *expr_ast, int level) {
  print_space(level);
  printf("求值表达式：\n");
  number_print(expr_ast->number_ast, level + 1);
}

// return
typedef struct return_ast {
  expr_ast_t *expr_ast;
} return_ast_t;

void return_ast_init(return_ast_t *return_ast, expr_ast_t *expr_ast) {
  return_ast->expr_ast = expr_ast;
}
return_ast_t *new_return_ast(expr_ast_t *expr_ast) {
  return_ast_t *return_ast = malloc(sizeof(return_ast_t));
  return_ast_init(return_ast, expr_ast);
  return return_ast;
}
void return_print(return_ast_t *return_ast, int level) {
  print_space(level);
  printf("返回：\n");
  expr_print(return_ast->expr_ast, level + 1);
}

// assign
typedef struct assign_ast {
  identifier_ast_t *identifier_ast;
  expr_ast_t *expr_ast;
} assign_ast_t;

void assign_ast_init(assign_ast_t *assign_ast, identifier_ast_t *identifier_ast,
                     expr_ast_t *expr_ast) {
  assign_ast->identifier_ast = identifier_ast;
  assign_ast->expr_ast = expr_ast;
}
assign_ast_t *new_assign_ast(identifier_ast_t *identifier, expr_ast_t *expr) {
  assign_ast_t *assign_ast = malloc(sizeof(assign_ast_t));
  assign_ast_init(assign_ast, identifier, expr);
  return assign_ast;
}
void assign_print(assign_ast_t *assign_ast, int level) {
  print_space(level);
  printf("赋值表达式：\n");
  identifier_print(assign_ast->identifier_ast, level + 1);
  expr_print(assign_ast->expr_ast, level + 1);
}

// statement = Return(exp) | Assign(variable, exp)
typedef struct statement_ast {
  union {
    return_ast_t *return_ast;
    assign_ast_t *assign_ast;
  };
  int u;
} statement_ast_t;

statement_ast_t *new_statement_ast() {
  statement_ast_t *statement = malloc(sizeof(statement_ast_t));
  return statement;
}
void statement_ast_init_w_return(statement_ast_t *statement_ast,
                                 return_ast_t *return_ast) {
  statement_ast->return_ast = return_ast;
  statement_ast->u = 0;
}
void statement_ast_init_w_assign(statement_ast_t *statement_ast,
                                 assign_ast_t *assign_ast) {
  statement_ast->assign_ast = assign_ast;
  statement_ast->u = 1;
}
void statement_print(statement_ast_t *statement_ast, int level) {
  print_space(level);
  printf("语句：\n");
  switch (statement_ast->u) {
  case 0:
    return_print(statement_ast->return_ast, level + 1);
    break;
  case 1:
    assign_print(statement_ast->assign_ast, level + 1);
    break;
  }
}

// function declaration = Function(id, statement)
typedef struct function_declaration_ast {
  identifier_ast_t *identifier;
  statement_ast_t *statement;
} function_declaration_ast_t;

void function_declaration_ast_init(
    function_declaration_ast_t *function_declaration_ast,
    identifier_ast_t *identifier_ast, statement_ast_t *statement_ast) {
  function_declaration_ast->identifier = identifier_ast;
  function_declaration_ast->statement = statement_ast;
}
function_declaration_ast_t *
new_function_declaration_ast(identifier_ast_t *identifier_ast,
                             statement_ast_t *statementa) {
  function_declaration_ast_t *function_declaration =
      malloc(sizeof(function_declaration_ast_t));
  function_declaration_ast_init(function_declaration, identifier_ast,
                                statementa);
  return function_declaration;
}
void function_declaration_print(
    function_declaration_ast_t *function_declaration_ast, int level) {
  print_space(level);
  printf("函数：\n");
  print_space(level);
  printf("函数名\n");
  identifier_print(function_declaration_ast->identifier, level + 1);
  print_space(level);
  printf("函数体\n");
  statement_print(function_declaration_ast->statement, level + 1);
}

// program = Program(function_declaration)
typedef struct program_ast {
  function_declaration_ast_t *function_declaration_ast;
} program_ast_t;

void program_ast_init(program_ast_t *program_ast,
                      function_declaration_ast_t *function_declaration_ast) {
  program_ast->function_declaration_ast = function_declaration_ast;
}
program_ast_t *
new_program_ast(function_declaration_ast_t *function_declaration) {
  program_ast_t *program = malloc(sizeof(program_ast_t));
  program_ast_init(program, function_declaration);
  return program;
}
void program_print(program_ast_t *program_ast) {
  int level = 0;
  printf("程序：\n");
  function_declaration_print(program_ast->function_declaration_ast, level + 1);
}
