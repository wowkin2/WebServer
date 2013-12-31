#ifndef __SERVER_C__
#define  __SERVER_C__
#include "server.h"
#include "log.c"

static char *defaultPage = 0;
static char *rootFolder = 0;

/*******************************************************************************
@purpose: create full path concating 'rootFolder' and 'someURL'
@input: relative of www-folder path 
@output: absolute file path
*******************************************************************************/
char *getFullPath(char *URL)
{
    char *tmp = malloc(strlen(rootFolder)+strlen(URL)+1);
    strcpy(tmp, rootFolder);
    strcat(tmp, URL);
    return tmp;
}
/*******************************************************************************
@purpose: 
*******************************************************************************/
char *GetFileTypeHeader(char *URL)
{
    int hash = 0;
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

        //default:
            
    }
    return "";
}
/*******************************************************************************
@purpose: start to send file
@input: buff to save data, filename to be read, seekValue offset of file begining
@return: length of readed buffer
@defaults: seekValue = 0
*******************************************************************************/
/*int getFileChunk(char *buff, char *fileName, ULI seekValue, ULI sizeValue){
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
}*/

#ifndef __FUNCTION_MODE__
void handleRequest(int connfd){
#else
/*******************************************************************************
@purpose: receive data from socket
@input: connection handle, buffer to save data
*******************************************************************************/
int recvRequest(int connfd, char **outBuff){
#endif
    char recvBuff[BUFF_LEN] = {0};
    int len = recv(connfd, recvBuff, BUFF_LEN , 0);
    if( len < 0)
    {
        logError("Error. Recv an empty request!");
    }
    
#ifdef __FUNCTION_MODE__
    *outBuff = malloc(len);
    memcpy(*outBuff, recvBuff, len);
    memset(*outBuff + len, '\0', 1);
    return len;
}
/*******************************************************************************
@purpose: parse request 
@input: connection handle, buffer to save data
*******************************************************************************/
Request* parseRequest(char *recvBuff){
#endif
    Request* req = malloc(sizeof(Request));
    memset(req, 0, sizeof(Request));   // here: 'req->status' is set to 'R_200_OK'
    
    // Check if request is valid
    if (recvBuff == NULL) req->status = R_500_SERVER_ERROR;
    else if (strlen(recvBuff)<14) req->status = R_400_BAD_REQUEST; // 14 == strlen("GET / HTTP/x.x")
    if (req->status == PROCESS_ERROR) return req;
    
    // Parse request to separate fields
    char method[255];
    char url[255];
    char protocol[255];
    sscanf (recvBuff, "%s %s %s", method, url, protocol);
    
    if (strcmp(method, "GET") == 0) // Check if method is supported
    {
        req->method = METHOD_GET;
        req->status = R_505_BAD_HTTP_VERSION;
        // Check HTTP version
        if (strcmp(protocol, HTTPVersionText[HTTP_V_1_0]) == 0)
        {
            req->version = HTTP_V_1_0;
            req->status = R_200_OK;
        }
        else if (strcmp(protocol, HTTPVersionText[HTTP_V_1_1]) == 0) 
        {
            req->version = HTTP_V_1_1;
            req->status = R_200_OK;
        }
        // Parse other parts of request
        if (req->status != R_505_BAD_HTTP_VERSION)
        {
            // parse url
            req->URL = malloc(strlen(url));
            strcpy(req->URL, url);
            
            // TODO: parse other parameters from request
        }
    }
    // Now only GET is supporting
    else
    {
        req->status = R_501_BAD_METHOD;
    }
#ifdef __FUNCTION_MODE__
    return req;
}
/*******************************************************************************
@purpose: Proccess one request to generate answer
@input: request string (ex. "GET /index.htm HTTP/1.0\nHostname: example.com\n\n")
@return: answer to be sent
*******************************************************************************/
Request* procRequest(Request* req){
#endif
    Request *answ = malloc(sizeof(Request));
    memset(answ, 0, sizeof(Request));   // here: 'answ->status' is set to 'R_200_OK'
    
    if (req == NULL) answ->status = R_500_SERVER_ERROR;
    else if (req->status == PROCESS_ERROR) puts("Ooopss1"); //answ->status = R_500_SERVER_ERROR;
    
    //check URL
    else if (req->URL == NULL) puts("Ooopss2"); //answ->status = R_500_SERVER_ERROR;
    
    answ->method = METHOD_GET;      //TODO check if we need this line
    answ->version = req->version;
    if ( answ->status == R_200_OK ) // R_200_OK was set as default value, can be changed on errors
    {
        // change page to default if host root is requested
        char *tmp = defaultPage;
        if (strcmp(req->URL, "/") != 0)
        {
            tmp = req->URL;
        }
        answ->URL = malloc(strlen(tmp));
        strcpy(answ->URL, tmp+1);
        
        // check if file is found
        char *fullPath = malloc(strlen(rootFolder)+strlen(answ->URL)+1);
        strcpy(fullPath, rootFolder);
        strcat(fullPath, answ->URL);
        if ( fileExists(fullPath) != 0)
        {
            answ->status = R_404_NOT_FOUND;
        }
        
        // check if client has permissions to file
        if (0 /* TODO Security */)
        {
            answ->status = R_403_FORBIDDEN;
        }
    }
#ifdef __FUNCTION_MODE__
    return answ;

}
/*******************************************************************************
@purpose: send answer chunk by chunk (if needed)
@input: connection handle, answer block
*******************************************************************************/
void sendAnswer(int connfd, Request* answ){
    char *fullPath = 0;
#endif
    char sendBuff[BUFF_LEN] = {0};
    char errorFlag = 0;
    
    // Check if answer was generated
    if (answ == NULL) errorFlag = 1;
    else if (answ->URL == NULL) answ->status = R_500_SERVER_ERROR;
    // Check if can send
    
    //HEADER
    snprintf(sendBuff, sizeof(sendBuff), 
                "%s %s\n%s\n", 
                HTTPVersionText[answ->version], 
                ResponseText[answ->status], 
                GetFileTypeHeader(answ->URL)
            );
    write(connfd, sendBuff, strlen(sendBuff));
    //\HEADER
    //BODY
    if ( answ->status == R_200_OK )
    {
        // get full path
        fullPath = malloc(strlen(rootFolder)+strlen(answ->URL)+1);
        strcpy(fullPath, rootFolder);
        strcat(fullPath, answ->URL);
        // open file and get it size
        FILE *f = fopen(fullPath, "rb");
        /*fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);*/

        char buff[BUFF_LEN];
        unsigned int tmpSize = 0;
        
        while(!feof(f)){
            tmpSize = fread(buff, 1, BUFF_LEN, f);
            write(connfd, buff, tmpSize);
        }        
        fclose(f);
    }
    //\BODY
}
/*******************************************************************************
@purpose: Proccess one connection after 'accept'
@input: connection handle
*******************************************************************************/
void procConn(char *IP, int connfd){
    char *recvBuff = 0;
    Request* req  = 0;
    Request* answ = 0;
    int i;
    do
    {
        #ifdef __FUNCTION_MODE__
            recvRequest(connfd, &recvBuff); // receive request
            req = parseRequest(recvBuff);   // parse them into structure
            answ= procRequest(req);         // generate answer
            logAccess(IP, req);             // write into log about request and answer
            //logAccess(IP, answ);             // write into log about request and answer
            sendAnswer(connfd, answ);       // send back answer
        #else
            handleRequest(connfd);
        #endif
        
    } while(0/*req->keepAlive != 0*/ /*&& timeDiff < KEEP_ALIVE_TIMEOUT*/);
    //close(connfd);
}

/*******************************************************************************
@purpose: Init listening of server 
@input: port to listen, maximum simultaneous connections
@return: listen socket handle
*******************************************************************************/
int startServer(char* interfaceToListen, int listenPort, int maxConnection){
    char ErrBuff[BUFF_LEN] = {0};
    int listenfd = 0;
    struct sockaddr_in serv_addr; 

    // create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( -1 == listenfd)
    {
        logError("Error. Can't open Socket!");
        close(listenfd);
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    #ifndef __RELEASE__
    // set socket opt - REUSEADDR
    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (-1 == status)
    {
        logError("Error. Can't set socket option REUSEADDR");
    }
    #endif

    // configure sockaddr_in
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = (
            interfaceToListen == 0 ? htonl(INADDR_ANY): inet_addr(interfaceToListen) 
        );
    serv_addr.sin_port = htons(listenPort); 

    // bind socket
    if ( -1 == bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) )
    {
        snprintf(ErrBuff, sizeof(ErrBuff), "Error. Can't BIND! %s", strerror(errno));
        logError(ErrBuff);
        close(listenfd);
        return -2;
    }

    // listen port
    if ( -1 == listen(listenfd, maxConnection) )
    {
        logError("Error. Can't LISTEN!");
        close(listenfd);
        return -3;
    }

    // print that server was started
    memset(ErrBuff, 0, sizeof(ErrBuff));
    snprintf(ErrBuff, sizeof(ErrBuff), 
            "Server started and listening up to %d simultaneous connections on:\n%s:%d", 
            maxConnection, interfaceToListen, listenPort);
    #ifndef __DEBUG_MODE__
    logError(ErrBuff);
    #endif
    printf("\n\n%s\n\n", ErrBuff);
    return listenfd;
}
#endif
