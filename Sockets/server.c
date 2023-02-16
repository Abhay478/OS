// #include<stdio.h>
// #include<stdlib.h>
// #include<string.h>
// #include<sys/socket.h>
// #include<arpa/inet.h>
// #include<unistd.h>
// #include <errno.h>

#include "osapi.h"

/*
gcc -I ~/Code/Lib/OSAPI server.c ~/Code/Lib/OSAPI/lib.a -o server
./server
*/

int main()
{
    int soc1 = makesock();
    bindsock(soc1, 12345, "127.0.0.1");
    while(1){
        int soc2 = othersock(soc1);
        char * msg = pullsock(soc2);


        if(!strcmp("Hello, world.", msg)){
            pushsock(soc2, "Well, hello there.", 100);
        }
        else if(!strcmp("Kill yoself.", msg)){
            pushsock(soc2, "See ya.", 100);
            printf("Termination command recieved.");
            break;
        }
        else {
            pushsock(soc2, "Who tf are you?", 100);
        }
        free(msg);
    }
    close(soc1);
}