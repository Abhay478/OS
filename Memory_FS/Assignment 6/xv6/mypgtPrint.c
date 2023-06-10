#include "types.h"
#include "user.h"
// #include "defs.h"

int glob[1000];
int main() {
    printf(1, "?\n");
    int * loc = (int *)malloc(4000);
    printf(1, "%x\n", (uint)loc);
    // if(!loc) printf(1, "Die.\n");
    printf(1, "%d\n", glob[999]);
    printf(1, "%d\n", loc[1]);
    pgt();
    exit();
}