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

void list_push(list *list, void *data) {
  if (list == NULL) {
    fprintf(stderr, "list_push: list not created\n");
    exit(-1);
  }
  struct node *new_node = malloc(sizeof(struct node));
  new_node->data = data;

  new_node->next = list->head;
  if (list->head == NULL) {
    list->head = list->tail = new_node;
  }
  list->head = new_node;
}

void list_push_back(list *list, void *data) {
  if (list == NULL) {
    fprintf(stderr, "list_push: list not created\n");
    exit(-1);
  }
  struct node *new_node = malloc(sizeof(struct node));
  new_node->data = data;

  new_node->next = NULL;
  if (list->tail == NULL) {
    list->head = list->tail = new_node;
    return;
  }
  list->tail->next = new_node;
  list->tail = new_node;
}

void *list_pop(list *list) {
  struct node *popped_data = list->head;
  list->head = popped_data->next;
  popped_data->next = NULL;

  return popped_data;
}

void list_print(list *list, void (*print)(void *)) {
  struct node *curr = list->head;
  while (curr != NULL) {
    print(curr->data);
    curr = curr->next;
  }
}
