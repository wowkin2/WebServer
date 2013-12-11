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

void procConn(int connfd)
{
    char sendBuff[1025] = {0};
    time_t ticks = time(NULL);
    
    snprintf(sendBuff, sizeof(sendBuff), getTempl("www/index.htm"), ctime(&ticks));
    write(connfd, sendBuff, strlen(sendBuff));
    close(connfd);
}

int main(int argc, char *argv[])
{
    
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(80); 
    

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        procConn(connfd);
    }
}
