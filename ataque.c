#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h> 
#define BUFLEN 20
static int se = 1;

void iniciar(int sig)
{
    if(sig == SIGUSR1)
    {
        se = 0;
    }
}

int crearSocket()
{
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) 
    {
        perror("Could not create socket. Error");
        return 1;
    }
    else
    {
        return sock;
    }
    
}

int pruebaConexion(int sock, unsigned char* buf)
{
    while (1) {
        int respuesta;
        if ((respuesta = recv(sock , buf , 1 , 0)) < 0)
            return 1;
        else if (respuesta == 0) 
        {
            printf("Connection closed by the remote end\n\r");
            return 0;
        }
    }
}

int conexion(int sock, struct sockaddr_in server)
{
        //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        perror("connect failed. Error");
        return 1;
    }
    else
    {
        puts("Connected...\n");
        return 0;
    }

}

int main(){

    signal(SIGUSR1, iniciar);
    pid_t process;
    int status = 0;
    int max = 15;
    for(int i = 0; i < max; i++)
    {
        if ((process = fork()) == 0)
        {
            break;
        }        
    }

    if(process == 0)
    {
        while(se){}
        struct sockaddr_in server;
        unsigned char buf[BUFLEN + 1];
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_family = AF_INET;
        server.sin_port = htons(80);
        int sock = crearSocket();
        conexion(sock, server);
        pruebaConexion(sock, buf);
        close(sock);
        exit(0);
    }
    else
    {
        kill(0, SIGUSR1);
        while((process = wait(&status))>0);
    }
    

    return 0;
}