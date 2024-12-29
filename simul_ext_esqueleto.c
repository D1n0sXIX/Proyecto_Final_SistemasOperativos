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
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre, EXT_DATOS *memdatos); // añadido un parametro --> memdatos
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);
int SelectorDeComando(const char *orden);  // Funcion EXTRA, facilita la introduccion de una comando
void printMemDatos(EXT_DATOS *memdatos); // Funcion EXTRA, ayuda para comprobar que se ha eliminaod el contenido de los bloques

// Main
int main(){
	 char comando[LONGITUD_COMANDO]; // Array que almacenara el comando introducido
	 char orden[LONGITUD_COMANDO]; // Array que determinara  los tipos de ordenes que son accesibles
	 char argumento1[LONGITUD_COMANDO]; // Array que almacena el primer argumento de una orden
	 char argumento2[LONGITUD_COMANDO]; // Array que lamacena el segundo argumento de una orden
	 
     EXT_SIMPLE_SUPERBLOCK ext_superblock;
     EXT_BYTE_MAPS ext_bytemaps;
     EXT_BLQ_INODOS ext_blq_inodos;
     EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     int grabardatos = 0;
     FILE *fent;
     
     // Lectura del fichero completo de una sola vez
     fent = fopen("particion.bin","r+b"); // Leemos el fichero que usamos como disco
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     /*Copiamos bloques de memoria desde el arreglo datosfich hacia las estructuras correspondientes que representan diferentes
     partes de un sistema de archivos simulado
     Cada línea tiene un proposito, relacionado con la organización y el contenido del archivo binario 'particion.bin'*/
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

        // Usamos un switch para manejar comandos
         int input = SelectorDeComando(orden);

         switch (input) { // AÑADIDO AQUI
             case 0: // dir
                 Directorio(directorio, &ext_blq_inodos);
                 break;
             case 1: // rename
                 Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
                 grabardatos = 1;
                 break;
             case 2: // copy
                 Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
                 grabardatos = 1;

                 break;
             case 3: // remove
                 Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, memdatos);
                 grabardatos = 1;

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
                 fclose(fent);
                 return 0;

            case 9: // debugger
                printMemDatos(memdatos);
                break;
            case 8: // help
                printf("Lista de comandos:");
                printf("\n- dir: Lista todos los ficheros");
                printf("\n- rename: Cambia el nombre de un fichero en la entrada correspondiente");
                printf("\n- copy: Permite copiar un fichero");
                printf("\n- remove: Permite eliminar un fichero");
                printf("\n- info: Muestra informaccion del Superbloque");
                printf("\n- imprimir: Muestra el contenido de un fichero");
                printf("\n- bytemaps: Muestra el contenido del bytemap de inodos y los 25 primeros elementos del bytemap de bloques");
                printf("\n- debugger: Muestra el contenido de todos los bloques para ver si realmente se han eliminado los datos");
                printf("\n- exit: Sales del programa");
                break;
             default: // Por si se introduce un comando no valido
                 printf("ERROR --> comando no valido\nUsa 'help' para más informacion");
                 break;
         }

        // Como solo modificamos la variable grabarDatos con  rename, copy y remove, si esta ha sido modificada --> grabamos los datos
        if (grabardatos) {
            Grabarinodosydirectorio(directorio, &ext_blq_inodos, fent);
            GrabarByteMaps(&ext_bytemaps, fent);
            GrabarSuperBloque(&ext_superblock, fent);
            GrabarDatos(memdatos, fent);
            grabardatos = 0; // Reiniciamos la condicion
        }
     }
}

//Definicion de funciones:
// Funcion que comprueba si el input introducido por el usuario es correcto
int SelectorDeComando(const char *orden) {
    // Creamos una lista de comandos
    const char *comandos[10] = {"dir", "rename", "copy", "remove", "info", "imprimir", "bytemaps", "exit", "help", "debugger"};
    // Y vamos comparandolos del 1 al 8
    for (int i = 0; i < 10; i++) {
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
                                                de bloques*/
    int i;
    printf("Inodos :");
    for(i=0;i<MAX_INODOS;i++){
        printf("%d ", ext_bytemaps -> bmap_inodos[i]);
    }
    printf("\n");
    printf("Bloques [0-25] :");
    for(i=0;i<25;i++){
        printf("%d ", ext_bytemaps -> bmap_bloques[i]);
    }
    printf("\n");
}

// Funcionq ue comprueba que el comando es valido dividiendo en argumentos el input
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {
    /* Debemos de inicializar los argumentos como cadenas vacias, esto es para poder tomar varios argumentos por separado
    ya que si solo tomamos 1 a la hora de ejecutar comandos como info, remove o copy, no seran comandos validos*/
    sprintf(argumento1, "%s", "");
    sprintf(argumento2, "%s", "");
    sprintf(orden, "%s", "");

    // Dividimos la entrada en orden y argumentos
    sscanf(strcomando, "%s %s %s", orden, argumento1, argumento2);

    // Convertimos el comando principal a minusculas para validarlo --> un poco extra
    for (int i = 0; orden[i]; i++) {
        orden[i] = tolower(orden[i]);
    }

    // Verificamos si el comando principal es valido
    const char *comandos_validos[10] = {"dir", "rename", "copy", "remove", "info", "imprimir", "bytemaps", "exit", "help", "debugger"};
    for (int i = 0; i < 10; i++) {
        if (strcmp(orden, comandos_validos[i]) == 0) {
            return 0;
        }
    }
    printf("ERROR, comando no reconocido\n");
    return -1;
}



void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){
    printf("\nInformaccion del SuperBloque:");
    printf("\n- Tamaño del Super Bloque = %d Bytes", psup -> s_block_size);
    printf("\n- Numero de Total de inodos = %d", psup -> s_inodes_count);
    printf("\n- Numero de inodos Libres = %d", psup -> s_free_inodes_count);
    printf("\n- Numero Total de Bloques = %d", psup -> s_blocks_count);
    printf("\n- Numero de Bloques Libres = %d", psup -> s_free_blocks_count);
    printf("\n- Primer bloque de Datos = %d Bytes", psup -> s_first_data_block);
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre) {
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo == NULL_INODO) {
            continue;
        }
        if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
            return i;
        }
    }
    printf("\nArchivo %s no encontrado", nombre);
    return -1;
}


void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
    printf("--- Listado de archivos ---\n");
    for (int i = 0; i < MAX_FICHEROS; i++) {
        // Saltar entradas vacías o la entrada especial '.'
        if (directorio[i].dir_inodo == NULL_INODO || strcmp(directorio[i].dir_nfich, ".") == 0) {
            continue;
        }

        unsigned short int inodoNum = directorio[i].dir_inodo;
        if (inodoNum >= MAX_INODOS) {
            printf("ERROR: Inodo fuera de rango en la entrada %d.\n", i);
            continue;
        }

        EXT_SIMPLE_INODE *inodo = &inodos->blq_inodos[inodoNum];
        printf("Nombre: %s | Tamaño: %d bytes | Inodo: %d | Bloques: ", directorio[i].dir_nfich, inodo->size_fichero, inodoNum);

        // Listar bloques asignados al archivo
        for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
            if (inodo->i_nbloque[j] == NULL_BLOQUE) break;
            printf("%d ", inodo->i_nbloque[j]);
        }
        printf("\n");
    }
    printf("---------------------------\n");
}



int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO && strcmp(directorio[i].dir_nfich, nombrenuevo) == 0) {
            printf("ERROR --> Ya existe un archivo con el nombre '%s'\n", nombrenuevo);
            return -1;
        }
    }

    int index = BuscaFich(directorio, inodos, nombreantiguo);
    if (index == -1 || strlen(nombrenuevo) >= LEN_NFICH) {
        return -1;
    }

    strncpy(directorio[index].dir_nfich, nombrenuevo, LEN_NFICH - 1);
    directorio[index].dir_nfich[LEN_NFICH - 1] = '\0';
    printf("Archivo --> '%s'\nRenombrado --> '%s'.\n", nombreantiguo, nombrenuevo);
    return 0;
}


int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
    // Usar la función BuscaFich para localizar el archivo
    int idArchivo = BuscaFich(directorio, inodos, nombre);
    if (idArchivo == -1) {
        printf("ERROR --> Archivo '%s' no encontrado\n", nombre);
        return -1;
    }

    // Obtener el inodo correspondiente al archivo
    unsigned short int inodo_id = directorio[idArchivo].dir_inodo;
    EXT_SIMPLE_INODE *inodo = &inodos->blq_inodos[inodo_id];

    printf("--> '%s':\n", nombre);

    // Iterar por los bloques asignados al archivo
    for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
        if (inodo->i_nbloque[j] == NULL_BLOQUE) break;
        // Imprimir el contenido del bloque directamente como una cadena
        printf("%s", memdatos[inodo->i_nbloque[j]-4].dato);
    }
    printf("\n");
    return 0;
}


int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre, EXT_DATOS *memdatos) {
    // Buscar el archivo a eliminar
    int indexFichero = BuscaFich(directorio, inodos, nombre);
    if (indexFichero == -1) {
        printf("ERROR --> El archivo '%s' no existe\n", nombre);
        return -1;
    }

    // Obtenemos el numero de inodo del archivo
    unsigned short int inodoNum = directorio[indexFichero].dir_inodo;
    if (inodoNum >= MAX_INODOS) {
        printf("ERROR --> Inodo fuera de rango --> '%s'\n", nombre);
        return -1;
    }

    // Referencia al inodo del archivo
    EXT_SIMPLE_INODE *inodo = &inodos->blq_inodos[inodoNum];

    // Liberamos los bloques ocupados por el archivo en el bytemap y limpiamos su contenido
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        unsigned short int bloque = inodo->i_nbloque[i];
        if (bloque == NULL_BLOQUE){
            break;
        }

        // Ajustamos el indice del bloque a la estructura --> memdatos
        unsigned short int bloqueMem = bloque - 4;

        if (bloqueMem >= 0 && bloqueMem < MAX_BLOQUES_DATOS) {
            // Limpiamos el contenido del bloque en memdatos
            memset(memdatos[bloqueMem].dato, 0, SIZE_BLOQUE);

            // Para depuraccion
            //printf("Bloque %d limpiado\n", bloqueMem);
        }

        // Liberamos el bloque en el mapa de bits
        ext_bytemaps->bmap_bloques[bloque] = 0; 
        ext_superblock->s_free_blocks_count++; // Incrementamos contador de bloques libres
    }

    // Limpiamos el inodo --> reset
    inodo->size_fichero = 0;
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        inodo->i_nbloque[i] = NULL_BLOQUE;
    }
    ext_bytemaps->bmap_inodos[inodoNum] = 0; // Marcamos el inodo como libre
    ext_superblock->s_free_inodes_count++; // Incrementamos contador de inodos libres

    // Limpiamos la entrada del directorio
    memset(directorio[indexFichero].dir_nfich, '\0', LEN_NFICH);
    directorio[indexFichero].dir_inodo = NULL_INODO;

    printf("--> '%s' eliminado\n", nombre);
    return 0;
}



int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
    // Buscamos el archivo origen con la funcion BuscaFich
    int origen = BuscaFich(directorio, inodos, nombreorigen);
    if (origen == -1) {
        printf("ERROR --> Archivo '%s' no encontrado\n", nombreorigen);
        return -1;
    }

    // Verificamos si el archivo destino ya existe
    if (BuscaFich(directorio, inodos, nombredestino) != -1) {
        printf("ERROR --> El archivo destino ya existe\n");
        return -1;
    }

    // Buscamos el primer inodo libre
    int inodoLibre = -1;
    for (int i = 0; i < MAX_INODOS; i++) {
        if (ext_bytemaps->bmap_inodos[i] == 0) { // Inodo libre
            inodoLibre = i;
            break;
        }
    }
    if (inodoLibre == -1) {
        printf("ERROR --> No quedan inodos libres\n");
        return -1;
    }

    // Encontramos la primera entrada libre en el directorio
    int entradaLibre = -1;
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo == NULL_INODO) { // Entrada vacia
            entradaLibre = i;
            break;
        }
    }
    if (entradaLibre == -1) {
        printf("ERROR --> No hay espacio en el directorio\n");
        return -1;
    }

    // Referenciamos al inodo origen y al nuevo inodo
    EXT_SIMPLE_INODE *inodoOrigen = &inodos->blq_inodos[directorio[origen].dir_inodo];
    EXT_SIMPLE_INODE *inodoNuevo = &inodos->blq_inodos[inodoLibre];

    // Copiamos el tamaño del archivo y reseteamos el inodo nuevo
    inodoNuevo->size_fichero = inodoOrigen->size_fichero;
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        inodoNuevo->i_nbloque[i] = NULL_BLOQUE;
    }

    // Copiamos los datos del archivo origen a bloques libres
    int bloquesAsignados = 0;
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        unsigned short int bloqueOrigen = inodoOrigen->i_nbloque[i];
        if (bloqueOrigen == NULL_BLOQUE) break;

        // Buscamos el primer bloque libre
        int nuevoBloque = -1;
        for (int j = PRIM_BLOQUE_DATOS; j < MAX_BLOQUES_PARTICION; j++) {
            if (ext_bytemaps->bmap_bloques[j] == 0) { // Bloque libre
                nuevoBloque = j;
                ext_bytemaps->bmap_bloques[j] = 1; // Marcar bloque como ocupado
                break;
            }
        }
        if (nuevoBloque == -1) {
            printf("ERROR --> No hay bloques libres\n");
            return -1;
        }

        // Copiamos datos del bloque origen al nuevo bloque
        memcpy(memdatos[nuevoBloque - 4].dato, memdatos[bloqueOrigen - 4].dato, SIZE_BLOQUE);
        inodoNuevo->i_nbloque[bloquesAsignados++] = nuevoBloque;
    }

    // Ponemos el inodo nuevo como ocupado
    ext_bytemaps->bmap_inodos[inodoLibre] = 1;

    // Crear una nueva entrada en el directorio
    strncpy(directorio[entradaLibre].dir_nfich, nombredestino, LEN_NFICH - 1);
    directorio[entradaLibre].dir_nfich[LEN_NFICH - 1] = '\0';
    directorio[entradaLibre].dir_inodo = inodoLibre;

    // Actualizamos el superbloque
    ext_superblock->s_free_inodes_count--;
    ext_superblock->s_free_blocks_count -= bloquesAsignados;

    printf("\nArchivo '%s' copiado como '%s'.\n", nombreorigen, nombredestino);
    return 0;
}



void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
    // Escribir los inodos en el archivo
    fseek(fich, SIZE_BLOQUE * 2, SEEK_SET); // El bloque 2 contiene la lista de inodos
    fwrite(inodos, sizeof(EXT_BLQ_INODOS), 1, fich);

    // Escribir el directorio en el archivo
    fseek(fich, SIZE_BLOQUE * 3, SEEK_SET); // El bloque 3 contiene el directorio
    fwrite(directorio, sizeof(EXT_ENTRADA_DIR) * MAX_FICHEROS, 1, fich);
    // Depuraccion
    //printf("Inodos y directorio grabados correctamente\n");
}
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
    // Escribir los bytemaps en el bloque 1
    fseek(fich, SIZE_BLOQUE * 1, SEEK_SET); // El bloque 1 contiene los bytemaps
    fwrite(ext_bytemaps, sizeof(EXT_BYTE_MAPS), 1, fich);
    // Depuraccion
    //printf("ByteMaps grabados correctamente.\n");
}
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
    // Escribir el superbloque en el bloque 0
    fseek(fich, 0, SEEK_SET); // El bloque 0 contiene el superbloque
    fwrite(ext_superblock, sizeof(EXT_SIMPLE_SUPERBLOCK), 1, fich);
    // Depuraccion
    //printf("SuperBloque grabado correctamente\n");
}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
    // Escribir los datos a partir del bloque 4
    fseek(fich, SIZE_BLOQUE * PRIM_BLOQUE_DATOS, SEEK_SET); // Los bloques de datos empiezan en el bloque 4
    fwrite(memdatos, sizeof(EXT_DATOS), MAX_BLOQUES_DATOS, fich);
    // Depuraccion
    //printf("Datos grabados correctamente\n");
}

// Funcion para poder debuggear
void printMemDatos(EXT_DATOS *memdatos) {
  for (int i = 0; i < MAX_BLOQUES_DATOS; i++) {
    printf("Bloque de datos %d\n", i);
    for (int j = 0; j < SIZE_BLOQUE; j++) {
      printf("%c", memdatos[i].dato[j]);
    }
    printf("\n");
  }
}