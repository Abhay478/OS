
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <semaphore.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <sys/random.h>
// cpp headers include stdlib.h or cstdlib twice. Therefore, undef everything and get it from the original.
#ifdef free
#undef free
#endif

#include <stdlib.h>

// Convenient pthread macros
#define maketh pthread_create
#define attrth pthread_attr_init
#define jointh pthread_join
#define exth pthread_exit
#define T_t pthread_t
#define A_t pthread_attr_t
#define gettid pthread_self
/*
#define lock_t pthread_mutex_t
#define makelock pthread_mutex_init
#define lock pthread_mutex_lock
#define unlock pthread_mutex_unlock
#define unmakelock pthread_mutex_destroy
*/

// global variable containing the number of points inside the circle
int in_circle = 0;

// global thread identifier
int t_count = 0;

using namespace std; // yay!

// a pair of doubles for the coordinates
typedef struct point{
    double x;
    double y;
} point;

// Thread data
typedef struct write_data{
    vector<point> p_in;
    vector<point> p_out;
    int tnum;
    int in;
    int out;
} wdata;
vector<wdata> rgen;

// Euclidean norm
int inside(long double x, long double y) {
    return (sqrt(x*x + y*y) <= 1);
}

// rng and counting. Also creates the point vector
int gen_points(int n,  wdata * w) {
    int t_out = 0;

    for(int i = 0; i < n; i++) {
        double x = 2 * (double)random() / RAND_MAX - 1; 
        double y = 2 * (double)random() / RAND_MAX - 1;

        point temp;
        temp.x = x;
        temp.y = y;

        if(inside(x, y)) {
            t_out++;
            w->p_in.push_back(temp);
        }
        else {
            w->p_out.push_back(temp);
        }
    }
    return t_out;
}

// the function in which the computation threads begin execution.
void * thread_init(void * v) {
    int n = *(int *)v;
    wdata thread_out; // everything
    thread_out.in = gen_points(n, &thread_out);
    thread_out.out = n - thread_out.in;

    // p has all the points.

    // Increment global point count, store the points generated in a global variable
    sem_t * s = sem_open("/sync", O_CREAT, 0666, 1);
    sem_wait(s); // semaphores are fun.
    in_circle += thread_out.in;
    t_count++;
    thread_out.tnum = t_count;
    rgen.push_back(thread_out);
    sem_post(s);

    exth(0); // Bye
}

// the function in which the io threads begin execution.
void * thread_io(void * v) {
    int c = *(int *)v;
    wdata w = rgen[c];

    string logname = "logs/";
    logname = logname + to_string(w.tnum); // using fstream because cpp strings are much nicer than C

    fstream f;
    f.open(logname, f.out | f.trunc);
    f.precision(3);
    f << "Thread " << w.tnum << ": " << w.in + w.out << " " << w.in << endl;
    f << "Points in square: ";

    for(auto p: w.p_out) 
        f << "(" << p.x << ", " << p.y << ") ";
    
    for (auto p : w.p_in) 
        f << "(" << p.x << ", " << p.y << ") ";
    
    f << endl;
    f << "Points in circle: ";

    for (auto p : w.p_in) 
        f << "(" << p.x << ", " << p.y << ") ";
    
    f << endl << endl;

    f.close();
    exth(0);

}

// computation thread spawner
long spawn(int count,  vector<T_t> * offspring, int * n_addr)
{
    T_t tid;
    A_t at;
    long t = clock(); // benchmarking
    for(int i = 0; i < count; i++){
        attrth(&at);
        maketh(&tid, &at, thread_init, n_addr);
        offspring->push_back(tid); // tids
    }

    return t; // start time of threads.
}

// io thread spawner
int * spawn_io(int count, vector<T_t> * offspring) {
    T_t tid;
    A_t at;
    int * a = (int *)malloc(count * sizeof(int));
    for(int i = 0; i < count; i++){
        attrth(&at);
        a[i] = i;
        maketh(&tid, &at, thread_io, a + i);
        offspring->push_back(tid); // tids
    }
    return a;
}

// thread vector joining, regardless of who spawned it
long join_all(int count, vector<T_t> offspring)
{
    for(auto i : offspring)
        jointh(i, NULL); // thread join

    return clock(); // more benchmarking
    
}

// preliminary input.
pair<int, int> io() {
    fstream fs;
    fs.open("inp.txt", fs.in);
    int n, k;
    fs >> n >> k;
    sem_unlink("/sync"); // for aborts.
    system("mkdir logs"); // temporary

    fs.close();
    return {n, k};
}

// the big one
void montecarlo(int n, int k) {
    int per = (!n%k) ? (n / k) : (n / k + 1);

    vector<T_t> offspring;
    fstream f;
    f.open("output.txt", f.out | f.trunc);

    // spawning threads
    struct timespec tp1, tp2;
    clock_gettime(CLOCK_REALTIME, &tp1);
    long start = spawn(k, &offspring, &per);

    // joining threads
    long end = join_all(k, offspring);
    clock_gettime(CLOCK_REALTIME, &tp2);

    // printing estimated value of pi.
    f << "Value: " << 4 * (double)in_circle / n <<  endl;
    //  cout << "Value: " << 4 * (double)in_circle / n <<  endl;

    double tt = (double) (end - start);

    // printing execution time of parallel part.
    f << "Ticks: " << tt / k <<  endl;
    // cout << "Ticks: " << tt / k <<  endl;

    f << "Time: " << (tp2.tv_sec - tp1.tv_sec) * 1000000 + ((double)(tp2.tv_nsec - tp1.tv_nsec)) / 1000 << " microseconds\n" << endl;
    // cout << "Time: " << (tp2.tv_sec - tp1.tv_sec) * 1000000 + ((double)(tp2.tv_nsec - tp1.tv_nsec)) / 1000 << " microseconds\n" << endl;

    // cout << tt / k << " " << (tp2.tv_sec - tp1.tv_sec) * 1000000 + ((double)(tp2.tv_nsec - tp1.tv_nsec)) / 1000 << endl;

    // logging
    vector<T_t> io_offspring;
    int * a = spawn_io(k, &io_offspring);
    join_all(k, io_offspring);
    free(a);
    
}

// file concatenation and cleanup
void combine_files() {
    system("cat logs/* >> output.txt");
    system("rm logs/*");
    system("rmdir logs");
    sem_unlink("/sync");
}

int main() {
    auto p = io();
    montecarlo(p.first, p.second);
    combine_files();

    return 0;
    
} 
