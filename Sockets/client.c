// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <string.h>
// #include <errno.h>

// int makesock() {
//     int soc1 = socket(AF_INET, SOCK_STREAM, 0);
//     if(soc1 == -1){
//         printf("%s\n", strerror(errno));
//         return 1;
//     }
//     else printf("Socket made: %d\n", soc1);
//     return soc1;
// }

// void connectsock(int socket, int port, char * IP){
//     struct sockaddr_in s;
//     s.sin_family = AF_INET;
//     s.sin_addr.s_addr = inet_addr(IP);
//     s.sin_port = htons(port);
    
//     if (connect(socket, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) == -1) printf("%s\n", strerror(errno));
//     else printf("Successfully connected to %s:%d\n", IP, port);
// }

// void pushsock(int socket, char * message) 
// {
//     struct timeval tv;
//     tv.tv_sec = 20;
//     tv.tv_usec = 0;

//     if( setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1)
//         printf("%s\n", strerror(errno));
//     else
//         send(socket, message, strlen(message), 0); 
// }

// void * pullsock(int socket)
// {
//     void * out = calloc(100, sizeof(char));
//     if(recv(socket, out, 100, 0) == -1)
//         printf("%s\n", strerror(errno));
//     else return out;
// }

#include "osapi.h"

/*
gcc -I ~/Code/Lib/OSAPI client.c ~/Code/Lib/OSAPI/lib.a -o client
./client "Hello, world."
*/

int main(int argc, char * argv[])
{
    int soc1 = makesock();

    connectsock(soc1, 12345, "127.0.0.1");
    pushsock(soc1, argv[1], strlen(argv[1]));
    char * ret = (char *)pullsock(soc1);

    printf("%s\n", ret);
    free(ret);

    close(soc1);

}