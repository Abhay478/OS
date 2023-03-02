#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <semaphore.h>

using namespace std;
random_device rd;
mt19937 gen(rd());
chrono::system_clock::time_point init;

class Museum;
Museum * museum;

class Params {
    int ride_time, cool_time, req_time;
    exponential_distribution<> ride_exp;
    exponential_distribution<> cool_exp;
    exponential_distribution<> req_exp;
    // exponential_distribution<> wander_exp
    public: 
    FILE * f;
    FILE * pass;
    FILE * car;
    int n, m, k;

    Params() {
        fstream fs;
        fs.open("inp-params.txt", ios::in);
        fs >> n >> m >> req_time >> cool_time >> ride_time >> k;
        fs.close();
        f = fopen("output.txt", "w");
        pass = fopen("pass.csv", "w");
        car = fopen("car.csv", "w");

        exponential_distribution<> x1(1/ride_time);
        ride_exp = x1;
        exponential_distribution<> x2(1/req_time);
        req_exp = x2;
        exponential_distribution<> x3(1/cool_time);
        cool_exp = x3;
    }

    chrono::duration<double, milli> ride() {
        return (chrono::duration<double, milli>) ride_exp(gen);
    }

    chrono::duration<double, milli> cooldown() {
        return (chrono::duration<double, milli>) cool_exp(gen);
    }

    chrono::duration<double, milli> request() {
        return (chrono::duration<double, milli>) req_exp(gen);
    }

    // chrono::duration<double, milli> wander() {
    //     (chrono::duration<double, milli>) wander_exp(gen);
    // }

    ~Params() {
        fclose(f);
        fclose(pass);
        fclose(car);
    }

};

chrono::duration<double, milli> now() {
    return (chrono::duration<double, milli>) (chrono::system_clock::now() - init);
}

class Car {
    atomic<int> status;
    sem_t * s;
    int count;
    chrono::duration<double, milli> tot;

    bool available() {
        int exc = 1;
        return (status.compare_exchange_strong(exc, 0));
    }

    public:

    chrono::duration<double, milli> go(Params * the, int cnum, int pnum) {
        if(!available()) return (chrono::duration<double, milli>) 0.0; // CAS check. Allows a single passenger onboard. Sets status to 0 to prevent any other passenger from intercepting the sem_wait call.
        
        sem_wait(s); // Passenger acquires semaphore, ride has begun.
        fprintf(the->f, "Car %d has accepted Passenger %d at %lf.\n", cnum, pnum, now());
        this_thread::sleep_for(the->ride());
        
        auto u = now();
        fprintf(the->f, "Car %d has returned Passenger %d at %lf.\n", cnum, pnum, u);
        status = -1;
        this_thread::sleep_for(the->cooldown()); // cooldown. Why, I do not know.
        
        status = 1; // Since in critical section, direct assignment is fine. available() would now return true.
        fprintf(the->f, "Car %d is now accepting passengers at %lf.\n", cnum, now());
        sem_post(s);
        
        return u;
    }

    
    Car() {
        sem_init(s, 0, 1);
        status = 1;
    }
    ~Car() {
        sem_destroy(s);
    }
};

class Museum {
    vector<Car *> cars;
    public: 
    Museum(Params * the) {
        for(int i = 0; i < the->m; i++) {
            cars.push_back(new Car());
        }
    }
    chrono::duration<double, milli> accept(Params * the, int pnum) {
        while(true) {
            for(int c = 0; c < the->m; c++) {
                auto t = cars[c]->go(the, c, pnum);
                if(t != (chrono::duration<double, milli>) 0.0) return t;
            }
        }
    }
};

void thread_init(Params * the, int pnum) {
    chrono::duration<double, milli> tot = (chrono::duration<double, milli>) 0;
    for(int j = 0; j < the->k; j++){    
        this_thread::sleep_for(the->request()); // wander
        auto start = now();
        fprintf(the->f, "Passenger %d has requested ride %d at %lf.\n", pnum, j, start);
        auto end = museum->accept(the, pnum);

        tot += end - start;
    }
    tot /= the->k;
    fprintf(the->f, "Passenger %d exits the museum at %lf.\n", pnum, now());
    fprintf(the->pass, "%lf\n", tot);
}

class Passengers {
    vector<thread> thr;
    public:
    void spawn(Params * the) {
        for(int i = 0; i < the->n; i++) {
            thr.push_back(thread(thread_init, the, i));
            fprintf(the->f, "Passenger %d enters the museum at %lf.\n", i, now());
        }
    }

    void join(Params * the) {
        for(int i = 0; i < the->n; i++) {
            thr[i].join();
            // cout << i << endl;
        }
    }

    ~Passengers() {
        while(!thr.empty()) {
            thr.pop_back();
        }
    }
};

int main() {
    init = chrono::system_clock::now();
    Params * the = new Params();
    museum = new Museum(the);

    Passengers * all = new Passengers();
    all->spawn(the);
    all->join(the);

    // delete museum;
    // delete the;
    // delete all;

    return 0;
}