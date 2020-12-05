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

struct expr_ast;
typedef struct expr_ast expr_ast_t;
void expr_print(expr_ast_t *expr_ast, int level);

// unOp(operator, expr)
typedef struct unary_operator_ast {
  char *operator_str;
  expr_ast_t *expr_ast;
} unary_operator_ast_t;

void unary_operator_ast_init(unary_operator_ast_t *unary_operator_ast,
                             char *operator, expr_ast_t * expr_ast) {
  unary_operator_ast->operator_str = operator;
  unary_operator_ast->expr_ast = expr_ast;
}
unary_operator_ast_t *new_unary_operator(char *operator,
                                         expr_ast_t * expr_ast) {
  unary_operator_ast_t *unary_operator_ast =
      malloc(sizeof(unary_operator_ast_t));
  unary_operator_ast_init(unary_operator_ast, operator, expr_ast);
  return unary_operator_ast;
}
void unary_operator_print(unary_operator_ast_t *unary_operator_ast, int level) {
  print_space(level);
  printf("一元运算：\n");
  print_space(level);
  printf("运算符：%s\n", unary_operator_ast->operator_str);
  expr_print(unary_operator_ast->expr_ast, level + 1);
}

// binOp(binary_operator, exp, exp)
typedef struct binary_operator_ast {
  char *operator_str;
  expr_ast_t *left_expr;
  expr_ast_t *right_expr;
} binary_operator_ast_t;

binary_operator_ast_t *new_binary_operator(char *operator,
                                           expr_ast_t * left_expr,
                                           expr_ast_t * right_expr) {
  binary_operator_ast_t *binary_operator_ast =
      malloc(sizeof(binary_operator_ast_t));
  binary_operator_ast->operator_str = operator;
  binary_operator_ast->left_expr = left_expr;
  binary_operator_ast->right_expr = right_expr;
  return binary_operator_ast;
}
void binary_operator_print(binary_operator_ast_t *binary_operator_ast,
                           int level) {
  print_space(level);
  printf("二元运算：\n");
  print_space(level);
  printf("运算符：%s\n", binary_operator_ast->operator_str);
  print_space(level);
  printf("操作数1：\n");
  expr_print(binary_operator_ast->left_expr, level + 1);
  print_space(level);
  printf("操作数2：\n");
  expr_print(binary_operator_ast->right_expr, level + 1);
}

typedef struct conditional_ast {
  expr_ast_t *condition;
  expr_ast_t *if_expr;
  expr_ast_t *else_expr;
} conditional_ast_t;
conditional_ast_t *new_conditional_ast(expr_ast_t *condition,
                                       expr_ast_t *if_expr,
                                       expr_ast_t *else_expr) {
  conditional_ast_t *conditional_ast = malloc(sizeof(conditional_ast_t));
  conditional_ast->condition = condition;
  conditional_ast->if_expr = if_expr;
  conditional_ast->else_expr = else_expr;
  return conditional_ast;
}
void conditional_print(conditional_ast_t *conditional_ast, int level) {
  print_space(level);
  printf("条件运算：\n");
  expr_print(conditional_ast->condition, level + 1);
  print_space(level);
  printf("条件为真：\n");
  expr_print(conditional_ast->if_expr, level + 1);
  print_space(level);
  printf("条件为假：\n");
  expr_print(conditional_ast->else_expr, level + 1);
}

struct assign_ast;
typedef struct assign_ast assign_ast_t;
void assign_print(assign_ast_t *assign_ast, int level);

// expr  = Number(int) | unOp(operator, expr) |
// BinOp(binary_operator, exp, exp)| Assign(string, exp) | identifier(string)
// | Conditional(exp, exp, exp) //the three expressions are the condition,
// 'if' expression and 'else' expression, respectively
struct expr_ast {
  union {
    number_ast_t *number_ast;
    unary_operator_ast_t *unary_operator_ast;
    binary_operator_ast_t *binary_operator_ast;
    assign_ast_t *assign_ast;
    identifier_ast_t *identifier_ast;
    conditional_ast_t *conditional_ast;
  };
  int u;
};

void expr_ast_init_w_number(expr_ast_t *expr_ast, number_ast_t *number_ast) {
  expr_ast->number_ast = number_ast;
  expr_ast->u = 0;
}
expr_ast_t *new_expr_ast_w_number(number_ast_t *number_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast_init_w_number(expr_ast, number_ast);
  return expr_ast;
}
expr_ast_t *new_expr_ast_w_unary(unary_operator_ast_t *unary_operator_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast->unary_operator_ast = unary_operator_ast;
  expr_ast->u = 1;
  return expr_ast;
}
expr_ast_t *new_expr_ast_w_binary(binary_operator_ast_t *binary_operator_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast->binary_operator_ast = binary_operator_ast;
  expr_ast->u = 2;
  return expr_ast;
}
expr_ast_t *new_expr_ast_w_assign(assign_ast_t *assign_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast->assign_ast = assign_ast;
  expr_ast->u = 3;
  return expr_ast;
}
expr_ast_t *new_expr_ast_w_identifier(identifier_ast_t *identifier) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast->identifier_ast = identifier;
  expr_ast->u = 4;
  return expr_ast;
}
expr_ast_t *new_expr_ast_w_conditional(conditional_ast_t *conditional_ast) {
  expr_ast_t *expr_ast = malloc(sizeof(expr_ast_t));
  expr_ast->conditional_ast = conditional_ast;
  expr_ast->u = 5;
  return expr_ast;
}

void expr_print(expr_ast_t *expr_ast, int level) {
  print_space(level);
  printf("求值表达式：\n");
  switch (expr_ast->u) {
  case 0:
    number_print(expr_ast->number_ast, level + 1);
    break;
  case 1:
    unary_operator_print(expr_ast->unary_operator_ast, level + 1);
    break;
  case 2:
    binary_operator_print(expr_ast->binary_operator_ast, level + 1);
    break;
  case 3:
    assign_print(expr_ast->assign_ast, level + 1);
    break;
  case 4:
    identifier_print(expr_ast->identifier_ast, level + 1);
    break;
  case 5:
    conditional_print(expr_ast->conditional_ast, level + 1);
    break;
  }
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
  expr_ast_t *expr_ast; // option
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
  if (assign_ast->expr_ast == NULL) {
    print_space(level);
    printf("%s\n", "变量未初始化");
  } else {
    expr_print(assign_ast->expr_ast, level + 1);
  }
}

struct statement_ast;
typedef struct statement_ast statement_ast_t;
void statement_print(statement_ast_t *statement_ast, int level);

// If(expr, statement, statement option)
typedef struct if_ast {
  expr_ast_t *expr_ast;
  statement_ast_t *if_branch;
  statement_ast_t *else_branch;
} if_ast_t;

if_ast_t *new_if_ast(expr_ast_t *expr_ast, statement_ast_t *if_branch) {
  if_ast_t *if_ast = malloc(sizeof(if_ast_t));
  if_ast->expr_ast = expr_ast;
  if_ast->if_branch = if_branch;
  if_ast->else_branch = NULL;
  return if_ast;
}
void if_ast_add_else_branch(if_ast_t *if_ast, statement_ast_t *else_branch) {
  if_ast->else_branch = else_branch;
}
void if_ast_print(if_ast_t *if_ast, int level) {
  print_space(level);
  printf("条件语句：\n");
  expr_print(if_ast->expr_ast, level + 1);
  print_space(level);
  printf("if分支：\n");
  statement_print(if_ast->if_branch, level + 1);
  if (if_ast->else_branch != NULL) {
    print_space(level);
    printf("else分支：\n");
    statement_print(if_ast->else_branch, level + 1);
  }
}

struct block_item_ast;
typedef struct block_item_ast block_item_ast_t;
void block_item_print(block_item_ast_t *block_item_ast, int level);

typedef struct compound_ast {
  list *block_item_list;
} compound_ast_t;
compound_ast_t *new_compound_ast() {
  compound_ast_t *compound_ast = malloc(sizeof(compound_ast_t));
  compound_ast->block_item_list = list_create(NULL);
  return compound_ast;
}
void compound_add_block_item(compound_ast_t *compound_ast,
                             block_item_ast_t *block_item_ast) {
  list_push_back(compound_ast->block_item_list, block_item_ast);
}
void compound_ast_print(compound_ast_t *compound_ast, int level) {
  print_space(level);
  printf("复合语句：\n");
  block_item_ast_t *block_item = list_pop(compound_ast->block_item_list);
  while (block_item != NULL) {
    block_item_print(block_item, level + 1);
    block_item = list_pop(compound_ast->block_item_list);
  }
}

// statement = Return(exp)
//           | Declare(variable, exp option)
//           | Exp(exp)
//           | If(expr, statement, statement option)
//           | Compound(block_item list)
typedef struct statement_ast {
  union {
    return_ast_t *return_ast;
    assign_ast_t *assign_ast;
    expr_ast_t *expr_ast;
    if_ast_t *if_ast;
    compound_ast_t *compound_ast;
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
void statement_ast_init_w_expr(statement_ast_t *statement_ast,
                               expr_ast_t *expr_ast) {
  statement_ast->expr_ast = expr_ast;
  statement_ast->u = 2;
}
void statement_ast_init_w_if(statement_ast_t *statement_ast, if_ast_t *if_ast) {
  statement_ast->if_ast = if_ast;
  statement_ast->u = 3;
}
void statement_ast_init_w_compound(statement_ast_t *statement_ast, compound_ast_t *compound_ast) {
  statement_ast->compound_ast = compound_ast;
  statement_ast->u = 4;
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
  case 2:
    expr_print(statement_ast->expr_ast, level + 1);
    break;
  case 3:
    if_ast_print(statement_ast->if_ast, level + 1);
    break;
  case 4:
    compound_ast_print(statement_ast->compound_ast, level + 1);
    break;
  }
}

// block_item = Statement(statement) | Declaration(declaration)
typedef struct block_item_ast {
  union {
    statement_ast_t *statement;
    assign_ast_t *assign_ast;
  };
  int u;
} block_item_ast_t;
block_item_ast_t *new_block_item_ast() {
  block_item_ast_t *block_item_ast = malloc(sizeof(block_item_ast_t));
  return block_item_ast;
}
void block_item_init_w_statement(block_item_ast_t *block_item_ast,
                                 statement_ast_t *statement_ast) {
  block_item_ast->statement = statement_ast;
  block_item_ast->u = 0;
}
void block_item_init_w_assign(block_item_ast_t *block_item_ast,
                              assign_ast_t *assign_ast) {
  block_item_ast->assign_ast = assign_ast;
  block_item_ast->u = 1;
}
void block_item_print(block_item_ast_t *block_item_ast, int level) {
  print_space(level);
  printf("语句块：\n");
  switch (block_item_ast->u) {
  case 0:
    statement_print(block_item_ast->statement, level + 1);
    break;
  case 1:
    assign_print(block_item_ast->assign_ast, level + 1);
    break;
  }
}

// function_declaration = Function(id, block_item list)
typedef struct function_declaration_ast {
  identifier_ast_t *identifier;
  list *block_item_list;
} function_declaration_ast_t;

function_declaration_ast_t *
new_function_declaration_ast(identifier_ast_t *identifier_ast) {
  function_declaration_ast_t *function_declaration =
      malloc(sizeof(function_declaration_ast_t));
  function_declaration->identifier = identifier_ast;
  function_declaration->block_item_list = list_create(NULL);
  return function_declaration;
}
void function_declaration_add_block_item(
    function_declaration_ast_t *function_declaration_ast,
    block_item_ast_t *block_item) {
  list_push_back(function_declaration_ast->block_item_list, block_item);
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
  block_item_ast_t *block_item =
      list_pop(function_declaration_ast->block_item_list);
  while (block_item != NULL) {
    block_item_print(block_item, level + 1);
    block_item = list_pop(function_declaration_ast->block_item_list);
  }
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
