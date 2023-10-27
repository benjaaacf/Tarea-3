#include <stdio.h>
#include <string.h>
#include <dirent.h> // Para trabajar con directorios
#include "list.h" // Supongo que aquí tienes definida la estructura DocumentList

// Función para cargar documentos desde un archivo .txt
void cargarDocumentoDesdeArchivo(DocumentList *docList, const char *filename) {
    // Abre el archivo para lectura
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error al abrir el archivo: %s\n", filename);
        return;
    }

    // Inicializa una estructura Document para almacenar los datos del documento
    Document doc;
    doc.id = docList->count; // Usa el índice actual como ID
    doc.content[0] = '\0'; // Inicializa el contenido

    // Lee el contenido del archivo línea por línea
    char line[5000]; // Supongo que cada línea tiene un máximo de 5000 caracteres
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "Title:", 6) == 0) {
            // Si la línea comienza con "Title:", se guarda como título del documento
            strcpy(doc.title, line + 6);
            // Elimina el salto de línea del título
            char *newline = strchr(doc.title, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }
        } else if (strncmp(line, "*** START OF", 12) == 0) {
            // Final del encabezado, el resto del archivo se considera contenido
            break;
        }
    }

    // Lee el contenido del archivo y lo almacena en doc.content
    while (fgets(line, sizeof(line), file) != NULL) {
        strcat(doc.content, line);
    }

    // Agrega el documento a la lista
    docList->documents[docList->count] = doc;
    docList->count++;

    // Cierra el archivo
    fclose(file);
}

// Función para cargar documentos desde un directorio
void cargarDocumentosDesdeDirectorio(DocumentList *docList, const char *directoryPath) {
    DIR *dir;
    struct dirent *entry;

    // Abre el directorio
    dir = opendir(directoryPath);
    if (dir == NULL) {
        fprintf(stderr, "Error al abrir el directorio: %s\n", directoryPath);
        return;
    }

    // Lee los archivos del directorio
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            // Si es un archivo regular y tiene extensión .txt, carga el documento
            char filePath[256]; // Supongo un máximo de 256 caracteres para la ruta
            snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
            cargarDocumentoDesdeArchivo(docList, filePath);
        }
    }

    // Cierra el directorio
    closedir(dir);
}

// Función para cargar documentos desde archivos .txt o desde una carpeta
void cargarDocumentos(DocumentList *docList, const char *documentNames) {
    char *token = strtok(documentNames, " ");
    while (token != NULL) {
        if (strstr(token, ".txt") != NULL) {
            // Si el nombre es un archivo .txt, cargarlo
            cargarDocumentoDesdeArchivo(docList, token);
        } else {
            // Si no, asumir que es una carpeta y cargar documentos desde ella
            cargarDocumentosDesdeDirectorio(docList, token);
        }
        token = strtok(NULL, " ");
    }
}


// Función de comparación para qsort
int compararDocumentos(const void *doc1, const void *doc2) {
    return strcmp(((Document *)doc1)->title, ((Document *)doc2)->title);
}

// Función para contar palabras en un documento
int contarPalabras(const char *content) {
    int count = 0;
    int inWord = 0; // 1 si estamos dentro de una palabra, 0 si no

    for (int i = 0; content[i] != '\0'; i++) {
        if (content[i] != ' ' && content[i] != '\n' && content[i] != '\t') {
            if (!inWord) {
                inWord = 1;
                count++;
            }
        } else {
            inWord = 0;
        }
    }

    return count;
}

// Función para mostrar documentos ordenados por título
void mostrarDocumentosOrdenados(DocumentList *docList) {
    // Ordena los documentos por título
    qsort(docList->documents, docList->count, sizeof(Document), compararDocumentos);

    // Muestra los documentos ordenados junto con la cantidad de palabras y caracteres
    for (int i = 0; i < docList->count; i++) {
        Document *doc = &docList->documents[i];
        int numWords = contarPalabras(doc->content);
        int numChars = strlen(doc->content);
        printf("ID: %d\n", doc->id);
        printf("Título: %s\n", doc->title);
        printf("Cantidad de palabras: %d\n", numWords);
        printf("Cantidad de caracteres: %d\n\n", numChars);
    }
}


int contieneTodasLasPalabras(const char *titulo, const char *palabras) {
    const char *palabra = strtok(palabras, " ");

    // Itera a través de las palabras de búsqueda
    while (palabra != NULL) {
        if (strstr(titulo, palabra) == NULL) {
            return 0; // El título no contiene una de las palabras
        }
        palabra = strtok(NULL, " ");
    }

    return 1; // El título contiene todas las palabras
}

// Función para buscar libros por título
void buscarLibroPorTitulo(DocumentList *docList, const char *keywords) {
    int encontrados = 0; // Contador de libros encontrados
    printf("Libros que contienen todas las palabras buscadas:\n");

    // Itera a través de los documentos
    for (int i = 0; i < docList->count; i++) {
        Document *doc = &docList->documents[i];
        if (contieneTodasLasPalabras(doc->title, keywords)) {
            printf("Título: %s\n", doc->title);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron libros que contengan todas las palabras buscadas.\n");
    }
}