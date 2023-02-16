#include "osapi.h"
int main(){
    int soc1 = makesock();
    bindsock(soc1, 101, "127.0.0.1");
    char * nic = pullsock(soc1);

    printf("%s\n", nic);
    free(nic);

    close(soc1);
}