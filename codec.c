#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#define BUFLEN 20

int main(int argc , char *argv[]) {
    int sock;
    struct sockaddr_in server;
    unsigned char buf[BUFLEN + 1];
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        //perror("Could not create socket. Error");
        return 1;
    }
 
    server.sin_addr.s_addr = inet_addr("127.0.01");
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        //perror("connect failed. Error");
        return 1;
    }
    //puts("Connected...\n");

    while (1) {
            int respuesta;
            if ((respuesta = recv(sock , buf , 1 , 0)) < 0)
                return 1;
            else if (respuesta == 0) {
                //printf("Connection closed by the remote end\n\r");
                return 0;
            }
    }
    close(sock);
    return 0;
    }
}