#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TitleList *createTitleList() {
  TitleList *list = (TitleList *)malloc(sizeof(TitleList));
  list->head = NULL;
  return list;
}

void appendTitle(TitleList *list, const char *title) {
  TitleNode *newNode = (TitleNode *)malloc(sizeof(TitleNode));
  strcpy(newNode->title, title);
  newNode->next = NULL;

  if (list->head == NULL) {
    list->head = newNode;
    return;
  }

  TitleNode *current = list->head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = newNode;
}

void destroyTitleList(TitleList *list) {
  TitleNode *current = list->head;
  while (current != NULL) {
    TitleNode *next = current->next;
    free(current);
    current = next;
  }
  free(list);
}
