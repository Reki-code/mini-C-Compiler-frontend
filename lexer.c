#include "lexer.h"
#include "list.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *keyword_array[] = {"int", "return", "if", "else"};
type_t keyword_type[] = {int_k, return_k, if_k, else_k};
int keyword_len = sizeof(keyword_array) / sizeof(char *);
int keyword_check(char *string) {
  for (int i = 0; i < keyword_len; i++) {
    if (strcmp(keyword_array[i], string) == 0) {
      return i;
    }
  }
  return -1;
}

list *lexer(FILE *fp) {
  list *token_list = list_create(NULL);
  char *buffer;
  long numbytes = 1024;
  buffer = (char *)calloc(numbytes, sizeof(char));
  if (buffer == NULL) {
    fprintf(stderr, "%s\n", "内存分配失败");
    exit(-2);
  }
  size_t read_size = fread(buffer, sizeof(char), numbytes, fp);
  fclose(fp);

  long curr_index = 0;
  char last_char = buffer[curr_index];

  for (; last_char != '\0'; curr_index++, last_char = buffer[curr_index]) {

    while (isspace(last_char)) {
      curr_index++;
      last_char = buffer[curr_index];
    }
    token_t *new_token;
    char peek;
    switch (last_char) {
    case '{':
      new_token = malloc(sizeof(token_t));
      new_token->type = open_brace;
      new_token->value = "{";
      list_push_back(token_list, new_token);
      continue;
    case '}':
      new_token = malloc(sizeof(token_t));
      new_token->type = close_brace;
      new_token->value = "}";
      list_push_back(token_list, new_token);
      continue;
    case '(':
      new_token = malloc(sizeof(token_t));
      new_token->type = open_parenthesis;
      new_token->value = "(";
      list_push_back(token_list, new_token);
      continue;
    case ')':
      new_token = malloc(sizeof(token_t));
      new_token->type = close_parenthesis;
      new_token->value = ")";
      list_push_back(token_list, new_token);
      continue;
    case ';':
      new_token = malloc(sizeof(token_t));
      new_token->type = semicolon;
      new_token->value = ";";
      list_push_back(token_list, new_token);
      continue;
    case '-':
      new_token = malloc(sizeof(token_t));
      new_token->type = negation;
      new_token->value = "-";
      list_push_back(token_list, new_token);
      continue;
    case '~':
      new_token = malloc(sizeof(token_t));
      new_token->type = bitwise_complement;
      new_token->value = "~";
      list_push_back(token_list, new_token);
      continue;
    case ':':
      new_token = malloc(sizeof(token_t));
      new_token->type = colon;
      new_token->value = ":";
      list_push_back(token_list, new_token);
      continue;
    case '?':
      new_token = malloc(sizeof(token_t));
      new_token->type = question_mark;
      new_token->value = "?";
      list_push_back(token_list, new_token);
      continue;

    case '!':
      new_token = malloc(sizeof(token_t));
      peek = buffer[curr_index + 1];
      if (peek == '=') { // "!="
        curr_index++;
        new_token->type = not_equal;
        new_token->value = "!=";
      } else {
        new_token->type = logical_negation;
        new_token->value = "!";
      }
      list_push_back(token_list, new_token);
      continue;
    case '+':
      new_token = malloc(sizeof(token_t));
      new_token->type = addition;
      new_token->value = "+";
      list_push_back(token_list, new_token);
      continue;
    case '*':
      new_token = malloc(sizeof(token_t));
      new_token->type = multiplication;
      new_token->value = "*";
      list_push_back(token_list, new_token);
      continue;
    case '/':
      peek = buffer[curr_index + 1];
      if (peek == '/') { // "//"
        for (; last_char != '\n';
             curr_index++, last_char = buffer[curr_index]) {
        }
        continue;
      } else {
        new_token = malloc(sizeof(token_t));
        new_token->type = division;
        new_token->value = "/";
        list_push_back(token_list, new_token);
        continue;
      }
    case '=':
      new_token = malloc(sizeof(token_t));
      peek = buffer[curr_index + 1];
      if (peek == '=') { // "=="
        curr_index++;
        new_token->type = equal;
        new_token->value = "==";
      } else {
        new_token->type = assign;
        new_token->value = "=";
      }
      list_push_back(token_list, new_token);
      continue;
    case '&':
      peek = buffer[curr_index + 1];
      if (peek == '&') { // "&&"
        new_token = malloc(sizeof(token_t));
        curr_index++;
        new_token->type = and_k;
        new_token->value = "&&";
        list_push_back(token_list, new_token);
        continue;
      }
      break;
    case '|':
      peek = buffer[curr_index + 1];
      if (peek == '|') { // "||"
        new_token = malloc(sizeof(token_t));
        curr_index++;
        new_token->type = or_k;
        new_token->value = "||";
        list_push_back(token_list, new_token);
        continue;
      }
      break;
    case '<':
      new_token = malloc(sizeof(token_t));
      peek = buffer[curr_index + 1];
      if (peek == '=') { // "<="
        curr_index++;
        new_token->type = less_than_or_equal;
        new_token->value = "<=";
      } else { // "<"
        new_token->type = less_than;
        new_token->value = "<";
      }
      list_push_back(token_list, new_token);
      continue;
    case '>':
      new_token = malloc(sizeof(token_t));
      peek = buffer[curr_index + 1];
      if (peek == '=') { // ">="
        curr_index++;
        new_token->type = greater_than_or_equal;
        new_token->value = ">=";
      } else { // "<"
        new_token->type = greater_than;
        new_token->value = ">";
      }
      list_push_back(token_list, new_token);
      continue;
    }

    // identifier and keyword
    if (isalpha(last_char)) {
      char *str_value = malloc(20);
      str_value[0] = last_char;
      int str_index = 0;

      curr_index++;
      last_char = buffer[curr_index];
      while (isdigit(last_char) || isalpha(last_char)) {
        str_index++;
        str_value[str_index] = last_char;
        curr_index++;
        last_char = buffer[curr_index];
      }
      curr_index--;
      str_index++;
      str_value[str_index] = '\0';
      int c = keyword_check(str_value);
      token_t *new_token = malloc(sizeof(token_t));
      if (c == -1) {
        new_token->type = identifier;
        new_token->value = str_value;
      } else {
        new_token->type = keyword_type[c];
        new_token->value = keyword_array[c];
      }
      list_push_back(token_list, new_token);
      continue;
    }
    // number
    if (isdigit(last_char)) {
      char *str_value = malloc(20);
      str_value[0] = last_char;
      int str_index = 0;

      curr_index++;
      last_char = buffer[curr_index];
      while (isdigit(last_char)) {
        str_index++;
        str_value[str_index] = last_char;
        curr_index++;
        last_char = buffer[curr_index];
      }
      curr_index--;
      str_index++;
      str_value[str_index] = '\0';
      token_t *new_token = malloc(sizeof(token_t));
      new_token->type = number;
      new_token->value = str_value;
      list_push_back(token_list, new_token);
      continue;
    }
    // "string"
    if (last_char == '\"') {
      curr_index++;
      last_char = buffer[curr_index];
      int len = 0;
      while (last_char != '\"') {
        if (last_char == EOF) {
          fprintf(stderr, "%s\n", "not a string");
          exit(-2);
        }
        curr_index++;
        last_char = buffer[curr_index];
        len++;
      }
      char *str_value = malloc(len + 1);
      strncpy(str_value, buffer + curr_index - len, len);
      token_t *new_token = malloc(sizeof(token_t));
      new_token->type = literal;
      new_token->value = str_value;
      list_push_back(token_list, new_token);
      continue;
    }
  }
  token_t *new_token = malloc(sizeof(token_t));
  new_token->type = eof;
  new_token->value = "EOF";
  list_push_back(token_list, new_token);

  free(buffer);
  return token_list;
}
