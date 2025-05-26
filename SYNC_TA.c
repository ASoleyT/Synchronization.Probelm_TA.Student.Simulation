#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


sem_t waiting_students;    
sem_t student_ready;        
sem_t ta_ready;             

pthread_mutex_t chair_mutex; 
int waiting = 0;




void* ta_function(void* arg) {
    while (1) {
        sem_wait(&waiting_students); 

        pthread_mutex_lock(&chair_mutex); 
        waiting--; 
        pthread_mutex_unlock(&chair_mutex);

        sem_post(&ta_ready);      
        sem_wait(&student_ready);

        printf("TA is helping a student.\n");
        sleep(2); 
        printf("TA finished helping a student.\n");
    }
}

void* student_function(void* arg) {
    int id = *((int*) arg);
    while (1) {
        sleep(rand() % 5); 

        pthread_mutex_lock(&chair_mutex);
        if (waiting < 5) {
            waiting++;
            printf("Student %d is waiting. (waiting = %d)\n", id, waiting);
            sem_post(&waiting_students); 
            pthread_mutex_unlock(&chair_mutex);

            sem_wait(&ta_ready);      
            sem_post(&student_ready);   
            printf("Student %d is getting help.\n", id);
        } else {
            pthread_mutex_unlock(&chair_mutex);
            printf("Student %d left (no empty chair).\n", id);
        }
    }
    return NULL;
}

int main() {

    //// this is for simulation 

    pthread_t ta;
    pthread_t students[10];
    int ids[10];

    srand(time(NULL));
    sem_init(&waiting_students, 0, 0);
    sem_init(&student_ready, 0, 0);
    sem_init(&ta_ready, 0, 0);
    pthread_mutex_init(&chair_mutex, NULL);

    pthread_create(&ta, NULL, ta_function, NULL);
    for (int i = 0; i < 10; i++) {
        ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_function, &ids[i]);
    }

    pthread_join(ta, NULL);
    for (int i = 0; i < 10; i++)
        pthread_join(students[i], NULL);

    return 0;
}
