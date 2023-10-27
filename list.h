#ifndef LIST_H
#define LIST_H

typedef struct TitleNode {
  char title[256];
  struct TitleNode *next;
} TitleNode;

typedef struct TitleList {
  TitleNode *head;
} TitleList;

TitleList *createTitleList();
void appendTitle(TitleList *list, const char *title);
void destroyTitleList(TitleList *list);

#endif
