/***********************
 * Assignment 3
 * for CS3523
 * by Abhay Shankar K
 * CS21BTECH11001
*/


#include "dot.h"
using namespace std;
class Params;
void thread_init(Params * inp, int id);
random_device rd;
mt19937 gen(rd());
atomic<int> lck = 0;
chrono::system_clock::time_point init;

// Pseudo-global scope, passed around everywhere.
class Params {
    public:
    unsigned long long n, k, t1, t2;
    // fstream f;
    FILE * f;
    vector<bool> waiting;

    vector<thread> thr;

    vector<chrono::duration<double>> wc;
    vector<chrono::duration<double>> avg;
    exponential_distribution<> cs;
    exponential_distribution<> rs;

    Params() {
        fstream fs;
        fs.open("inp-params.txt", ios::in);
        fs >> n >> k >> t1 >> t2;
        fs.close();
        f = fopen("output.txt", "a");

        fprintf(f, "BCAS\n");

        for(int i = 0; i < n; i++) {
            waiting.push_back(false); 
            wc.push_back((chrono::duration<double>)0); 
            avg.push_back((chrono::duration<double>)0);
        }
    }

    ~Params() {
        fclose(f);
        for(int i = 0; i < n; i++) {
            waiting.pop_back(); 
            wc.pop_back(); avg.pop_back();
        }
    }

    // spawn
    void start() {
        for(int i = 0; i < n; i++) {
            thr.push_back(thread(thread_init, this, i));
        }
    }
    // join. IO for graph generation
    void end() {
        for(int i = 0; i < n; i++) {
            thr[i].join();
        }
        fprintf(f, "\n\nMax: %lf\nAvg: %lf\n", *minmax_element(wc.begin(), wc.end()).second, reduce(avg.begin(), avg.end()) / avg.size());
        FILE * ff = fopen("bcas.csv", "a");
        fprintf(ff, "%lf,%lf\n", *minmax_element(wc.begin(), wc.end()).second, reduce(avg.begin(), avg.end()) / avg.size());
        fclose(ff);
    }

};

// Returns waiting time. Entry, critical section.
chrono::duration<double> request(int i, int id, Params * inp) {
    // entry
    inp->waiting[id] = true;
    int temp = 0;
    auto s = chrono::system_clock::now();
    fprintf(inp->f, "CS Request %d by thread %d at %lf s. \n", i, id, (chrono::duration<double>) (s - init));
    while(!lck.compare_exchange_strong(temp, 1) && inp->waiting[id]) {
        // std::this_thread::yield();
        temp = 0;
    }

    //critical
    auto e = chrono::system_clock::now();
    fprintf(inp->f, "CS Entry %d by thread %d at %lf s. \n", i, id, (chrono::duration<double>) (e - init));
    inp->waiting[id] = false;
    this_thread::sleep_for((chrono::duration<double, milli>) inp->cs(gen));

    auto t = chrono::system_clock::now();
    fprintf(inp->f, "CS Exit %d by thread %d at %lf s. \n", i, id, (chrono::duration<double>) (t - init));

    return (chrono::duration<double>) (e - s);
    
} 

// Exit, remainder section.
void accede(int i, int id, Params * inp) {
    //exit
    int j = (id + 1) % inp->n;
    for(; j != id; j = (j + 1) % inp->n ) {
        if(inp->waiting[j]) {
            inp->waiting[j] = false; 
            break;
        }
    }

    if (j == id) {
        lck = 0; 
    }
 
    // remainder
    this_thread::sleep_for((chrono::duration<double, milli>) inp->rs(gen));
}

// Thread starting point
void thread_init(Params * inp, int id) {
    vector<chrono::duration<double>> wt;
    for(int i = 0; i < inp->k; i++) {
        wt.push_back(request(i, id, inp));
        accede(i, id, inp);
    }

    inp->wc[id] = *minmax_element(wt.begin(), wt.end()).second;
    inp->avg[id] = reduce(wt.begin(), wt.end()) / wt.size();
}

// distribution setup
void dists(Params * the) {
    exponential_distribution<> cs(1/the->t1);
    exponential_distribution<> rs(1/the->t2);

    the->cs = cs;
    the->rs = rs;
}

int main() {
    init = chrono::system_clock::now();
    Params * the = new Params();

    dists(the);
    
    the->start();
    the->end();

    delete the;
}
