
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define maketh pthread_create
#define attrth pthread_attr_init
#define jointh pthread_join
#define exth pthread_exit
#define T_t pthread_t
#define A_t pthread_attr_t
#define gettid pthread_self
#define lock_t pthread_mutex_t
#define makelock pthread_mutex_init
#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock
#define unmakelock pthread_mutex_destroy

void * thread_init(void * v) {
    printf("%d\n", getpid());
    exth(0);
}

int main() {
    A_t at;
    T_t tid;

    for(int i = 0; i < 10; i++) {
        attrth(&at);
        maketh(&tid, &at, thread_init, NULL);
        jointh(tid, NULL);
    }
}