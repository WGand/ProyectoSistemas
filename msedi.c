#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h> 
#include <sys/time.h>
#define BUFLEN 20
static int se = 1;
static long childs = 0;

void iniciar(int sig)
{
    if(sig == SIGUSR1)
    {
        se = 0;
    }
}

int crearSocket()
{
    int sock = socket(PF_INET , SOCK_STREAM , 0);
    if (sock == -1) 
    {
        return 1;
    }
    else
    {
        return sock;
    }
    
}

int pruebaConexion(int sock, unsigned char* buf)
{
    int respuesta;
    if ((respuesta = recv(sock , buf , 1 , 0)) < 0)
        return 1;
    else if (respuesta == 0) 
    {
        return 0;
    }
}

int conexion(int sock, struct sockaddr_in server)
{
        //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
        //perror("connect failed. Error");
        return 1;
    }
    else
    {
        return 0;
    }

}

int main(){

    signal(SIGUSR1, iniciar);
    long padre = getpid();
    pid_t process;
    int status = 0;

    while(se){

        childs++;
        if ((process = fork()) == 0)
        {
            break;
        } 
    }

    if(process == 0)
    {
        struct timeval timeout;
        timeout.tv_sec = 0.0001;
        timeout.tv_usec = 0;
        int conectado = 2;
        int conex = 1;
        struct sockaddr_in server;
        unsigned char buf[BUFLEN + 1];
        server.sin_addr.s_addr = inet_addr("192.168.1.103");
        server.sin_family = AF_INET;
        server.sin_port = htons(80);
        int sock;
        while(se)
        {
            if(conex == 1)
            {
                sock = crearSocket();
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
                conectado = conexion(sock, server);
            }
            if (conectado == 1)
            {
                kill(padre, SIGUSR1);
                kill(0, SIGUSR1);
            }
            if(se)
            {
                conex = pruebaConexion(sock, buf);
            }
        }
        close(sock);
        _exit(conectado);
    }
    if(process > 0)
    {
        int status;
        pid_t pid;
        long conectados = 0;
        long desconectados = 0;
        childs = childs + padre;
        puts("Calculando.");
        for(long i = padre + 1; i<=childs; i++)
        {
            if(pid = waitpid(i, &status, WNOHANG) != -1)
            {
                if(WIFEXITED(status))
                {
                    if(status == 0)
                    {
                        conectados++;
                    }
                    else
                    {
                        desconectados++;
                    }
                }
            }
        }
        printf("hijos: %ld \n", childs - padre);
        printf("Conectados: %ld \n", conectados);
        printf("Desconectados: %ld \n", desconectados);
        printf("Procesos con salida no exitosa: %ld \n", childs - padre - conectados - desconectados);
        kill(0, SIGKILL);
        _exit(0);
    }

}
