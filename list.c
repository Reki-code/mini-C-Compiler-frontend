#include "list.h"
#include <stdio.h>
#include <stdlib.h>

list *list_create(void *new_data) {
  list *new_list = malloc(sizeof(list));
  new_list->head =new_list->tail = NULL;

  return new_list;
}

void list_free(list *list) {
  // TODO: free list
}

struct node *new_node(void *data) {
    struct node *new_node = malloc(sizeof(struct node));
    new_node->data = data;
    return new_node;
}

void list_push(list *list, void *data) {
  if (list == NULL) {
    fprintf(stderr, "list_push: list not created\n");
    exit(-1);
  }
  struct node *n = new_node(data);

  n->next = list->head;
  if (list->head == NULL) {
    list->head = list->tail = n;
  }
  list->head = n;
}

void list_push_back(list *list, void *data) {
  if (list == NULL) {
    fprintf(stderr, "list_push: list not created\n");
    exit(-1);
  }
  struct node *n = new_node(data);

  n->next = NULL;
  if (list->tail == NULL) {
    list->head = list->tail = n;
    return;
  }
  list->tail->next = n;
  list->tail = n;
}

void *list_pop(list *list) {
  struct node *data = list->head;
  list->head = data->next;
  return data->data;
}

void *list_peek(list *list) {
  return list->head->data;
}

void list_print(list *list, void (*print)(void *)) {
  struct node *curr = list->head;
  while (curr != NULL) {
    print(curr->data);
    curr = curr->next;
  }
}
