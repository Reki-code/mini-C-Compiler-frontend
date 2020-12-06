#ifndef _TOKEN_H_
#define _TOKEN_H_

enum type_e {
  open_brace = 0, close_brace, open_parenthesis, close_parenthesis, semicolon,
  identifier, return_k, int_k, assign, number, negation, bitwise_complement,
  logical_negation, addition, multiplication, division, and_k, or_k, equal,
  not_equal, less_than, less_than_or_equal, greater_than, greater_than_or_equal,
  if_k, else_k, colon, question_mark, for_k, while_k, do_k, break_k,
  continue_k, commas, literal, eof };
typedef enum type_e type_t;
typedef struct token {
  type_t type;
  char *value;
} token_t;

char *get_type_name(type_t type);
void print_token(void *token);

#endif
