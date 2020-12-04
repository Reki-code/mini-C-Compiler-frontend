#include <stdlib.h>
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

// expr =  = Expr(int)
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

// function declaration =  = Function(id, statement)
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
