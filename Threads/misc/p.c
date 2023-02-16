/****
 * Multithreading for perfect number generation
 * for CS3510
 * by Abhay Shankar K
 * CS21BTECH11001
*/

#include "osapi.h"
int * primes;
    T_t * children;
    int n, k;

// #define PARAMS PROT_READ | PROT_WRITE, MAP_SHARED
// #define PARAMS_STD NULL, 1024, PARAMS, fd, 0
// // macos doesn't have non-standard itoa. So, this.
// #define itos(name, i) sprintf(name, "%d", i)

// // concise pthread-related names
// #define maketh pthread_create
// #define attrth pthread_attr_init
// #define jointh pthread_join
// #define exth pthread_exit
// #define T_t pthread_t
// #define A_t pthread_attr_t
// #define gettid pthread_self
// #define lock_t pthread_mutex_t
// #define makelock pthread_mutex_init
// #define lock pthread_mutex_lock
// #define unlock pthread_mutex_unlock
// #define unmakelock pthread_mutex_destroy

 // inputs.
// lock_t the_lock;


// Checks whether a number is prime by a linear search of primes[]. 
// This is better than binary search for this specific application, because 
// the inputs will generally be small and occur near the beginning of primes[].
int isprime(int input)
{
    int i;
    for(i = 1; i <= primes[0]; i++)
        if(primes[i] >= input) break;
    
    if(input == primes[i]) return 1;
    else return 0;
}

// generates the prime sieve. (sieve of atkin)
void gen_sieve(int input)
{
    int S[] = {1, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 49, 53, 59};
    int p = ceil(sqrt(input));
    int * bools = calloc(input + 2, sizeof(int));
    primes = calloc(input + 2, sizeof(int));

    primes[0] = 4; // next write index
    primes[1] = 2;
    primes[2] = 3;
    primes[3] = 5;

    for(int x = 1; x <= p; x++){
        for(int y = 1; y <= p; y += 2){
            int m = 4*x*x + y*y;
            if(m > input) break;
            int q = m % 60;
            int s[] = {1, 13, 17, 29, 37, 41, 49, 53};
            for(int j = 0; j < 8; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}   
        }
    }
    for(int x = 1; x <= p; x++){
        for(int y = 2; y <= p; y += 2){
            int m = 3*x*x + y*y;
            if(m > input) break;
            int q = m % 60;
            int s[] = {7, 19, 31, 43};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }
    for(int x = 2; x <= p; x++){
        for(int y = x - 1; y >= 1; y -= 2){
            int m = 3*x*x - y*y;
            if(m > input) break;
            int q = m % 60;
            int s[] = {11, 23, 47, 59};
            for(int j = 0; j < 4; j++)
                if(q == s[j]) {bools[m] = !bools[m]; break;}
        }
    }

    int * holes = calloc(input, sizeof(int));
    int c = 0;
    for(int i = 0; i <= input; i += 60){
        for(int j = 0; j < 16; j++){
            holes[c] = i + S[j];
            c++;
        }
    }
    // holes[0] = c;
    for(int i = 1; i < c; i++){
        int sq = holes[i]*holes[i];
        if(sq > input) break;
        if(bools[i]){
            for(int r = 0; r < c; r++){
                int m = sq * r;
                if(m > input) break;
                bools[m] = 0;
            }
        }
    }
    for(int i = 7; i <= input; i++){
        if(bools[i]){
            primes[primes[0]] = i;
            primes[0]++;
        }
    }
    free(bools);
    free(holes);

}

// logging. Consider renaming to lumberjack().
void outcome(int number, int status, char * file_name)
{
    FILE * f = fopen(file_name, "a+");
    if(status) fprintf(f, "%d: is a perfect number\n", number);
    else fprintf(f, "%d: is not a perfect number\n", number);
    fclose(f);
}

// uses the Euclid-Euler theorem (stated in report). This is the fastest, because it is specifically about perfect numbers.
// Assumes that no odd perfect numbers exist: valid, since no odd perfect numbers have been discovered.
int mersenne_check(int input)
{
    double soln = 0.25 + sqrt(1.0 / 16 + ((double)input)/2);
    if(soln != (int)soln) return 0;
    soln = 2*soln;
    double p = log2(soln);
    if(p != (int)p) return 0;
    soln--;
    return isprime((int)p) && isprime((int)soln);
}

void * thread_init(void * in){
    T_t this = gettid();

    int j;
    for(j = 0; j < k; j++)
        if(this == children[j]) break;
    
    int m = ceil((double)n / k);
    int s = m*(j) + 1; // start
    int e = m*(j + 1)>n?n:m*(j + 1); // end

    char * perfs = calloc(23, sizeof(char)); // Only 5 perfect numbers less than 10^9. Their length would be 22. "6 28 496 8128 33550336"
    char * temp = calloc(9, sizeof(char)); // contains only one perfect number at a time.
    char * logname = calloc(20, sizeof(char)); // 16 for text, 3 for process number.

    sprintf(logname, "logs/OutFile%d.log", j + 1);

    for(int i = s; i <= e; i++){
        int stat = mersenne_check(i);  // *
        if(stat){
            sprintf(temp, "%d ", i); // buffer
            // printf("Thread%d PERF: %d\n", j + 1, i);
            strcat(perfs, temp);
        }
        // outcome(i, stat, logname); // this is the costliest portion: 
        //the program obviously runs several orders of magnitude faster when it doesn't have to log a million entries.
    }
    
    free(logname);
    free(temp);

    if(strlen(perfs)){
        sem_t * s;
        s = sem_open("/sync", O_CREAT, 0666, 1);
        sem_wait(s);
        // lock(&the_lock);
        // FILE * f = fopen("OutMain.txt", "a");
        printf("Thread%d: %s\n", j + 1, perfs);
        // fclose(f);
        // unlock(&the_lock);
        sem_post(s);
    }
    free(perfs);
}

// // Thread-spawner. No pthread_create()s outside this function.
// void spawn()
// {
//     T_t tid;
//     A_t at;
//     system("rm OutMain.txt");
//     for(int i = 0; i < k; i++){
//         attrth(&at);
//         maketh(&tid, &at, thread_init, NULL);
//         children[i] = tid;
//     }
    
// }

// // called in initial thread. Calls pthread_join() repeatedly for each thread spawned by spawn().
// // unlinks semaphore
// void join_all()
// {
//     for(int i = 0; i < k; i++)
//         jointh(children[i], NULL);
    
//     sem_unlink("/sync");
// }

// Handles user input of filename.
// assigns values to n and k. alloc's children.
void acquire_params()
{
    printf("Enter input file.\n");
    char * fname = malloc(1);
    size_t buf = 0;
    getline(&fname, &buf, stdin);
    fname[strlen(fname) - 1] = 0; // removing terminal \n.

    FILE * fin = fopen(fname, "r");
    fscanf(fin, "%d %d", &n, &k);
    fclose(fin); // we do not need this anymore

    free(fname);
}

// Calls acquire_params(), gen_sieve(), spawn(), join_all(), as well as some system calls.
    
int main()
{
    // acquire_params();
    // T_t * children; 
    // int n, k;
    scanf("%d %d", &n, &k);

    // Constraints. 
    // Due to logging, the program takes too long to compute large numbers.
    // Since only 8 threads can run concurrently on my machine, it is pointless to allow for more processes. 
    // k < 100 just in case this program is tested on a server.
    // Upper bound on n approaches INT_MAX. long could be used, but it is too time-consuming to run.
    if(n < 0 || k < 0){
        printf("Must be positive.\n");
        return 1;
    }
    if(n / 1000000001 || k / 101){
        printf("Too big.");
        return -1;
    }
    
    // system("rm logs/*"); 
    // removes all previous logfiles. Since they are opened in append mode, 
    // this is necessary, otherwise the entries of the previous execution will remain.
    
    gen_sieve((int)ceil(sqrt(2*n))); 
    // The fact that we need less than sqrt(2x) primes to check for perfectness of x follows from the Euclid-Euler theorem. 
    // For feasible values of n, sqrt(n) primes are sufficient, because of the distribution of perfect numbers.

    children = calloc(105, sizeof(T_t)); // error on linux if k + 1 instead of a hardcoded sufficiently large number.
    
    // we add this sem_unlink in case of abnormal termination in the previous execution: it is a persistent object and without this call, sem_open would fail.
    // in normal termination, join_all() calls sem_unlink() and this statement is unnecessary.
    sem_unlink("/sync");
    // makelock(&the_lock, NULL);

    // pthread_create() called in this
    spawn(k, children, thread_init); 
    // pthread_join() called in this
    join_all(k, children);
    // unmakelock(&the_lock);


    free(children);
    free(primes);

}

