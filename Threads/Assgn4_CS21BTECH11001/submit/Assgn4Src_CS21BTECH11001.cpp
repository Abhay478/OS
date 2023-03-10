#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <iomanip> 
#include <atomic>
#include <execinfo.h>
#include <vector>
#include <deque>
#include <chrono>
#include <random>
#include <thread>
#include <semaphore.h>
#include <fcntl.h>           
#include <sys/stat.h>
#include <algorithm>
#include <errno.h>
#include <cstring>

using namespace std;
random_device rd;
mt19937 gen(rd());
chrono::system_clock::time_point init;

class Params {
    int ride_l, cool_l, req_l;
    exponential_distribution<> ride_exp;
    exponential_distribution<> cool_exp;
    exponential_distribution<> req_exp;
    // exponential_distribution<> wander_exp
    public: 
    FILE * f;
    int n, m, k;

    Params(int mode) {
        fstream fs;
        fs.open("inp-params.txt", ios::in);
        fs >> n >> m >> req_l >> cool_l >> k;
        ride_l = 5; // Hardcoded, as requested by TA. Used to be user-defined.
        fs.close();
        f = fopen("output.txt", mode?"w":"a");
        fprintf(f, "\n**************\n%d, %d:\n", n, m);

        exponential_distribution<> x1(ride_l);
        ride_exp = x1;
        exponential_distribution<> x2(req_l);
        req_exp = x2;
        exponential_distribution<> x3(cool_l);
        cool_exp = x3;
    }

    // Return actual ride / request / cooldown times, with an upper bound of 1s. 
    chrono::duration<double, milli> ride() {
        auto out = (chrono::duration<double, milli>) ride_exp(gen);
        if(out > (chrono::duration<double, milli>)1000.0) return (chrono::duration<double, milli>)1000.0;
        else return out;
    }

    chrono::duration<double, milli> cooldown() {
        // return (chrono::duration<double, milli>) cool_exp(gen);
        auto out = (chrono::duration<double, milli>) cool_exp(gen);
        if(out > (chrono::duration<double, milli>)1000.0) return (chrono::duration<double, milli>)1000.0;
        else return out;
    }

    chrono::duration<double, milli> request() {
        // return (chrono::duration<double, milli>) req_exp(gen);
        auto out = (chrono::duration<double, milli>) req_exp(gen);
        if(out > (chrono::duration<double, milli>)1000.0) return (chrono::duration<double, milli>)1000.0;
        else return out;
    }

    ~Params() {
        fclose(f);
    }

};

Params * the;

chrono::duration<double, milli> now() {
    return (chrono::duration<double, milli>) (chrono::system_clock::now() - init);
}

// So first, spawn the m + n threads for all the things. 
// Next, we want to make them wait on each other. Some sort of signaling mechanism, perhaps?
// Also, one global semaphore for all Cars.

class Fleet; // Cars
class Crowd; // Passengers
class Time;

Crowd * crowd = nullptr;
Fleet * fleet = nullptr;
Time * q_time = nullptr;

void passengers_init(int tnum);
void cars_init(int tnum);

enum CarStatus{Service, Cooldown, Waiting};

class Fleet {
    public:
    vector<thread> cars;
    deque<atomic<CarStatus>> status;
    sem_t * glob;

    // Thread spawner.
    Fleet() {
        glob = sem_open("glob", O_CREAT, 0666, 0);
        for(int i = 0; i < the->n; i++) {
            status.emplace_back(Cooldown); // Start in Cooldown because that is the 'do-nothing' state.
            cars.push_back(thread(cars_init, i));
        }

    }

    // Setter.
    void ready(int cnum) {
        status[cnum] = Waiting;
        sem_post(glob);
    }

    // Book a car
    int acquire() {
        int q = sem_wait(glob); // To ensure that there is some car. 
        // Passengers who wish to enter the Park without the protective shield ofered by the car must do so at their own risk.
        if(q == -1) {
            printf("Glob: %s\n", strerror(errno)); 
            // Not necessary now, but did help figuring out big bug, so yay.
        }
        // Polling bad, but polling must. Such is life.
        while(true){
            CarStatus temp = Waiting;
            for(int i = 0; i < status.size(); i++) {
                if (status[i].compare_exchange_strong(temp, Service)) { 
                    // We need CAS coz multiple passenger threads can modify Waiting -> Service. 
                    // Other two do not, since that modification can only be done by a single thread.
                    return i;
                }
                else temp = Waiting;
            }
        }
        return -1; // Never.
    }

    // Un-book a car
    void release(int cnum) {
        status[cnum] = Cooldown;
        // Direct assignment fine, since no other passenger could be in that car. 
        // Which is good, since cmpexchg is inf-looping here. Sigh.
    }

    // Getter. Added in a fit of coding pedantism.
    CarStatus stat(int cnum) {
        return status[cnum];
    }

    // Unlink semaphores or die. Also, thread joiner.
    ~Fleet() {
        sem_unlink("glob");
        for(int i = 0; i < cars.size(); i++) {
            cars[i].join();
        }
    }
};

class Crowd {
    vector<thread> people;

    atomic<int> live;
    public:

    // Leave the museum.
    void leave(int num) {
        live--;
    }

    // Thread joiner.
    ~Crowd() {
        for(int i = 0; i < people.size(); i++) {
            people[i].join();
        }
    }

    // Checker.
    bool alive() {
        return live > 0;
    }

    // Thread spawner.
    Crowd() {
        live = the->m;
        for(int i = 0; i < the->m; i++) {
            people.push_back(thread(passengers_init, i));
        }
    }
};

class Time {
    vector<chrono::duration<double, milli>> cars;
    vector<chrono::duration<double, milli>> people;
    sem_t * p, * c;

    public:
    Time(){
        // reallocation messes with thread-safety, so.
        cars.reserve(the->n);
        people.reserve(the->m);

        p = sem_open("p", O_CREAT, 0666, 1);
        c = sem_open("c", O_CREAT, 0666, 1);
    }
    
    // End of Passenger. 
    void exit(chrono::duration<double, milli> t, int num) {
        int q = sem_wait(p);
        if(q == -1) {
            printf("%s\n", strerror(errno));
        }
        people.push_back(t);
        crowd->leave(num);
        sem_post(p);
        // One of those revolving doors, ig.
    }

    // End of Car.
    void power_off(chrono::duration<double, milli> t, int num) {
        int q = sem_wait(c);
        if(q == -1) {
            printf("%s\n", strerror(errno));
        }
        cars.push_back(t);
        sem_post(c);
    }

    // Logger. 
    void out() {
        FILE * f = fopen("q.csv", "a");
        while (cars.size() < the->n || people.size() < the->m);
        chrono::duration<double, milli> avg;

        // STL coz the usual loop for average was giving some obscure compiler errors.
        avg = (chrono::duration<double, milli>) accumulate(cars.begin(), cars.end(), (chrono::duration<double, milli>)0.0) / cars.size();
        fprintf(f, "%lf,", avg);

        avg = (chrono::duration<double, milli>) accumulate(people.begin(), people.end(), (chrono::duration<double, milli>)0.0) / people.size();
        fprintf(f, "%lf\n", avg);

        // Unlink semaphores or die.
        sem_unlink("p");
        sem_unlink("c");

    }

};

/* Big debugging thing.
void handler(int sig) {
    void * array[100];
    size_t size;

    size = backtrace(array, 100);

    fprintf(stderr, "%d:\n", getpid());
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    // exit(1);
    // for(int i = 0; i < the->n; i++) {
    //     cout << fleet->cars[i].get_id() << endl;
    // }
    while(true);
}
*/

// Passenger thread starter.
void passengers_init(int tnum) {
    auto start = now();

    fprintf(the->f, "Passenger %d entered the museum at %lf.\n", tnum, start);

    for(int i = 0; i < the->k; i++){
        this_thread::sleep_for(the->request()); // Assuming initial wander.
        fprintf(the->f, "Passenger %d made a request at %lf.\n", tnum, now());
        int cnum = fleet->acquire();

        if(cnum == -1) {
            cout << "Tf." << endl;
            exit(-1);
        }
        this_thread::yield();

        fprintf(the->f, "Passenger %d boarded Car %d at %lf.\n", tnum, cnum, now());
        this_thread::sleep_for(the->ride());
        fleet->release(cnum);
        fprintf(the->f, "Passenger %d deboarded Car %d at %lf.\n", tnum, cnum, now());

    }    

    auto end = now();

    fprintf(the->f, "Passenger %d exited at %lf.\n", tnum, end);
    q_time->exit((chrono::duration<double, milli>) (end - start), tnum);

}

// Car thread starter.
void cars_init(int tnum) {
    auto start = now();
    fprintf(the->f, "Car %d activated at %lf.\n", tnum, start);
    
    while(crowd == nullptr);
    int count = 0;

    while(crowd->alive()) {    
        fleet->ready(tnum);
        if(crowd->alive()) fprintf(the->f, "Car %d waiting at %lf.\n", tnum, now());
        while(fleet->stat(tnum) == Waiting && crowd->alive()) {
            // this_thread::yield();
        }

        if(crowd->alive()) fprintf(the->f, "Car %d started ride at %lf.\n", tnum, now());
        while(fleet->stat(tnum) == Service && crowd->alive());
        
        if(crowd->alive()) {
            count++;
            fprintf(the->f, "Car %d entering cooldown at %lf.\n", tnum, now());
            this_thread::sleep_for(the->cooldown());
        }
        
    }

    auto end = now();
    fprintf(the->f, "Car %d deactivated at %lf, after %d rides.\n", tnum, end, count);
    q_time->power_off((chrono::duration<double, milli>) (end - start), tnum);

}

// main
int main(int argc, char * argv[]) {
    init = chrono::system_clock::now();
    /* Debugging things.
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
    */

   // Constructor mania.
    the = new Params(argc - 1);
    fleet = new Fleet();
    crowd = new Crowd();
    q_time = new Time();

    // Contains busy wait. 
    q_time->out();

    // All the unlinks and joins.
    delete q_time;
    delete fleet;
    delete crowd;
    delete the;

    return 0;
}

