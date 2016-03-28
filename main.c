//
//  main.c
//  EXR2.3
//
//  Created by Abraham Esses on 3/17/16.
//  Copyright © 2016 Abraham Esses. All rights reserved.
//
//Crea un programa concurrente que permita simular las operaciones de un banco (utiliza los mecanismos de sincronización vistos en clase)
//
//Hay 8 cajeros para atención de clientes / usuarios
//OK... 5 cajeros atienden operaciones generales. El tiempo promedio de atención es entre 3 y 5 minutos.
//OK... 1/2 3 cajeros atienden operaciones empresariales. Este tipo de cajeros, cuando no hay ningún cliente empresarial, pueden atender clientes con operaciones generales.
//OK... Todos los cajeros toman un receso de 3 minutos cada que atienden 5 clientes
//Cuando un cajero está atendiendo a un usuario se imprime
//Id del cajero
//OK... Id del cliente
//OK... Tipo de operación realizada (general/empresarial)
//Cuando un cajero está descansando se imprime un mensaje para indicar al cliente que detiene sus operaciones / actividades OK...
//Simula la llegada de 150 usuarios
//100 usuarios van a realizar operaciones generales. El tiempo de llegada de estos usuarios es entre 50 y 220 segundos OK ...
//50 usuarios desean realizar operaciones empresariales. El tiempo promedio de llegada de estos usuarios es entre 90 y 340 segundos OK...
//Cada que llega un cliente se forma en una fila de espera e imprime lo siguiente:
//Identificador del cliente OK...
//Tipo de operación a realizar (empresarial o general) OK...

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#define N 150

sem_t cajeros_gen;
sem_t cajeros_emp;
void * operacion_emp(void*);
void * operacion_gen(void*);
pthread_mutex_t mutex;
int num_op_gen = 0;
int num_op_emp = 0;
typedef struct{
    int id;
    int tipo_op;
}cliente;
void * operacion_emp(void * p){
    pthread_mutex_lock(&mutex);
    int temp = num_op_emp;
    pthread_mutex_unlock(&mutex);
    int t1,t2,valor_sem;
    t1 = (rand() % (340+1-90))+90;
    printf("Esperando %d segundos\n",t1);
    sleep(t1);
    if (temp <= 5)
    {
        sem_wait(&cajeros_emp);
        sem_getvalue(&cajeros_emp, &valor_sem);
        printf("Cajero %d : Atendiendo al cliente %d,con operación %d\n",valor_sem,((cliente*)p)->id,((cliente *)p)->tipo_op);
        t2=(rand() % (5+1-3))+3;
        printf("Generando operación por %d segundos\n",t2);
        sleep(t2);
        pthread_mutex_lock(&mutex);
        num_op_emp = temp++;
        pthread_mutex_unlock(&mutex);
        sem_post(&cajeros_emp);
    }
    else{
        int a;
        sem_getvalue(&cajeros_emp, &a);
        printf("Cajero %d descansando...\n",a);
        sleep(3);
        pthread_mutex_lock(&mutex);
        num_op_emp = 0;
        pthread_mutex_lock(&mutex);
    }
    pthread_exit(NULL);
}
void * operacion_gen(void * p){
    
    pthread_mutex_lock(&mutex);
    int temp = num_op_gen;
    pthread_mutex_unlock(&mutex);
    int t3,t4,valor_sem;
    t3 = (rand() % (220+1-50))+50;
    
    printf("Esperando %d segundos\n",t3);
    sleep(t3);
    if (temp <= 5)
    {
        sem_wait(&cajeros_gen);
        sem_getvalue(&cajeros_gen, &valor_sem);
        printf("Cajero %d: Atendiendo al cliente %d,con operación %d\n",valor_sem,((cliente*)p)->id,((cliente *)p)->tipo_op);
        t4=(rand() % (5+1-3))+3;
        //printf("Generando operación por %d segundos\n",t4);
        sleep(t4);
        pthread_mutex_lock(&mutex);
        num_op_gen++;
        pthread_mutex_unlock(&mutex);
        sem_post(&cajeros_gen);
    }
    else{
        int a;
        sem_getvalue(&cajeros_gen, &a);
        printf("Cajero %d descansando...\n",a);
        sleep(3);
        pthread_mutex_lock(&mutex);
        num_op_gen = 0;
        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}
int main(int argc, const char * argv[]) {
    pthread_t * hilos = (pthread_t *)malloc(sizeof(pthread_t)*N);
    cliente * params;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&cajeros_gen, 0, 5);
    sem_init(&cajeros_emp, 0, 3);
    int op_gen = 0;
    int op_emp = 0;
    for (int i = 0; i < N; ++i) {
        params = malloc(sizeof(cliente));
        (*params).id = i;
        (*params).tipo_op = (rand() % (2+1-1))+1;
        
        switch ((*params).tipo_op) {
            case 1:
                if (op_gen < 100) {
                    printf("+++ Creando el hilo con id %d y tipo de operación general (%d)...\n",(*params).id, (*params).tipo_op);
                    pthread_create((hilos+i),NULL, operacion_gen,  (void *) params);
                    op_gen++;
                }
                break;
            case 2:
                if (op_emp < 50) {
                printf("+++ Creando el hilo con id %d y tipo de operación empresarial (%d)...\n",(*params).id, (*params).tipo_op);
                pthread_create((hilos+i),NULL, operacion_emp,  (void *)params);
                op_emp++;
                }
                break;
        }
    }
    for (int i = 0; i < N; ++i) {
        
        pthread_join(*(hilos+i), NULL);
        
        printf("--- Terminando el hilo %d ...\n", i);
    }
    sem_destroy(&cajeros_emp);
    sem_destroy(&cajeros_gen);
    free(hilos);
    return 0;
}