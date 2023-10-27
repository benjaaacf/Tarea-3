#ifndef MAP_H
#define MAP_H

#define MAX_WORD_LEN 100
#define HASH_TABLE_SIZE 1000

typedef struct WordNode {
  char word[MAX_WORD_LEN];
  int frequency;
  struct TitleList *titles; // Lista de titulos
  struct WordNode *next;
} WordNode;

typedef struct Map {
  int size;
  WordNode **table;
} Map;

Map *createMap(int size);
void insertWord(Map *map, const char *word, const char *title);
WordNode *searchWord(Map *map, const char *word);
void destroyMap(Map *map);

#endif
