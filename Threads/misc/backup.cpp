
#include <stdio.h>
#include <stdlib.h>
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
#include <deque>


// Convenient pthread macros
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

// global variable containing the number of points inside the circle
int in_circle = 0;

int t_count = 0;

using namespace std; 
// a pair of doubles for the coordinates and an int to represent whether the point is inside the circle.
typedef struct point{
    double x;
    double y;
} point;

typedef struct write_data{
    vector<point> p_in;
    vector<point> p_out;
    int tnum;
    int in;
    int out;
} wdata;
deque<wdata> rgen;

// tests whether a point is inside the unit circle
int inside(long double x, long double y) {
    return (sqrt(x*x + y*y) <= 1);
}

// rng and counting. Also creates the point vector
int gen_points(int n,  wdata * w) {
    int t_out = 0;

    // *p = (point *)malloc(n * sizeof(point));

    for(int i = 0; i < n; i++) {
        double x = 2 * (double)random() / RAND_MAX - 1;
        double y = 2 * (double)random() / RAND_MAX - 1;
        // point temp;
        // temp.x = x;
        // temp.y = y;

        if(inside(x, y)) {
            t_out++;
            // w->p_in.push_back(temp);
        }
        // else {
        //     w->p_out.push_back(temp);
        // }
    }

    return t_out;
}
/*
void thread_io(,  fstream &f,  fstream &g) {

    f << "Points in circle: ";
    g << "Points not in circle: ";
    for (auto u : *p) {
        // if(u.in) fprintf(f, "(%lf, %lf) ", u.x, u.y);
        if(u.in) f << "(" << u.x << ", " << u.y << ") ";
        else g << "(" << u.x << ", " << u.y << ") ";
    }

    f <<  endl;
    g <<  endl <<  endl;
}
*/
// the function in which the thread begins execution.
void * thread_init(void * v) {
    int n = *(int *)v;
    // vector<point> p;
    wdata * thread_out = new wdata;
    thread_out->in = gen_points(n, thread_out);
    thread_out->out = n - thread_out->in;

    // p has all the points.

    // string logname = "logs/";
    // now to get the name of the file, and increment the global point count.
    sem_t * s = sem_open("/sync", O_CREAT, 0666, 1);
    sem_wait(s); // semaphores are fun.
    in_circle += thread_out->in;
    t_count++;
    // thread_out->tnum = t_count;
    // rgen.push_back(*thread_out);
    // logname = logname +  to_string(t_count); // because t_count might change outside the synchonised code region.
    sem_post(s);
    // No longer mutating global variables.

    /*
    // log files
     fstream fs, ft, fu;
    fs.open(logname + "_i", fs.out | fs.trunc);
    ft.open(logname + "_o", fs.out | fs.trunc);

    fs << "Thread " << t_count << ": " << num << " " << n - num <<  endl;

    thread_io(&p, fs, ft); // does all the logging

    // combines into single logfile
     string str = "cat " + logname + "_i " + logname + "_o > " + logname;
    system(str.c_str());   

    fs.close();
    ft.close();*/

    exth(0); // Bye
}


void * thread_io(void * v) {
    int c = *(int *)v;
    // sem_t * s = sem_open("/sync", O_CREAT, 0666, 1);
    // wdata w = rgen.front();
    // cout << "Got.";
    // rgen.erase(rgen.begin());
    // cout << "After.";
    // sem_close(s);

    wdata w = rgen[c];

    string logname = "logs/";
    logname = logname + to_string(w.tnum);

    fstream f;
    f.open(logname, f.out | f.trunc);
    f.precision(3);
    f << "Thread " << w.tnum << ": " << w.in << " " << w.out << endl;
    f << "Points in circle: ";
    for (auto p : w.p_in) {
        f << "(" << p.x << ", " << p.y << ") ";
    }
    f << endl;
    f << "Points outside circle: ";
    for(auto p: w.p_out) {
        f << "(" << p.x << ", " << p.y << ") ";
    }
    f << endl << endl;

    f.close();
    exth(0);

}


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

void spawn_io(int count, vector<T_t> * offspring) {
    T_t tid;
    A_t at;
    int * a = (int *)malloc(count * sizeof(int));
    for(int i = 0; i < count; i++){
        attrth(&at);
        a[i] = i;
        maketh(&tid, &at, thread_io, a + i);
        offspring->push_back(tid); // tids
    }
    free(a);
}

long join_all(int count, vector<T_t> * offspring)
{
    for(int i = 0; i < count; i++)
        jointh(offspring->at(i), NULL); // thread join

    offspring->clear();
    return clock(); // more benchmarking
    
}

pair<int, int> io() {
     fstream fs;
    fs.open("inp.txt", fs.in);
    // fs.read();
    int n, k;
    fs >> n >> k;
    sem_unlink("/sync");
    // system("mkdir logs");
    // FILE * f = fopen("inp.txt", "r+");
    // fscanf(f, "%d %d", &n, &k);
    // fclose(f);

    fs.close();
    return {n, k};
}

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
    long end = join_all(k, &offspring);
    clock_gettime(CLOCK_REALTIME, &tp2);

    // printing estimated value of pi.
    f << "Value: " << 4 * (double)in_circle / n <<  endl;
     cout << "Value: " << 4 * (double)in_circle / n <<  endl;

    double tt = (double) (end - start);

    // printing execution time of parallel part.
    f << "Ticks: " << tt / k << " microseconds\n" <<  endl;
    cout << "Ticks: " << tt / k << " microseconds\n" <<  endl;

    f << "Time: " << (tp2.tv_sec - tp1.tv_sec) * 1000000 + ((double)(tp2.tv_nsec - tp1.tv_nsec)) / 1000 << endl;
    cout << "Time: " << (tp2.tv_sec - tp1.tv_sec) * 1000000 + ((double)(tp2.tv_nsec - tp1.tv_nsec)) / 1000 << endl;


    // spawn_io(k, &offspring);
    // join_all(k, &offspring);
    /*
    f.precision(3);
    for (auto w : rgen) {
        f << "Thread " << w.tnum << ": " << w.in << " " << w.out << endl;
        f << "Points in circle: ";
        for (auto p : w.p_in) {
            f << "(" << p.x << ", " << p.y << ") ";
        }
        f << endl;
        f << "Points outside circle: ";
        for(auto p: w.p_out) {
            f << "(" << p.x << ", " << p.y << ") ";
        }
        f << endl << endl;
    }
    f.close();
    */

}

void combine_files() {
    system("cat logs/* >> output.txt");
    system("rm logs/*");
    system("rmdir logs");
    sem_unlink("/sync");
}

int main() {
    auto p = io();
    montecarlo(p.first, p.second);
    // combine_files();
    // int end = clock();
    // cout << "Serial part takes: " << end - (times.second - times.first) << " microseconds."<<  endl; // time taken by serial part

    return 0;
    
} 
