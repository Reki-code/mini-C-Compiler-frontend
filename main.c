#include <stdio.h>
#include "token.h"
#include "list.h"
#include "lexer.h"


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
  printf("%s\n", "lexer exit");

  list_print(token_list, print_token);

  return 0;
}
