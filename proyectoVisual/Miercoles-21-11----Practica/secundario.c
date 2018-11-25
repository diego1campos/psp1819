#include "libComun.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <regex.h>
#include <time.h>//time

void rellenarFechaYHora(char *);

int main(int argc, char const *argv[]){
    if ( argc != 2 ){
        fprintf( stderr, "Error: debe proporcionar el nombre del FIFO y nada más\n" );
        return -1;
    }
    printf("Bienvenido a Cal by Diego Campos.\nRESULTADO = NÚMERO A (OPERACIÓN) NÚMERO B\nPrograma secundario\n");
    //Elemento B:
    while( getNum( 'B' ) );
    printf("Esperando datos del programa principal...\n");
    int desLectura = open( argv[1], O_RDONLY );
    double numEnviado;
    /*int readTub = */read( desLectura, &numEnviado, sizeof( numEnviado ) );
    read( desLectura, operacion, sizeof( char ) );
    close( desLectura );
    printf("Datos leidos con éxito. Tuberia cerrada con éxito\n");
    //Lectura finalizada :D
    //printf("%s, %lf", operacion, numDouble);
    int tub[2];//recuperar resultado
    pipe( tub );//Iniciar pipe
    pid_t pid = fork();
    if ( pid == -1 ) fprintf( stderr, "error fork hijo" );
    else if ( pid == 0 ){//Hijo!! :D
        close ( tub[0] );//ya que yo voy a escribir en tub :D
        //Ejecutar operacion
        double resHijo;
        switch( operacion[0] ){
            case '+' :
                resHijo = numEnviado + numDouble;
                break;
            case '-' :
                resHijo = numEnviado - numDouble;
                break;
            case '*' :
                resHijo = numEnviado * numDouble;
                break;
            case '/' :
                resHijo = numEnviado / numDouble;
                break;
            default:
                fprintf( stderr, "Error: default xd\n");
        }
        write( tub[1], &resHijo, sizeof(double) );//se le envia un puntero y el sizeof para saber cuanto debe de ESCRIBIR a partir de esa direccion de memoria :D
        close( tub[1] );//cierro extremo escritura
        //return 0;//mato hijo xd
    }
    else{//Padre!!! :D
        close( tub[1] );//Ya que solo voy a leer :D
        double resPadre;
        read( tub[0], &resPadre, sizeof(double) );//se le envia un puntero y el sizeof para saber cuanto debe de LEER a partir de esa direccion de memoria :D
        close( tub[0] );//cierro extremo lectura
        printf("Abriendo tuberia\n");
        int descriptorEscritura = open( argv[1], O_WRONLY );
        if ( descriptorEscritura == -1 ){
            fprintf( stderr, "File : "__FILE__" Error al abrir tuberia : %s : Line %d\n", strerror( errno ), __LINE__ );
            return -1;
        }
        write( descriptorEscritura, &resPadre, sizeof( resPadre ) );
        close( descriptorEscritura );//cierro tuberia
        printf("Datos enviados con éxito. Tuberia cerrada con éxito\n");
        printf("Escribriendo en log.txt...\n");
        //procedo a escribir en log.txt
        FILE* fEscritura = fopen( "./log.txt", "a+" );
        if ( fEscritura == NULL ){
            fprintf( stderr, "File : "__FILE__" Error al abrir log.txt : %s : Line %d\n", strerror( errno ), __LINE__ );
            return -1;
        }
        //fuente de tamaño double-.-->http://decsai.ugr.es/~jfv/ed1/c/cdrom/cap2/cap24.htm
        //size of reForLog... double	64	1.7E-308 a 1.7E+308... esto equivale a 17 MÁS 307 CEROS... no? :)
        int size2Doubles = ( 2 + 307 ) * 2;//1.7 que pasa a ser 1 7 y 307 ceros * 2 xd
        char fechaHora[TAM_MAX];
        rellenarFechaYHora( fechaHora );
        char resForLog[ size2Doubles + strlen( fechaHora ) ];//"genial" xd
        sprintf( resForLog, "%s %lf %c %lf = %lf\n", fechaHora, numEnviado, operacion[0], numDouble, resPadre );
        int errorEscritura = fputs( resForLog, fEscritura );
        if ( errorEscritura <= 0 ){
            fprintf( stderr, "File : "__FILE__" Error al intentar escribir en log.txt: Line %d\n", __LINE__ );
        }
        fclose( fEscritura );//cierro fichero luego de escribir xd
        printf("Escritura realizada con éxito!!\n");
        //terminando de escribir en log.txt :D!!!
        printf("Fin del programa. Chaoo!!\n");
    }
    return 0;
}

void rellenarFechaYHora(char *cadena){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(cadena ,"%2d/%2d/%4d (%d:%d:%d) -", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
}