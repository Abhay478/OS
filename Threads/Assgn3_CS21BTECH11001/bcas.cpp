#include "dot.h"
using namespace std;
class Params;
void thread_init(Params * inp, int id);
random_device rd;
mt19937 gen(rd());
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
        fs.open("input.txt", ios::in);
        fs >> n >> k >> t1 >> t2;
        fs.close();
        f = fopen("output.txt", "w");

        for(int i = 0; i < n; i++) {
            waiting.push_back(false); 
            wc.push_back((chrono::duration<double>)0); 
            avg.push_back((chrono::duration<double>)0);
        }
    }

    ~Params() {
        fclose(f);
        for(int i = 0; i < n; i++) {waiting.pop_back(); wc.pop_back(); avg.pop_back();}
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
        fprintf(f, "\n\nMax: %lf\nAvg: %lf\n", *minmax_element(wc.begin(), wc.end()).second, reduce(avg.begin(), avg.end()) / avg.size());
        FILE * ff = fopen("bcas.csv", "a");
        fprintf(ff, "%lf,%lf\n", *minmax_element(wc.begin(), wc.end()).second, reduce(avg.begin(), avg.end()) / avg.size());
        fclose(ff);
    }

};

atomic<int> lck = 0;
chrono::system_clock::time_point init;
chrono::duration<double> request(int i, int id, Params * inp) {
    
    auto s = chrono::system_clock::now();
    fprintf(inp->f, "Request %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (s - init));
    // printf("Request %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (s - init));
    inp->waiting[id] = true;
    int temp = 0;
    while(inp->waiting[id] && !lck.compare_exchange_strong(temp, 1, memory_order_seq_cst)) {temp = 0;}
    inp->waiting[id] = false;

    auto e = chrono::system_clock::now();
    fprintf(inp->f, "Entry %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (e - init));
    // printf("Entry %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (e - init));
    // cout << "!" << endl;
    this_thread::sleep_for((chrono::duration<double, milli>) inp->cs(gen));

    return (chrono::duration<double>) (e - s);
    
} 

void accede(int i, int id, Params * inp) {
    auto t = chrono::system_clock::now();
    fprintf(inp->f, "Exit %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (t - init));
    // printf("Exit %d by thread %d at %lf. \n", i, id, (chrono::duration<double>) (t - init));
    // cout << "~" << endl;
    int j = (id + 1) % inp->n;
    for(; j != id; j = (j + 1) % inp->n ) {
        if(inp->waiting[j]) {
            inp->waiting[j] = false; 
            // cout << "Passed to " << j << endl; 
            break;
        }
    }

    if (j == id) {
        lck = 0; 
        // cout << "Free." << endl;
    }
 
    this_thread::sleep_for((chrono::duration<double, milli>) inp->rs(gen));
}

void thread_init(Params * inp, int id) {
    vector<chrono::duration<double>> wt;
    for(int i = 0; i < inp->k; i++) {
        wt.push_back(request(i, id, inp));
        accede(i, id, inp);
    }

    inp->wc[id] = *minmax_element(wt.begin(), wt.end()).second;
    inp->avg[id] = reduce(wt.begin(), wt.end()) / wt.size();
}


int main() {
    init = chrono::system_clock::now();
    Params * the = new Params();

    exponential_distribution<> cs(the->t1);
    exponential_distribution<> rs(the->t2);

    the->cs = cs;
    the->rs = rs;
    
    the->start();
    the->end();

    delete the;
}
