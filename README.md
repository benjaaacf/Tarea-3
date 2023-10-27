Integrantes:
Benjamín Caballeria 21.608.700-3        
Sofía Meza 20.999.001-6
Fernanda Cadíz 21.561.209-0

Para ejecutar: ./tarea3
Para Compilar: gcc main.c list.c map.c -o tarea3


Funcionalidades
Funcionando correctamente:
1.Cargar documentos
2.Mostrar documentos ordenados por título
3.Buscar un libro por titulo
6.Buscar libros por palabra

Problemas conocidos:
1.Funcion "Palabras con mayor frecuencia": en esta funcion al ingresar algun caracter como el nombre del libro, "pasa de largo" y vuelve a abrir el menu, creemos que se debe a un error al buscar el titulo y relacionar la informacion.

2.Funcion "Palabas más relevantes": En esta funcion ocurre algo similar a lo anterior, al ingresar el nombre pasa nuevamente al menu, el error creemos que tambien sea por la informacion.

3.Funcion "Mostrar palabra en su contexto dentro del libro": Esta funcion acepta los parametros pero al momento de ingresar la palabra a buscar, no hace nada mas, creemos que se debe a que no prosesa la informacion que contiene el libro.


Contribuciones:
Benjamin Caballeria: Funcion cargar docs - main - Funcion buscar libros por palabra

Sofia Meza: Funcion mostrar docs ordenados - mapa.c - mapa.h - Funcion palabras con mayor frecuencia

Fernanda Cadiz: Funcion buscar libro por titulo - list.c - list.h - Funcion palabras mas relevantes

funcion mostrar palabra en su contexto: Todos


Puntajes:

Benjamin Caballeria: 3/3
Sofia Meza: 3/3
Fernnda Cadiz: 3/3