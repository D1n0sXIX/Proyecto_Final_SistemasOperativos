/*
  Proyecto Final de Sistemas Operativos
  Hugo Olza
  Alejandro Mamán
  INSO 2B
*/


// Librerias
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

// Defines
#define LONGITUD_COMANDO 100

// Declaracion de funciones
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);
int SelectorDeComando(const char *orden); // Funcion EXTRA, facilita la introduccion de una comando


// Main
int main(){
    // IMPORTANTE MODIFICACCION: He quitado los * a estos arrays, me daba error a la hora de copilar
	 char comando[LONGITUD_COMANDO];
	 char orden[LONGITUD_COMANDO];
	 char argumento1[LONGITUD_COMANDO];
	 char argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int entradadir;
     int grabardatos;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     //...
     
     fent = fopen("particion.bin","r+b"); // Leemos el fichero que usamos como disco
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     /*copiamos bloques de memoria desde el arreglo datosfich hacia las estructuras correspondientes que representan diferentes
     partes de un sistema de archivos simulado.
     Cada línea tiene un proposito, relacionado con la organización y el contenido del archivo binario particion.bin.*/
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(directorio, (EXT_ENTRADA_DIR *)&datosfich[3], sizeof(directorio));
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Bucle de tratamiento de comandos -> Bucle principal
     for (;;){
		 do {
		 printf ("\n>> "); // Prompt del usuario
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin); // Aqui leemos el comando
         comando[strcspn(comando, "\n")] = 0; // Eliminamos el salto de linea 

		 }while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);

        // Usamos un switch para manejar comandos // AÑADIDO AQUI
         int input = SelectorDeComando(orden); // AÑADIDO AQUI

         switch (input) { // AÑADIDO AQUI
             case 0: // dir
                 Directorio(directorio, &ext_blq_inodos);
                 break;
             case 1: // rename
                 Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
                 break;
             case 2: // copy
                 Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
                 break;
             case 3: // remove
                 Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
                 break;
             case 4: // info
                 LeeSuperBloque(&ext_superblock);
                 break;
             case 5: // imprimir
                 Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
                 break;
             case 6: // bytemaps
                 Printbytemaps(&ext_bytemaps);
                 break;
             case 7: // exit
                 GrabarDatos(memdatos, fent);
                 fclose(fent);
                 return 0;

            case 8: // help
                printf("Lista de comandos:");
                printf("\n- dir: Lista todos los ficheros");
                printf("\n- rename: Cambia el nombre de un fichero en la entrada correspondiente");
                printf("\n- copy: Permite copiar un fichero");
                printf("\n- remove: Permite eliminar un fichero");
                printf("\n- info: Muestra informaccion del Superbloque");
                printf("\n- imprimir: Muestra el contenido de un fichero");
                printf("\n- exit: Sales del programa");
                break;
             default: // Por si se introduce un comando no valido
                 printf("ERROR, Este comando no es valido\n");
                 break;
         }

         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(memdatos,fent);
            fclose(fent);
            return 0;
         }
         
     }
}

// ESTO ES LO PRIMERO CON LO QUE NOS TENEMOS QUE PONER!!!
//Definicion de funciones:

// Funcion que convierte un comando en un indice comprobando si es correcto
int SelectorDeComando(const char *orden) {
    // Creamos una lista de comandos
    const char *comandos[9] = {"dir", "rename", "copy", "remove", "info", "imprimir", "bytemaps", "exit", "help"};
    // Y vamos comparandolos del 1 al 8
    for (int i = 0; i < 9; i++) {
        // Si la orden introducida coincide con el comando se devuleve esa posicion del comando
        // La cual es la que determina el tipo de orden orden
        if (strcmp(orden, comandos[i]) == 0) {
            return i;
        }
    }
    // Si no se encuentra devolvemos -1, como comando no reconocido
    return -1;
}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){/*Muestra el contenido del bytemap de inodos y los 25 primeros elementos del bytemap
                                                de bloques. Este comando te resultará muy útil durante el desarrollo.*/
    int i;
    printf("Inodos :");
    for(i=0;i<MAX_INODOS;i++){
        printf("%d", ext_bytemaps -> bmap_inodos[i]);
    }
    printf("\n");
    printf("Bloques [0-25] :");
    for(i=0;i<25;i++){
        printf("%d", ext_bytemaps -> bmap_bloques[i]);
    }
    printf("\n");
}
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
    /* Debemos de inicializar los argumentos como cadenas vacias, esto es para poder tomar varios argumentos por separado
    ya que si solo tomamos 1 a la hora de ejecutar comandos como info, remove o copy, no seran comandos validos*/
    sprintf(argumento1, "%s", "");
    sprintf(argumento2, "%s", "");
    sprintf(orden, "%s", "");

    // Dividimos la entrada en orden y argumentos
    sscanf(strcomando, "%s %s %s", orden, argumento1, argumento2);

    // Convertimos el comando principal a minúsculas para validarlo
    for (int i = 0; orden[i]; i++) {
        orden[i] = tolower(orden[i]);
    }

    // Verificamos si el comando principal es valido
    const char *comandos_validos[9] = {"dir", "rename", "copy", "remove", "info", "imprimir", "bytemaps", "exit", "help"};
    for (int i = 0; i < 9; i++) {
        if (strcmp(orden, comandos_validos[i]) == 0) {
            return 0;
        }
    }
    printf("ERROR\n");
    return -1;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){
    printf("\nInformaccion del SuperBloque:");
    printf("\n- Tamaño del Super Bloque = %d Bytes", psup -> s_block_size);
    printf("\n- Numero de Total de inodos = %d", psup -> s_inodes_count);
    printf("\n- Numero de inodos Libres = %d", psup -> s_free_blocks_count);
    printf("\n- Numero Total de Bloques = %d", psup -> s_blocks_count);
    printf("\n- Numero de Bloques Libres = %d", psup -> s_free_blocks_count);
    printf("\n- Primer bloque de Datos = %d Bytes", psup -> s_first_data_block);
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){
    int i;
    for (i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo == NULL_INODO) {
            continue;
        }
        if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
            return i;
        }
    }
    printf("\nEl archivo %s no encontrado", nombre);
    return -1;
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){
    int i;
    for(i=0; i< MAX_FICHEROS; i++){
        // Añadimos una condifccion que nos permita evitar entradas vacias y la entrada raiz
        if (directorio[i].dir_inodo == NULL_INODO || strcmp(directorio[i].dir_nfich, ".") == 0) {
            continue;
        }
        // MOstramos informaccion del directorio en cuestion
        printf("\nNombre: %s    Inodo: %d    Tamaño: %d Bloques:", directorio[i].dir_nfich, directorio[i].dir_inodo, inodos->blq_inodos[directorio[i].dir_inodo].size_fichero);
    }
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
    // Verificar si el nombre nuevo ya existe
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO && strcmp(directorio[i].dir_nfich, nombrenuevo) == 0) {
            printf("ERROR: Ya existe un archivo con el nombre '%s'.\n", nombrenuevo);
            return -1;
        }
    }

    // Buscar el archivo con el nombre antiguo
    int index = BuscaFich(directorio, inodos, nombreantiguo);
    if (index == -1) {
        printf("ERROR: Archivo '%s' no encontrado.\n", nombreantiguo);
        return -1;
    }

    // Validar longitud del nuevo nombre
    if (strlen(nombrenuevo) >= LEN_NFICH) {
        printf("ERROR: El nombre '%s' es demasiado largo (máximo %d caracteres).\n", nombrenuevo, LEN_NFICH - 1);
        return -1;
    }

    // Renombrar el archivo
    strncpy(directorio[index].dir_nfich, nombrenuevo, LEN_NFICH - 1);
    directorio[index].dir_nfich[LEN_NFICH - 1] = '\0'; // Asegurar terminación de cadena
    printf("Archivo '%s' renombrado a '%s'.\n", nombreantiguo, nombrenuevo);

    return 0; // Éxito
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
    // Buscar el archivo en el directorio
    int nombreFichero = BuscaFich(directorio, inodos, nombre);
    unsigned short int inodoNum = directorio[nombreFichero].dir_inodo;
    EXT_SIMPLE_INODE *inodo = &inodos->blq_inodos[inodoNum];

    // Comprobacciones
    
    if (nombreFichero == -1) {
        return -1; // Error ya manejado en BuscaFich
    }

    // Obtener el inodo correspondiente
    if (inodoNum >= MAX_INODOS) {
        printf("ERROR: Inodo fuera de rango para el archivo '%s'.\n", nombre);
        return -1;
    }


    // Verificar si el archivo tiene contenido
    if (inodo->size_fichero == 0) {
        printf("El archivo '%s' esta vacio.\n", nombre);
        return 0;
    }

    // Mostrar contenido del archivo
    printf("Contenido del archivo '%s':\n", nombre);
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        unsigned short int bloque_num = inodo->i_nbloque[i];
        if (bloque_num == NULL_BLOQUE) {
            break; // Fin de bloques asignados
        }

        // Validar el rango del bloque
        if (bloque_num >= MAX_BLOQUES_DATOS) {
            printf("ERROR: Bloque %d fuera de rango.\n", bloque_num);
            break;
        }

        // Mostrar el contenido del bloque
        fwrite(memdatos[bloque_num].dato, 1, SIZE_BLOQUE, stdout);
    }
    printf("\n");

    return 0;
}


int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich){
      
}
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich){

}
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich){

}
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich){

}
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich){

}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich){

}

