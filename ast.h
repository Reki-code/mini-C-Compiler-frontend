#ifndef _AST_H_
#define _AST_H_

typedef struct program_ast program_ast_t;
typedef struct function_declaration_ast function_declaration_ast_t;
typedef struct block_item_ast block_item_ast_t;
typedef struct statement_ast statement_ast_t;
typedef struct function_declaration_ast function_declaration_ast_t;
typedef struct do_ast do_ast_t;
typedef struct while_ast while_ast_t;
typedef struct for_decl_ast for_decl_ast_t;
typedef struct for_ast for_ast_t;
typedef struct if_ast if_ast_t;
typedef struct assign_ast assign_ast_t;
typedef struct return_ast return_ast_t;
typedef struct function_call_ast function_call_ast_t;
typedef struct conditional_ast conditional_ast_t;
typedef struct binary_operator_ast binary_operator_ast_t;
typedef struct unary_operator_ast unary_operator_ast_t;
typedef struct identifier_ast identifier_ast_t;
typedef struct number_ast number_ast_t;
typedef struct expr_ast expr_ast_t;

typedef struct compound_ast compound_ast_t;
// program
program_ast_t *
new_program_ast(function_declaration_ast_t *function_declaration);
void program_add_function_declarartion(
    program_ast_t *program_ast,
    function_declaration_ast_t *function_declaration_ast);
void program_print(program_ast_t *program_ast);
//  function_declaration
function_declaration_ast_t *
new_function_declaration_ast(identifier_ast_t *function_name);
void function_declaration_add_parameter(
    function_declaration_ast_t *function_declaration,
    identifier_ast_t *identifier_ast);
void function_declaration_add_block_item(
    function_declaration_ast_t *function_declaration_ast,
    block_item_ast_t *block_item);
void function_declaration_print(
    function_declaration_ast_t *function_declaration_ast, int level);
// expr
void expr_ast_init_w_number(expr_ast_t *expr_ast, number_ast_t *number_ast);
expr_ast_t *new_expr_ast_w_number(number_ast_t *number_ast);
expr_ast_t *new_expr_ast_w_unary(unary_operator_ast_t *unary_operator_ast);
expr_ast_t *new_expr_ast_w_binary(binary_operator_ast_t *binary_operator_ast);
expr_ast_t *new_expr_ast_w_assign(assign_ast_t *assign_ast);
expr_ast_t *new_expr_ast_w_identifier(identifier_ast_t *identifier);
expr_ast_t *new_expr_ast_w_conditional(conditional_ast_t *conditional_ast);
expr_ast_t *
new_expr_ast_w_function_call(function_call_ast_t *function_call_ast);
expr_ast_t *new_null_expr();
void expr_print(expr_ast_t *expr_ast, int level);
// number literals
number_ast_t *new_number_ast(int value);
void number_print(number_ast_t *number_ast, int level);
// identifier
identifier_ast_t *new_identifier_ast(char *name);
void identifier_print(identifier_ast_t *identifier_ast, int level);
// unOp(operator, expr)
unary_operator_ast_t *new_unary_operator(char *operator, expr_ast_t * expr_ast);
void unary_operator_print(unary_operator_ast_t *unary_operator_ast, int level);
// binOp(binary_operator, exp, exp)
binary_operator_ast_t *new_binary_operator(char *operator,
                                           expr_ast_t * left_expr,
                                           expr_ast_t * right_expr);
void binary_operator_print(binary_operator_ast_t *binary_operator_ast,
                           int level);
// assign
assign_ast_t *new_assign_ast(identifier_ast_t *identifier, expr_ast_t *expr);
void assign_print(assign_ast_t *assign_ast, int level);
// identifier
identifier_ast_t *new_identifier_ast(char *name);
void identifier_print(identifier_ast_t *identifier_ast, int level);
// Conditional(exp, exp, exp)
conditional_ast_t *new_conditional_ast(expr_ast_t *condition,
                                       expr_ast_t *if_expr,
                                       expr_ast_t *else_expr);
void conditional_print(conditional_ast_t *conditional_ast, int level);
// FunCall(string, exp list) // string is the function name
function_call_ast_t *new_function_call_ast(identifier_ast_t *function_name);
void function_call_add_argument(function_call_ast_t *function_call_ast,
                                expr_ast_t *argument);
void function_call_print(function_call_ast_t *function_call_ast, int level);
// statement
statement_ast_t *new_statement_ast();
void statement_ast_init_w_return(statement_ast_t *statement_ast,
                                 return_ast_t *return_ast);
void statement_ast_init_w_assign(statement_ast_t *statement_ast,
                                 assign_ast_t *assign_ast);
void statement_ast_init_w_expr(statement_ast_t *statement_ast,
                               expr_ast_t *expr_ast);
void statement_ast_init_w_if(statement_ast_t *statement_ast, if_ast_t *if_ast);
void statement_ast_init_w_compound(statement_ast_t *statement_ast,
                                   compound_ast_t *compound_ast);
void statement_ast_init_w_for(statement_ast_t *statement_ast,
                              for_ast_t *for_ast);
void statement_ast_init_w_for_decl(statement_ast_t *statement_ast,
                                   for_decl_ast_t *for_decl_ast);
void statement_ast_init_w_while(statement_ast_t *statement_ast,
                                while_ast_t *while_ast);
void statement_ast_init_w_do(statement_ast_t *statement_ast, do_ast_t *do_ast);
void statement_ast_init_w_break(statement_ast_t *statement_ast);
void statement_ast_init_w_continue(statement_ast_t *statement_ast);
void statement_print(statement_ast_t *statement_ast, int level);
// return
return_ast_t *new_return_ast(expr_ast_t *expr_ast);
void return_ast_init(return_ast_t *return_ast, expr_ast_t *expr_ast);
void return_print(return_ast_t *return_ast, int level);
// if
if_ast_t *new_if_ast(expr_ast_t *expr_ast, statement_ast_t *if_branch);
void if_ast_add_else_branch(if_ast_t *if_ast, statement_ast_t *else_branch);
void if_ast_print(if_ast_t *if_ast, int level);
// compound
compound_ast_t *new_compound_ast();
void compound_add_block_item(compound_ast_t *compound_ast,
                             block_item_ast_t *block_item_ast);
void compound_ast_print(compound_ast_t *compound_ast, int level);
// for
for_ast_t *new_for_ast();
void for_ast_set_initial(for_ast_t *for_ast, expr_ast_t *initial);
void for_ast_set_condition(for_ast_t *for_ast, expr_ast_t *condition);
void for_ast_set_post(for_ast_t *for_ast, expr_ast_t *post);
void for_ast_set_body(for_ast_t *for_ast, statement_ast_t *body);
void for_ast_print(for_ast_t *for_ast, int level);
// ForDecl
for_decl_ast_t *new_for_decl_ast();
void for_decl_set_initial(for_decl_ast_t *for_decl_ast, assign_ast_t *initial);
void for_decl_set_condition(for_decl_ast_t *for_decl_ast,
                            expr_ast_t *condition);
void for_decl_set_post(for_decl_ast_t *for_decl_ast, expr_ast_t *post);
void for_decl_set_body(for_decl_ast_t *for_decl_ast, statement_ast_t *body);
void for_decl_ast_print(for_decl_ast_t *for_decl_ast, int level);
// while
while_ast_t *new_while_ast();
void while_ast_set_condition(while_ast_t *while_ast, expr_ast_t *condition);
void while_ast_set_body(while_ast_t *while_ast, statement_ast_t *body);
void while_ast_print(while_ast_t *while_ast, int level);
// do
do_ast_t *new_do_ast();
void do_ast_set_body(do_ast_t *do_ast, statement_ast_t *body);
void do_ast_set_condition(do_ast_t *do_ast, expr_ast_t *condition);
void do_ast_print(do_ast_t *do_ast, int level);
// function_declaration
function_declaration_ast_t *
new_function_declaration_ast(identifier_ast_t *function_name);
void function_declaration_add_parameter(
    function_declaration_ast_t *function_declaration,
    identifier_ast_t *identifier_ast);
void function_declaration_add_block_item(
    function_declaration_ast_t *function_declaration_ast,
    block_item_ast_t *block_item);
void function_declaration_print(
    function_declaration_ast_t *function_declaration_ast, int level);
// block_item
block_item_ast_t *new_block_item_ast();
void block_item_init_w_statement(block_item_ast_t *block_item_ast,
                                 statement_ast_t *statement_ast);
void block_item_init_w_assign(block_item_ast_t *block_item_ast,
                              assign_ast_t *assign_ast);
void block_item_print(block_item_ast_t *block_item_ast, int level);

void print_space(int level);

#endif
