#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define BUFF_LEN 1025

void logError(char *Caption){}
void logAccess(char *Caption){}

char *getTempl(char *fileName)
{
    if (fileName == 0) return 0;
    if (strlen(fileName) == 0) return 0;
    
    FILE *f = fopen(fileName, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

char *getTimeS()
{
    time_t ticks = time(NULL);
    char buff[30];
    strcpy( buff, ctime(&ticks) );
    char *tmpTime = (char*) malloc ( (strlen(buff) + 1) * sizeof(char) );
    strcpy(tmpTime, buff);
    memset(tmpTime + strlen(tmpTime) - 1, 0, 2);
    return tmpTime;
}

char *procRequest(char *request)
{
    char sendBuff[BUFF_LEN] = {0};
    char *Template = getTempl("www/index.htm");
    char *tmpBuff = 0;
    
    // #################################
    // You need to proccess request here
    // #################################
    printf("[%s] %s", getTimeS(), request);
    
    snprintf(sendBuff, sizeof(sendBuff), Template, getTimeS());
    tmpBuff = (char*) malloc ( (strlen(sendBuff) + 1) * sizeof(char) );
    strcpy(tmpBuff, sendBuff);
    
    free(Template);
        
    return tmpBuff;
}

void procConn(int connfd)
{
    int rc, n;
    char *sendBuff = 0;
    char recvBuff[BUFF_LEN] = {0};

    if( recv(connfd, recvBuff, BUFF_LEN , 0) < 0)
    {
        logError("Error in recv request!");
    }
    sendBuff = procRequest(recvBuff);
    
    write(connfd, sendBuff, strlen(sendBuff));
    free(sendBuff);
    
    close(connfd);
}

int startServer(int listenPort, int maxConnection)
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( -1 == listenfd)
    {
        printf("Error. Can't open Socket!\n");
        close(listenfd);
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (-1 == status)
    {
        perror("setsockopt(...,SO_REUSEADDR,...)");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(listenPort); 
    
    if ( -1 == bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) )
    {
        printf("Error. Can't BIND!\n");
        printf("%s\n", strerror(errno));
        close(listenfd);
        return -2;
    }
    if ( -1 == listen(listenfd, maxConnection) )
    {
        printf("Error. Can't LISTEN!\n");
        close(listenfd);
        return -3;
    }

    printf("\n\nServer started: Listening 80 port with 10 simultaneous connections...\n\n");
    return listenfd;
}
int getComm()
{
    FILE *ptrFileIn;
    char buff[255];
    ptrFileIn = fopen("command.txt", "r");
    if (ptrFileIn == NULL) 
    {
        printf("Can't open command file in!\n");
        return 1;
    }
    fgets(buff, 255, ptrFileIn);
    return strcmp(buff, "shutdown\n");
}
int main(int argc, char *argv[])
{
    int listenSocket = 0, connSocket = 0;
    listenSocket = startServer(80, 10);
    if (listenSocket > 0)
    {
        while(1)
        {
            connSocket = accept(listenSocket, (struct sockaddr*)NULL, NULL);
            procConn(connSocket);
            if (getComm() == 0)
            {
                break;
            }
        }
    }
    close(listenSocket);
    printf("Exited Successfully!\n");
}
