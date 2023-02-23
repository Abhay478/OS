#include "dot.h"
using namespace std;
class Params;
void thread_init(Params * inp, int id);
random_device rd;
mt19937 gen(rd());
class Params {
    public:
    int n, k, t1, t2;
    FILE * f;
    // vector<bool> waiting;
    exponential_distribution<> cs;
    exponential_distribution<> rs;
    vector<thread> thr;

    Params() {
        fstream fs;
        fs.open("input.txt", ios::in);
        fs >> n >> k >> t1 >> t2;
        fs.close();
        f = fopen("output.txt", "w");

        // for(int i = 0; i < n; i++) waiting.push_back(true);
        cs(t1);
        rs(t2);
    }

    void start() {
        for(int i = 0; i < n; i++) {
            thr.push_back(thread(thread_init, this, i));
        }
    }

    void end() {
        for(int i = 0; i < n; i++) {
            thr[i].join();
        }
    }

    ~Params() {
        fclose(f);
    }

};

atomic_flag lck = ATOMIC_FLAG_INIT;
chrono::system_clock::time_point init;
void request(int i, int id, Params * inp) {
    auto s = chrono::system_clock::now();
    fprintf(inp->f, "Request %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (s - init));
    while(lck.test_and_set(memory_order_seq_cst)) ;

    auto e = chrono::system_clock::now();
    fprintf(inp->f, "Entry %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (e - init));
    this_thread::sleep_for((chrono::duration<double, milli>) inp->t1);
} 

void accede(int i, int id, Params * inp) {
    auto t = chrono::system_clock::now();
    fprintf(inp->f, "Exit %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (t - init));
    lck.clear(memory_order_seq_cst);

    this_thread::sleep_for((chrono::duration<double, milli>) inp->t2);
}

void thread_init(Params * inp, int id) {
    // auto id = this_thread::get_id();
    for(int i = 0; i < inp->k; i++) {
        request(i, id, inp);
        accede(i, id, inp);
    }
}

int main() {
    init = chrono::system_clock::now();
    Params * the = new Params();
    
    the->start();
    the->end();

    delete the;
}
