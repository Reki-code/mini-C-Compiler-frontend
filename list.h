#ifndef _LIST_H_
#define _LIST_H_

struct node {
    void *data;
    struct node *next;
};

typedef struct list {
  struct node *head;
  struct node *tail;
} list;

list *list_create(void *data);

void list_free(list *list);

void list_push(list *list, void *data);
void list_push_back(list *list, void *data);

void *list_pop(list *list);

void *list_peek(list *list);

void list_print(list *list, void (*print)(void *data));

#endif
