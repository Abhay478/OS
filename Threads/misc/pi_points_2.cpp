
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

long long int in_circle = 0;

bool inside(long double x, long double y) {
    return (sqrt(x*x + y*y) <= 1);
}

int gen_points(int n) {
    int t_out = 0;

    // char * id = malloc(10);
    // sprintf(id, "%d", gettid());
    // FILE * f = fopen("shits.txt", "a+");

    for(int i = 0; i < n; i++) {
        long double x = 2 * (double)random() / RAND_MAX - 1;
        long double y = 2 * (double)random() / RAND_MAX - 1;

        // fprintf(f, "%Lf %Lf %Lf\n", x, y, sqrt(x*x + y*y));
        if(inside(x, y)) {
            t_out++;
        }
    }
    // fclose(f);

    // sem_t * s = sem_open("/sync", O_CREAT, 0666, 1);
    // sem_wait(s);
    // in_circle += t_out;
    // sem_post(s);

    return t_out;
}

void * thread_init(void * v) {
    
    int n = *(int *)v;
    int num = gen_points(n);

    sem_t * s = sem_open("/sync", O_CREAT, 0666, 1);
    sem_wait(s);
    in_circle += num;
    sem_post(s);
    exth(0);
}

long spawn(int count, T_t * offspring, int * n_addr)
{
    T_t tid;
    A_t at;
    // struct timespec t;
    // clock_gettime(CLOCK_REALTIME, &t);
    // long start = t.tv_sec * 1000000000 + t.tv_nsec;
    long t = clock();
    for(int i = 0; i < count; i++){
        attrth(&at);
        maketh(&tid, &at, thread_init, n_addr);
        offspring[i] = tid;
    }

    // return start;
    return t;
}

long join_all(int count, T_t * offspring)
{
    for(int i = 0; i < count; i++)
        jointh(offspring[i], NULL);

    // struct timespec t;
    // clock_gettime(CLOCK_REALTIME, &t);

    // long end = t.tv_sec * 1000000000 + t.tv_nsec;


    return clock();
    
}

// long double run() {
//     long int tot = 1000000;
//     int count = 8;

//     int n = tot / count;
//     int runs = 10;

//     long double out = 0;
    
//     for(int i = 0; i < runs; i++){    
//         T_t * offspring = malloc(count * sizeof(T_t));

//         spawn(count, offspring, thread_init, &n);
//         join_all(count, offspring);

//         out += 4 * ((long double)in_circle / tot);
//         in_circle = 0;
//         free(offspring);
//     }

//     return out / runs;

// }

// int main() {
//     FILE * f = fopen("shits.txt", "w");
//     int runs = 100;
//     for(int i = 0; i < 100; i++)
//         fprintf(f, "%Lf\n", run());
    
// }


int main() {
    int n, k;
    FILE * f = fopen("inp.txt", "r+");
    fscanf(f, "%d %d", &n, &k);
    fclose(f);
    int per = n / k + 1;

    T_t * offspring = (T_t *)malloc(k * sizeof(T_t));
    f = fopen("output.txt", "w");
    long start = spawn(k, offspring, &per);

    long end = join_all(k, offspring);
    fprintf(f, "Value: \n\n%lf\n", 4 * (double)in_circle / n);
    // fprintf(f, "Time: \n\n%ld microseconds\n", (end - start) / 1000);
    double tt = ((double) (end - start) / CLOCKS_PER_SEC) * 1000000 ;
    fprintf(f, "Time: \n\n%lf microseconds\n", (double)(end - start) / k);


} 