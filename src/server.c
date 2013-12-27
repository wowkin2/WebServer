#include "server.h"

/*********************
@purpose: log each error in server work
@TODO
*********************/
void logError(char *Caption){
    char sendBuff[BUFF_LEN] = {0};
    
    FILE *fptr = fopen("log/error.log", "a+");
    fseek(fptr, 0, SEEK_END);
    
    memset(sendBuff, 0, sizeof(sendBuff));
    snprintf(sendBuff, sizeof(sendBuff), "[%s] \"%s\"\n", getTimeS(), Caption);
    fwrite (sendBuff , sizeof(char), strlen(sendBuff), fptr);
    
    fclose(fptr);
}
/*********************
@purpose: log each connection to server
@TODO
*********************/
void logAccess(char *IP, Request *req){
    char sendBuff[BUFF_LEN] = {0};
    
    FILE *fptr = fopen("log/access.log", "a+");
    fseek(fptr, 0, SEEK_END);
    
    memset(sendBuff, 0, sizeof(sendBuff));
    snprintf(sendBuff, sizeof(sendBuff), 
            "%s [%s] \"%s %s %s\" %d  %d\n", 
            IP, getTimeS(), 
            RequestMethodText[req->method], req->URL, HTTPVersionText[req->version], 
            ResponseCode[req->status], 0/*strlen(req->data)*/
            );
            
    fwrite (sendBuff , sizeof(char), strlen(sendBuff), fptr);
    
    fclose(fptr);
}

/*********************
@purpose: start to send file
@input: buff to save data, filename to be read, seekValue offset of file begining
@return: length of readed buffer
@defaults: seekValue = 0
*********************/
int getFileChunk(char *buff, char *fileName, ULI seekValue, ULI sizeValue){
    if (fileName == 0) return -1;
    if (strlen(fileName) == 0) return -1;
    
    FILE *fptr = fopen(fileName, "rb");
    
    fseek(fptr, 0, SEEK_END);
    long fsize = ftell(fptr);
    
    if (seekValue > fsize) return -2;
    fsize = fsize - seekValue;
    
    if (fsize > CHUNK_SIZE)
    fsize = CHUNK_SIZE;
    
    fseek(fptr, seekValue, SEEK_SET);

    buff = malloc(fsize + 1);
    fread(buff, fsize, 1, fptr);
    fclose(fptr);
    
    return fsize;
}
/*********************
@purpose: receive data from socket
@input: connection handle, buffer to save data
*********************/
int recvRequest(int connfd, char **recvBuff){

    return getData(connfd, recvBuff);


    int size_recv , total_size= 0;
    struct timeval begin , now;
    char chunk[CHUNK_SIZE];
    double timediff;
    
    //if (recvBuff != NULL) free(recvBuff);
    *recvBuff=malloc(0);
    
    fcntl(connfd, F_SETFL, O_NONBLOCK);
     
    // beginning time
    gettimeofday(&begin , NULL);     
    while(1)
    {
        gettimeofday(&now , NULL);         
        // time elapsed in seconds
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);

        if( total_size > 0 && timediff > RECV_TIMEOUT )
        {
            break;  // break after timeout, even if got some data
        }
        else if( timediff > RECV_TIMEOUT*2)
        {
            break;  // break twice timeout, even if got no data
        }
         
        memset(chunk ,0 , CHUNK_SIZE);  //clear the variable
        if((size_recv =  recv(connfd, chunk , CHUNK_SIZE , 0) ) < 0)
        {
            // if nothing was received then we want to wait a little before trying again, 0.1 seconds
            usleep(100000);
        }
        else
        {
            *recvBuff = realloc(*recvBuff, (total_size + size_recv)*sizeof(char));
            memcpy(recvBuff + total_size, chunk, size_recv);
            
            total_size += size_recv;
            // reset beginning time
            gettimeofday(&begin , NULL);
        }
    }
    
    return total_size;
}
int getData(int connfd, char **outBuff)
{
    char recvBuff[BUFF_LEN] = {0};
    if( recv(connfd, recvBuff, BUFF_LEN , 0) < 0)
    {
        //logError("Error in recv request!");
    }
    *outBuff = malloc(strlen(recvBuff));
    //strcpy(*outBuff, recvBuff);
    memcpy(*outBuff, recvBuff, strlen(recvBuff));
    memset(*outBuff + strlen(recvBuff), '\0', 1);
    return strlen(recvBuff);
}
/*********************
@purpose: parse request 
@input: connection handle, buffer to save data
*********************/
Request* parseRequest(char *request){
    Request* newReq = malloc(sizeof(Request));
    //newReq = malloc(sizeof(Request*));
    //*newReq = malloc(sizeof(Request));
    memset(newReq, 0, sizeof(Request));
    
    if (request == NULL) (newReq)->status = PROCESS_ERROR;
    else if (strlen(request)<14) (newReq)->status = PROCESS_ERROR; // 14 == strlen("GET / HTTP/x.x")
    
    if ((newReq)->status == PROCESS_ERROR) return (newReq);
    
    char method[255];
    char url[255];
    char protocol[255];
    
    sscanf (request, "%s %s %s", method, url, protocol);
    if (strcmp (method, "GET") == 0) 
    {
        (newReq)->method = METHOD_GET;
        if (strcmp (protocol, HTTPVersionText[HTTP_V_1_0]) == 0)
        {
            (newReq)->version = HTTP_V_1_0;
            (newReq)->URL = malloc(strlen(url));
            strcpy((newReq)->URL, url);        
        }
        else if (strcmp (protocol, HTTPVersionText[HTTP_V_1_1]) == 0) 
        {
            (newReq)->version = HTTP_V_1_1;
            (newReq)->URL = malloc(strlen(url));
            strcpy((newReq)->URL, url);
        }
        else
        {
            (newReq)->status = R_400_BAD_REQUEST;
        }
    }
    else
    {
        (newReq)->status = R_501_BAD_METHOD;
    }
    (newReq)->status = R_200_OK;
    return (newReq);
}
/*********************
@purpose: Proccess one request to generate answer
@input: request string (ex. "GET /index.htm HTTP/1.0\nHostname: example.com\n\n"
@return: answer to be sent
*********************/
Request* procRequest(Request* req){
    //answ = malloc(sizeof(Request*));
    Request *answ = malloc(sizeof(Request));
    memset(answ, 0, sizeof(Request));
    
    if (req == NULL) (answ)->status = -1;
    else if (req->status != PROCESS_ERROR) (answ)->status = -2;
    
    //check URL
    if (req->URL == NULL) (answ)->status = -3;
    else if (strlen(req->URL)<12) (answ)->status = -4; // 14 == strlen("HTTP/x.x ddd")
    
    //printf("\n%s %s %s %s\n", HTTP_Ver, req->method == 0 ? "GET":"???", req->URL, ResponseText[req->status]);

    answ->method = METHOD_GET;
    answ->status = R_200_OK;
    if (strcmp(req->URL, "/") == 0)
    {
        answ->URL = malloc(strlen("index.htm"));
        strcpy(answ->URL, "index.htm");
    }
    else
    {
        answ->URL = malloc(strlen(req->URL)-1);
        strcpy(answ->URL, (req->URL)+1);
    }
    return (answ);

}
char *getPathByVHost(char *vhost)
{
    return "www/";
}
char *GetFileTypeHeader(char *URL)
{
    // get right part (extension) of url (file name, without params)
    // get hash of extension
    switch (hash)
    {
        case HASH_HTM:
        case HASH_HTML:
            
        break;
        case HASH_CSS:
            
        break;
        case HASH_JS:
            
        break;
        case HASH_PNG:
            
        break;
        case HASH_JPEG:
        case HASH_JPG:
            
        break;
        case HASH_ICO:
            
        break;
        case HASH_GIF:
            
        break;

        default:
            
    }
}
/*********************
@purpose: send answer chunk by chunk (if needed)
@input: connection handle, answer block
*********************/
void sendAnswer(int connfd, Request* answ){
    char sendBuff[BUFF_LEN] = {0};
    if (answ == NULL) printf("asnw if NULL\n");
    else if (answ->URL == NULL) printf("URL is NULL\n");
    
    //HEADER
    char *pngHead =
        "Connection: close\n"
        "Content-Type: image/png\n"
        "Content-Transfer-Encoding: binary\n"
        "Content-Length: 2373\n\n";
    char *htmHead =
        "Content-Type: text/html; charset=UTF-8\n\n";
    int a = 1;
    if (strcmp(answ->URL, "somelogo.png") == 0)
    {
        a = 0;
    }
    
    snprintf(sendBuff, sizeof(sendBuff), 
        "%s %s\n%s\n", HTTP_Ver, ResponseText[answ->status], 
        "" GetFileTypeHeader(answ->URL)
        /*a==0?pngHead:htmHead*/
        );
    write(connfd, sendBuff, strlen(sendBuff));
    //\HEADER
    //BODY
    char *rootPath = getPathByVHost("localhost");
    char *fullPath = malloc(strlen(rootPath)+strlen(answ->URL)+1);
    strcpy(fullPath, rootPath);
    strcat(fullPath, answ->URL);
    /*snprintf(sendBuff, sizeof(sendBuff), getTempl(fullPath), getTimeS());
    printf("---------------------------\n%s", sendBuff);*/
    
    //char *pa = getTempl1(connfd, fullPath);
    FILE *f = fopen(fullPath, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buff = malloc(fsize);
    fread(buff, 1, fsize, f);
    fclose(f);
    write(connfd, buff, fsize);
    free(buff);
    //\BODY
}
/*********************
@purpose: Proccess one connection after 'accept'
@input: connection handle
*********************/
void procConn(char *IP, int connfd){
    char *recvBuff = 0;
    Request* req = 0;
    Request* answ = 0;
    do
    {
        recvRequest(connfd, &recvBuff);
        req = parseRequest(recvBuff);
        logAccess(IP, req);
        answ= procRequest(req);
        sendAnswer(connfd, answ);
        
    } while(0/*req->keepAlive != 0*/ /*&& timeDiff < KEEP_ALIVE_TIMEOUT*/);
    close(connfd);
}

/*********************
@purpose: Init listening of server 
@input: port to listen, maximum simultaneous connections
@return: listen socket handle
*********************/
int startServer(int listenPort, int maxConnection){
    int listenfd = 0;
    struct sockaddr_in serv_addr; 
    // create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( -1 == listenfd)
    {
        logError("Error. Can't open Socket!\n");
        close(listenfd);
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    // set socket opt - REUSEADDR
    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (-1 == status)
    {
        logError("Error. Can't set socket option REUSEADDR\n");
    }
    // configure sockaddr_in
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(listenPort); 
    // bind socket
    if ( -1 == bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) )
    {
        logError("Error. Can't BIND!\n");
        logError(strerror(errno));
        close(listenfd);
        return -2;
    }
    // listen port
    if ( -1 == listen(listenfd, maxConnection) )
    {
        logError("Error. Can't LISTEN!\n");
        close(listenfd);
        return -3;
    }
    char printBuff[BUFF_LEN];
    memset(printBuff, 0, sizeof(printBuff));
    snprintf(printBuff, sizeof(printBuff), 
            "Server started: Listening %d port with %d simultaneous connections...", 
            listenPort, maxConnection);
    logError(printBuff);
    printf("\n\n%s\n\n", printBuff);
    return listenfd;
}
/*ULI fsize = 2373;   //getFileSize(fullPath);
    char *fileBuff = 0;
    
    FILE *f = fopen(fullPath, "rb");
    
    fileBuff = malloc(fsize + 1);
    ULI result = fread(fileBuff, 1, fsize, f);
    if (result != fsize) printf("Error\n");
    
    fclose(f);*/

    // open the file
    /*FILE *f;
    unsigned char buffer[1024];
    int n;

    f = fopen(fullPath, "rb");
    if (f)
    {
        n = fread(buffer, 1024, 1, f);
        write(connfd, buffer, n);
    }
    else
    {
        printf("Opening file\n");
    }*/
