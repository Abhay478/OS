#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <deque>
#include <utility>
using namespace std;

#define PG(a) (a/sz)

// linear search
bool present(deque<int> &q, int n) {
    for(int i = 0; i < q.size(); i++) {
        if (q[i] == n) {
            // cout << "Hit at " << q[i] << ".\n";
            return true;
        }
    }
    return false;
}

void FIFO() {
    int fr, sz;
    fstream f;
    f.open("input.txt", ios::in);
    f >> fr >> sz;

    deque<int> q;
    int acc;
    int out = 0;
    while (!f.eof()) {
        f >> acc;
        acc = PG(acc);
        if(!present(q, acc)){  // Miss.
            if(q.size() >= fr) {
                q.pop_front();
            }
            q.push_back(acc); 
            out++;
        }
    }
    cout << "FIFO had " << out << " faults." << endl;

    f.close();
}

// LRU subroutine. Shuffles around deque.
bool requeue(deque<int> &q, int p) {
    for(int i = 0; i < q.size(); i++) {
        if(q[i] == p) {
            // cout << "Hit " << p << endl;
            q.erase(q.begin() + i);
            q.push_front(p);
            return true;
        }
    }
    return false;
}

void LRU() {
    int fr, sz;
    fstream f;
    f.open("input.txt", ios::in);
    f >> fr >> sz;

    deque<int> q;
    int acc;
    int out = 0;
    while(!f.eof()) {
        f >> acc;
        acc = PG(acc);
        if(!requeue(q, acc)) { // Miss.
            if(q.size() >= fr) {
                q.pop_back();
            }
            q.push_front(acc); 
            out++;
        } 
    }
    cout << "LRU had " << out << " faults." << endl;

    f.close();
}

// Chooses eviction victim for OPT.
void kill(deque<int> &all, deque<int> &mem, int acc) {
    vector<int> v;
    // Init to max possible, covers case where no future occurence of page.
    for(int i = 0; i < mem.size(); i++) {
        v.push_back(all.size());
    }

    // Finds next.
    for(int i = 0; i < mem.size(); i++) {
        for(int j = 0; j < all.size(); j++) {
            if(mem[i] == all[j]) {
                v[i] = j;
                break;
            }
        }
    }

    // Checks max.
    int m = 0, mi = 0;
    for(int i = 0; i < mem.size(); i++) {
        if(v[i] > m) {
            m = v[i];
            mi = i;
        }
    }

    // Replace.
    mem[mi] = acc;

}

void OPT() {
    int fr, sz;
    fstream f;
    f.open("input.txt", ios::in);
    f >> fr >> sz;

    deque<int> all;
    int acc;
    while (!f.eof()){
        f >> acc;
        acc = PG(acc);
        all.push_back(acc);
    }
    deque<int> mem;
    int out = 0;
    while(all.size()) {
        acc = PG(all.front());
        all.pop_front();
        if(!present(mem, acc)){    // Miss.
            if(mem.size() >= fr) kill(all, mem, acc);
            else mem.push_back(acc);
            out++;
        }
    }

    cout << "OPT had " << out << " faults." << endl;
    f.close();
    
}

int main() {
    FIFO();
    LRU();
    OPT();
}