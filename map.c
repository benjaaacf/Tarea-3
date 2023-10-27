#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

Map *createMap(int size) {
  Map *map = (Map *)malloc(sizeof(Map));
  map->size = size;
  map->table = (WordNode **)malloc(sizeof(WordNode *) * size);
  for (int i = 0; i < size; i++) {
    map->table[i] = NULL;
  }
  return map;
}

int hash(const char *key, int size) {
  unsigned long hash = 0;
  for (int i = 0; key[i]; i++) {
    hash = (hash * 31 + key[i]) % size;
  }
  return (int)hash;
}

void insertWord(Map *map, const char *word, const char *title) {
  int index = hash(word, map->size);
  WordNode *entry = map->table[index];

  while (entry != NULL) {
    if (strcmp(entry->word, word) == 0) {
      entry->frequency++;
      // Agregar el título a la lista de títulos
      appendTitle(entry->titles, title);
      return;
    }
    entry = entry->next;
  }

  WordNode *new_word = (WordNode *)malloc(sizeof(WordNode));
  strcpy(new_word->word, word);
  new_word->frequency = 1;
  new_word->titles = createTitleList();
  appendTitle(new_word->titles, title);
  new_word->next = map->table[index];
  map->table[index] = new_word;
}

WordNode *searchWord(Map *map, const char *word) {
  int index = hash(word, map->size);
  WordNode *entry = map->table[index];
  while (entry != NULL) {
    if (strcmp(entry->word, word) == 0) {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

void destroyMap(Map *map) {
  for (int i = 0; i < map->size; i++) {
    WordNode *entry = map->table[i];
    while (entry != NULL) {
      WordNode *next = entry->next;
      destroyTitleList(entry->titles);
      free(entry);
      entry = next;
    }
  }
  free(map->table);
  free(map);
}
