#include "token.h"
#include "lexer.h"
#include <stdio.h>

void print_token(void *token) {
  token_t *tp = token;
  printf("(%s  \t'%s')\n", get_type_name(tp->type), tp->value);
}

char *type_names[] = {
  [open_brace] = "左大括号",
  [close_brace] = "右大括号",
  [open_parenthesis] = "左小括号",
  [close_parenthesis] = "右小括号",
  [semicolon] = "分号",
	[identifier] = "标识符",
	[return_k] = "关键字return",
  [int_k] = "关键字int",
	[number] = "整数常量",
  [assign] = "赋值",
  [negation] = "负号",
  [bitwise_complement] = "位取反",
  [logical_negation] = "逻辑非",
  [addition] = "加号",
  [multiplication] = "乘号",
  [division] = "除号",
  [and_k] = "逻辑与",
  [or_k] = "逻辑或",
  [equal] = "相等",
  [not_equal] = "不相等",
  [less_than] = "小于",
  [less_than_or_equal] = "小于或等于",
  [greater_than] = "大于",
  [greater_than_or_equal] = "大于等于",
	[operator] = "运算符",
	[literal] = "字符串字面量",
	[eof] = "结束标记"
};

char *get_type_name(type_t type) { return type_names[type]; }
