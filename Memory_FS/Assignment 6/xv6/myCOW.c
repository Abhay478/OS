#include "types.h"
#include "user.h"


int pi[3141];
int main() {
    printf(1, "Flags: [2|6]7 means write bit is 1, [2|6]5 means it is 0.\n");
    pgt();
    int f = fork();
    if(f) {
        wait();
        exit();
    }
    else {
        pgt();
        pi[1500] = 2;
        pgt();
        exit();
    }
}