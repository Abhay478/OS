#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define maketh pthread_create
#define attrth pthread_attr_init
#define jointh pthread_join
#define exth pthread_exit
#define T_t pthread_t
#define A_t pthread_attr_t

void * func(void * v)
{
    int sum = 0;
    int u = atoi((char *)v);
    printf("%d\n", u);
    u++;
    while(--u){
        sum += u;
    }
    int * out = &sum;
    exth(out);
}

int main(int argc, char * argv[])
{
    T_t t;
    A_t a;
    attrth(&a);
    maketh(&t, &a, func, argv[1]);

    void * out;
    jointh(t, out);
    printf("%p\n", out);
    int sum = *(int *)(out);

    printf("%d\n", sum);


}