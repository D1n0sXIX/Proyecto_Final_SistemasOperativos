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

// Main
int main(){
	 char *comando[LONGITUD_COMANDO];
	 char *orden[LONGITUD_COMANDO];
	 char *argumento1[LONGITUD_COMANDO];
	 char *argumento2[LONGITUD_COMANDO];
	 
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
     
     fent = fopen("particion.bin","r+b"); // Leemos el ficheroq ue nos funciona de disco
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     /*copiamos bloques de memoria desde el arreglo datosfich hacia las estructuras correspondientes que representan diferentes
     partes de un sistema de archivos simulado.
     Cada línea tiene un proposito, relacionado con la organización y el contenido del archivo binario particion.bin.*/
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Bucle de tratamiento de comandos -> Bucle principal
     for (;;){
		 do {
		 printf (">> "); // Prompt del usuario
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin); // Aqui leemos el comando
		 }while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);

         // Debemos empezar por desarrollar la lectura de comandos
         // Estos ifs son como un swich muy grande
	     if (strcmp(orden,"dir")==0) {
            Directorio(&directorio,&ext_blq_inodos);
            continue;
            }
         //...
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
         
     }
}

// ESTO ES LO PRIMERO CON LO QUE NOS TENEMOS QUE PONER!!!
//Definicion de funciones:


void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){/*Muestra el contenido del bytemap de inodos y los 25 primeros elementos del bytemap
                                                de bloques. Este comando te resultará muy útil durante el desarrollo.*/

}
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){
    int i;
    sprintf(argumento1, "%s",""); //Inicializamos a vacío
    sprintf(argumento2, "%s","");
    sprintf(orden, "%s","");
    sscanf(strcomando, "%s %s %s", orden,argumento1, argumento2);
    for(i=0; orden[i]; i++){
        orden[i] = tolower(orden[i]);
    }
    // printf("%s %s %s\n",orden,argumento1,argumento2);
    if ((strcmp(orden,"dir")!=0) && (strcmp(orden,"rename")!=0) && (strcmp(orden,"copy")!=0)
    && (strcmp(orden,"remove")!=0) && (strcmp(orden,"info")!=0) && (strcmp(orden,"imprimir")!=0)
    && (strcmp(orden,"salir")!=0) && (strcmp(orden,"bytemaps")!=0)){
        printf("ERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,sali]\n");
        return -1;
    }
}
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup)
{
    printf("Bloque %d Bytes\n", psup -> s_block_size);
    printf("inodos particion = %d\n", psup -> s_inodes_count);
    printf("inodos particion = %d\n", psup -> s_free_block_count);
    printf("Bloques particion = %d\n", psup -> s_blocks_count);
    printf("Bloques libres = %d\n", psup -> s_free_blocks_count);
    printf("Primer bloque de datos = %d Bytes\n", psup -> s_first_data_block);
}
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){

}
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){

}
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){

}
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre){
   
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

