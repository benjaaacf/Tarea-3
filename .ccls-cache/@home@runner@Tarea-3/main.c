#include "list.h"
#include "map.h"
#include <ctype.h>
#include <dirent.h> // Agregamos esta librería para las funciones de directorio
#include <limits.h> // Agregamos esta librería para la constante PATH_MAX
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Función para convertir una cadena a minúsculas.
void toLowerCase(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
}

int totalBooks = 0;

void cargarDocs(Map *wordMap, const char *location) {
  struct stat path_stat;
  if (stat(location, &path_stat) == 0) {
    if (S_ISDIR(path_stat.st_mode)) {
      // La entrada es una carpeta, cargar todos los archivos en la carpeta
      DIR *dir = opendir(location);
      if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
          if (entry->d_type == DT_REG) { // Verificar si es un archivo regular
            char file_path[PATH_MAX];
            snprintf(file_path, PATH_MAX, "%s/%s", location, entry->d_name);
            FILE *file = fopen(file_path, "r");
            if (file != NULL) {
              char title[256] = ""; // Inicializar title como una cadena vacía

              char line[256];
              bool title_read =
                  false; // Bandera para controlar la lectura del título
              while (fgets(line, sizeof(line), file)) {
                if (!title_read) {
                  char *title_start = strstr(line, "Title:");
                  if (title_start != NULL) {
                    title_start += strlen("Title:");
                    strcpy(title, title_start);
                    // Convierte la palabra a minúsculas antes de insertarla en
                    // la tabla hash
                    toLowerCase(title);
                    title_read = true;
                  }
                }

                char *word = strtok(line, " \t\n");
                while (word != NULL) {
                  // Convierte la palabra a minúsculas antes de insertarla en la
                  // tabla hash
                  toLowerCase(word);
                  insertWord(wordMap, word, title);
                  word = strtok(NULL, " \t\n");
                }
              }

              fclose(file);
              totalBooks++;
            } else {
              printf("No se pudo abrir el archivo: %s\n", file_path);
            }
          }
        }
        closedir(dir);
      } else {
        printf("No se pudo abrir la carpeta: %s\n", location);
      }
    } else if (S_ISREG(path_stat.st_mode)) {
      // La entrada es un nombre de archivo, cargar el archivo individualmente
      FILE *file = fopen(location, "r");
      if (file != NULL) {
        char title[256] = "";    // Inicializar title como una cadena vacía
        bool title_read = false; // Bandera para controlar la lectura del título

        char line[256];
        while (fgets(line, sizeof(line), file)) {
          if (!title_read) {
            char *title_start = strstr(line, "Title:");
            if (title_start != NULL) {
              title_start += strlen("Title:");
              strcpy(title, title_start);
              // Convierte la palabra a minúsculas antes de insertarla en la
              // tabla hash
              toLowerCase(title);
              title_read = true;
            }
          }

          char *word = strtok(line, " \t\n");
          while (word != NULL) {
            // Convierte la palabra a minúsculas antes de insertarla en la tabla
            // hash
            toLowerCase(word);
            insertWord(wordMap, word, title);
            word = strtok(NULL, " \t\n");
          }
        }

        fclose(file);
        totalBooks++;
      } else {
        printf("No se pudo abrir el archivo: %s\n", location);
      }
    } else {
      printf("La ubicación especificada no es valida: %s\n", location);
    }
  } else {
    printf("La ubicacion especificada no existe: %s\n", location);
  }
}

void mostrarDocsOrdenados(const Map *wordMap) {
  printf("\nDocumentos ordenados por titulo:\n\n");

  // Crear un conjunto para almacenar títulos únicos
  char uniqueTitles[256][256];

  int uniqueTitleCount = 0;

  // Iterar a través del mapa y recolectar títulos únicos
  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      const char *currentTitle = entry->titles->head->title;
      bool isUnique = true;

      // Verificar si el título ya se ha agregado al conjunto de títulos únicos
      for (int j = 0; j < uniqueTitleCount; j++) {
        if (strcmp(currentTitle, uniqueTitles[j]) == 0) {
          isUnique = false;
          break;
        }
      }

      if (isUnique) {
        strcpy(uniqueTitles[uniqueTitleCount], currentTitle);
        uniqueTitleCount++;
      }

      entry = entry->next;
    }
  }

  // Mostrar los titulos unicos
  for (int i = 0; i < uniqueTitleCount; i++) {
    printf("%s\n", uniqueTitles[i]);
  }
}

// Función para buscar un libro por titulo
void buscarPorTitulo(const Map *wordMap, const char *keywords) {
  // Crear un conjunto para almacenar titulos unicos que coincidan con las
  // palabras clave
  char matchingTitles[256][256];

  int matchingTitleCount = 0;

  // Convertir las palabras clave a minusculas
  char keywordsLowerCase[256];
  strcpy(keywordsLowerCase, keywords);
  toLowerCase(keywordsLowerCase);

  // Buscar titulos que coincidan con las palabras clave
  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      const char *currentTitle = entry->titles->head->title;
      bool isUnique = true;

      // Verificar si el titulo ya se ha agregado al conjunto de titulos unicos
      for (int j = 0; j < matchingTitleCount; j++) {
        if (strcmp(currentTitle, matchingTitles[j]) == 0) {
          isUnique = false;
          break;
        }
      }

      if (isUnique && strstr(currentTitle, keywordsLowerCase) != NULL) {
        strcpy(matchingTitles[matchingTitleCount], currentTitle);
        matchingTitleCount++;
      }

      entry = entry->next;
    }
  }

  // Mostrar los titulos que coinciden con las palabras clave
  for (int i = 0; i < matchingTitleCount; i++) {
    printf("%s\n", matchingTitles[i]);
  }
}

void palabrasFrecuentes(const Map *wordMap, const char *title) {
  // Encontrar el libro por su titulo
  TitleList *titleList = createTitleList();

  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      TitleNode *titleNode = entry->titles->head;
      while (titleNode != NULL) {
        if (strcmp(titleNode->title, title) == 0) {
          appendTitle(titleList, entry->titles->head->title);
          break;
        }
        titleNode = titleNode->next;
      }
      entry = entry->next;
    }
  }

  // Crear un contador para las palabras
  typedef struct {
    char word[MAX_WORD_LEN];
    int frequency;
  } WordCounter;

  WordCounter counters[10];
  for (int i = 0; i < 10; i++) {
    strcpy(counters[i].word, "");
    counters[i].frequency = 0;
  }

  // Contar las palabras en el libro
  TitleNode *current = titleList->head;
  while (current != NULL) {
    char line[256];
    FILE *file = fopen(current->title, "r");
    if (file != NULL) {
      while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " \t\n");
        while (token != NULL) {
          for (int i = 0; i < 10; i++) {
            if (strcmp(token, counters[i].word) == 0) {
              counters[i].frequency++;
              break;
            }
            if (counters[i].frequency < counters[i + 1].frequency) {
              strcpy(counters[i].word, token);
              counters[i].frequency = 1;
              break;
            }
          }
          token = strtok(NULL, " \t\n");
        }
      }
      fclose(file);
    }
    current = current->next;
  }

  // Mostrar las 10 palabras con mayor frecuencia
  for (int i = 0; i < 10; i++) {
    if (counters[i].frequency > 0) { // Solo mostrar palabras con frecuencia > 0
      printf("Palabra: %s, Frecuencia: %d\n", counters[i].word,
             counters[i].frequency);
    }
  }

  // Liberar la memoria de la lista
  destroyTitleList(titleList);
}

// Funcion para mostrar las 10 palabras mas relevantes en un libro
typedef struct {
  char word[256];
  int frequency;
  double relevance;
} WordInfo;

void palabrasRelevantes(const Map *wordMap, const char *title,
                        int totalDocuments) {
  TitleList *titleList = createTitleList();

  // Buscar el libro por su titulo
  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      if (strcmp(entry->titles->head->title, title) == 0) {
        TitleNode *titleNode = entry->titles->head;
        while (titleNode != NULL) {
          appendTitle(titleList, titleNode->title);
          titleNode = titleNode->next;
        }
      }
      entry = entry->next;
    }
  }

  // Crear un contador para las palabras
  WordInfo wordInfo[10];
  for (int i = 0; i < 10; i++) {
    strcpy(wordInfo[i].word, "");
    wordInfo[i].frequency = 0;
    wordInfo[i].relevance = 0.0;
  }

  int totalWords = 0; // Contador de palabras totales en el libro

  // Contar las palabras en el libro y actualizar las frecuencias
  FILE *file = fopen(title, "r");
  if (file != NULL) {
    char line[256];
    while (fgets(line, sizeof(line), file)) {
      char *token = strtok(line, " \t\n");
      while (token != NULL) {
        totalWords++; // Incrementar el contador de palabras totales

        for (int i = 0; i < 10; i++) {
          if (strcmp(token, wordInfo[i].word) == 0) {
            wordInfo[i].frequency++;
            break;
          }
          if (wordInfo[i].frequency < wordInfo[i + 1].frequency) {
            strcpy(wordInfo[i].word, token);
            wordInfo[i].frequency = 1;
            break;
          }
        }
        token = strtok(NULL, " \t\n");
      }
    }
    fclose(file);
  }

  // Calcular la relevancia de las palabras
  for (int i = 0; i < 10; i++) {
    if (wordInfo[i].frequency > 0) {
      // Calcular la relevancia usando la formula mencionada
      double relevance =
          (double)wordInfo[i].frequency / totalWords *
          log((double)totalDocuments / (double)wordInfo[i].frequency);
      wordInfo[i].relevance = relevance;
    }
  }

  // Mostrar las 10 palabras mas relevantes
  for (int i = 0; i < 10; i++) {
    if (wordInfo[i].frequency > 0) { // Solo mostrar palabras con frecuencia > 0
      printf("Palabra: %s, Frecuencia: %d, Relevancia: %.6lf\n",
             wordInfo[i].word, wordInfo[i].frequency, wordInfo[i].relevance);
    }
  }

  // Liberar la memoria de la lista de titulos
  destroyTitleList(titleList);
}

// Función para buscar libros por palabra
void buscarLibrosPorPalabra(const Map *wordMap, const char *word) {
  // Convertir la palabra a minusculas
  char wordLowerCase[256];
  strcpy(wordLowerCase, word);
  toLowerCase(wordLowerCase);

  // Crear un conjunto para almacenar titulos únicos de libros que contienen la
  // palabra
  char matchingTitles[256][256];

  int matchingTitleCount = 0;

  // Buscar libros que contienen la palabra
  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      if (strcmp(entry->word, wordLowerCase) == 0) {
        TitleNode *titleNode = entry->titles->head;
        while (titleNode != NULL) {
          const char *currentTitle = titleNode->title;
          bool isUnique = true;

          // Verificar si el titulo ya se ha agregado al conjunto de titulos
          // unicos
          for (int j = 0; j < matchingTitleCount; j++) {
            if (strcmp(currentTitle, matchingTitles[j]) == 0) {
              isUnique = false;
              break;
            }
          }

          if (isUnique) {
            strcpy(matchingTitles[matchingTitleCount], currentTitle);
            matchingTitleCount++;
          }

          titleNode = titleNode->next;
        }
      }
      entry = entry->next;
    }
  }

  // Mostrar los titulos de los libros que contienen la palabra
  for (int i = 0; i < matchingTitleCount; i++) {
    printf("%s\n", matchingTitles[i]);
  }
}

// Funcion para mostrar una palabra en su contexto dentro del libro
void mostrarContexto(const Map *wordMap, const char *title, const char *word) {
  // Buscar el libro por su título
  TitleList *titleList = createTitleList();
  for (int i = 0; i < wordMap->size; i++) {
    WordNode *entry = wordMap->table[i];
    while (entry != NULL) {
      if (strcmp(entry->titles->head->title, title) == 0) {
        TitleNode *titleNode = entry->titles->head;
        while (titleNode != NULL) {
          appendTitle(titleList, titleNode->title);
          titleNode = titleNode->next;
        }
      }
      entry = entry->next;
    }
  }

  // Mostrar el contexto de la palabra en el libro
  TitleNode *current = titleList->head;
  while (current != NULL) {
    FILE *file = fopen(current->title, "r");
    if (file != NULL) {
      char line[256];
      while (fgets(line, sizeof(line), file)) {
        if (strstr(line, word) != NULL) {
          printf("%s: %s", current->title, line);
        }
      }
      fclose(file);
    }
    current = current->next;
  }

  // Liberar la memoria de la lista de titulos
  destroyTitleList(titleList);
}

int main() {
  // Crear una tabla hash para las palabras
  Map *wordMap = createMap(HASH_TABLE_SIZE);

  int opcion;
  char arr[256];

  while (1) {
    printf("==============================\n");
    printf("|    Buscador de documentos  |\n");
    printf("==============================\n");
    printf("1. Cargar documentos\n");
    printf("2. Mostrar documentos ordenados por titulo\n");
    printf("3. Buscar un libro por titulo\n");
    printf("4. Palabras con mayor frecuencia en un libro\n");
    printf("5. Palabras mas relevantes en un libro\n");
    printf("6. Buscar libros por palabra\n");
    printf("7. Mostrar palabra en su contexto dentro del libro\n");
    printf("8. Salir\n");
    printf("Seleccione una opcion: ");
    scanf("%d", &opcion);

    while (getchar() != '\n')
      ;

    if (opcion == 8) {
      break;
    }

    switch (opcion) {
    case 1:
      // Cargar documentos
      printf("Ingrese los nombres del .TXT o carpeta: ");
      char input[256];
      scanf(" %[^\n]s", input);
      printf("Cargando documentos...\n");
      cargarDocs(wordMap, input);
      printf("¡Documentos cargados!\n");
      break;
    case 2:
      mostrarDocsOrdenados(wordMap);
      break;
    case 3:
      printf("Ingrese palabras clave para buscar por titulo: ");
      scanf(" %[^\n]s", arr);
      buscarPorTitulo(wordMap, arr);
      break;
    case 4:
      printf("Ingrese el titulo de un libro para las palabras con "
             "mayor frecuencia: ");
      scanf(" %[^\n]s", arr);
      palabrasFrecuentes(wordMap, arr);
      break;
    case 5:
      printf("Ingrese el titulo de un libro para las palabras mas "
             "relevantes: ");
      scanf(" %[^\n]s", arr);
      palabrasRelevantes(wordMap, arr, totalBooks);
      break;
    case 6:
      printf("Ingrese una palabra para buscar libros que la contengan: ");

      buscarLibrosPorPalabra(wordMap, arr);
      break;
    case 7:
      printf("Ingrese el titulo de un libro: ");
      scanf(" %[^\n]s", arr);
      char word[256];
      printf("Ingrese una palabra para buscar en el contexto: ");
      scanf(" %[^\n]s", word);
      mostrarContexto(wordMap, arr, word);
      break;
    default:
      printf("Opcion no valida.\n");
    }
  }

  destroyMap(wordMap);

  return 0;
}