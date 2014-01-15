#ifndef __SERVER_C__
#define  __SERVER_C__
#include "server.h"
#include "log.c"
#include "params.c"

static char *defaultPage = 0;
static char *rootFolder = 0;

/*******************************************************************************
@purpose: create full path concating 'rootFolder' and 'someURL'
@input: relative of www-folder path 
@output: absolute file path
*******************************************************************************/
inline char *getFullPath(char *URL)
{
    char *tmp = malloc(strlen(rootFolder)+strlen(URL)+1);
    strcpy(tmp, rootFolder);
    strcat(tmp, URL);
    return tmp;
}
/*******************************************************************************
@purpose: generate standard error page
@input: error code
@output: string with HTML-page
*******************************************************************************/
inline char *getErrorPage(ResponseStatus status)
{
    char buff[BUFF_LEN] = {0};
    char errTemplate[] = "<!DOCTYPE html>"
                          "<html><head><title> %s </title></head>"
                          "<body><h1 align=\"center\">%s</h1></body>"
                          "</html>";
    snprintf(buff, sizeof(buff), 
            errTemplate, 
            ResponseText[status], 
            ResponseText[status]);
    char *ptrRet = malloc(strlen(buff)+1);
    strcpy(ptrRet, buff);
    return ptrRet;
}
/*******************************************************************************
@purpose: get mime type by file extension
@input: file path
@return: string with MIME-type
*******************************************************************************/
char* getMimeType(char *URL)
{
    char *pStart = strchr(URL,'.');
    // if not found '.' (extension delimiter) so send default
    if (pStart == NULL)
        return mimeText[HASH_DEFAULT];
    
    // get hash of extension
    register
    unsigned long hash = getHash(pStart+1);   // +1 to skip '.'

    // find suitable and return it
    register int i;
    for(i = 0; i < HASH_LAST; i++)
    {
        if (mimeHash[i] == hash)
            return mimeText[i];
    }
    // if not found - return default
    return mimeText[HASH_DEFAULT];
}

/*******************************************************************************
@purpose: generate header for response
@input: URL
@return: ready header for appending to response
*******************************************************************************/
char* getHeader(Request *req)
{
    char buff[BUFF_LEN] = {0};
    char *fullPath = getFullPath(req->URL);
        
                
    char len_buff[50] = {0};

    if (req->version != HTTP_V_1_1)
    {
        snprintf(len_buff, sizeof(buff), "Content-Length: %lu", getFileSize(fullPath));
    }
    else
    {
        strcpy(len_buff, "Transfer-Encoding: chunked");
    }
    
    snprintf(buff, sizeof(buff), 
                "Allow: GET\n"
                "Content-Type: %s\n"
                "Connection: %s\n"
                "%s\n",
                getMimeType(fullPath),
                getParamS(req->lst, "Connection", "close"),
                len_buff // Length or Chunked
            );

    char *ptr = malloc(strlen(buff) + 1);
    strcpy(ptr, buff);
    return ptr;
}
/*******************************************************************************
@purpose: Free allocated data for request structure
@input: pointer to structure
*******************************************************************************/
void freeRequest(Request *rq)
{
    // TODO handle next commented line
    //if (rq->URL != NULL) free(rq->URL);
    removeAll(rq->lst);
}

/*******************************************************************************
@purpose: start to send file
@input: path to file, socket where to send
*******************************************************************************/
long sendFileChunked(char *fullPath, int connfd)
{
    char sendBuff[BUFF_LEN] = {0};
    long retSize = 0;
    puts(fullPath);
    FILE *f = fopen(fullPath, "rb");
    if (f != NULL)
    {
        char data_buff[BUFF_LEN] = {0};
        char response_buff[BUFF_LEN+20] = {0};
        unsigned int dataSize = 0;
        
        while(!feof(f)){
            dataSize = fread(data_buff, 1, BUFF_LEN, f);
            snprintf(response_buff, sizeof(response_buff), "%X\r\n", dataSize);

            int numbOffset = strlen(response_buff);
            memcpy(response_buff + numbOffset, data_buff, dataSize);
            memcpy(response_buff + numbOffset + dataSize, "\r\n", 2);
            retSize +=  write(connfd, response_buff, dataSize + numbOffset + 2); //len of CRLF == 2
        }
        retSize +=  write(connfd, "0\r\n\r\n", 5);
        fclose(f);
        return retSize;
    }
    return -1;
}
/*******************************************************************************
@purpose: start to send file
@input: path to file, socket where to send
*******************************************************************************/
long sendFile(char *fullPath, int connfd)
{
    char sendBuff[BUFF_LEN] = {0};
    long retSize = 0;
    FILE *f = fopen(fullPath, "rb");
    if (f != NULL)
    {
        char buff[BUFF_LEN];
        unsigned int tmpSize = 0;
        
        while(!feof(f)){
            tmpSize  =  fread(buff, 1, BUFF_LEN, f);
            retSize +=  write(connfd, buff, tmpSize);
        }
        fclose(f);
        return retSize;
    }
    return -1;
}

#ifndef __FUNCTION_MODE__
void handleRequest(char *IP, int connfd){
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
    if (req->status == R_200_OK)
    {
        // Parse request to separate fields
        char method[255];
        char url[255];
        char version[255];

        // count parsed values (must be 3)
        if ( 3 == sscanf (recvBuff, "%s %s %s", method, url, version))
        {
            if (strcmp(method, "GET") == 0) // Check if method is supported
            {
                req->method = METHOD_GET;
                
                // Check HTTP version
                if (strcmp(version, HTTPVersionText[HTTP_V_1_0]) == 0)
                {
                    req->version = HTTP_V_1_0;
                    req->status = R_200_OK;
                }
                else if (strcmp(version, HTTPVersionText[HTTP_V_1_1]) == 0) 
                {
                    req->version = HTTP_V_1_1;
                    req->status = R_200_OK;
                }
                else
                {
                    req->status = R_505_BAD_HTTP_VERSION;
                }
                // Parse other parts of request
                if (req->status == R_200_OK)
                {
                    if (strlen(url) < 1)
                    {
                        
                    }
                    // Check URL length
                    if (strlen(url) > MAX_URL_LEN)
                    {
                        req->status = R_414_TOO_LONG_URL;
                    }
                    else 
                    {
                        // Remove excessive parameters from URL
                        char *pStart, *pEnd;
                        pEnd   = strrchr(url,'#');
                        if (pEnd != NULL) *pEnd = '\0';
                        pEnd   = strrchr(url,'?');
                        if (pEnd != NULL) *pEnd = '\0';
                        
                        /*pStart = strrchr(url,'/');
                        if (pStart == NULL)          // if not found '/'*/
                            pStart = url;               
                         
                        // COPY URL
                        req->URL = malloc(strlen(pStart));
                        strcpy(req->URL, pStart);

                        // parse rest parameters('key: value\n')
                        req->lst = initList();
                        getParamsFromString( strchr(recvBuff, '\n')+1, req->lst, ':');
                        //printAll(req->lst);
                    }
                }
                else
                {
                    req->URL = NULL;
                }
            }
            // Now only GET is supporting
            else
            {
                req->status = R_501_BAD_METHOD;
            }
        }
        else
        {
            req->status = R_400_BAD_REQUEST;
        }
    }
#ifdef __FUNCTION_MODE__
    return req;
}
/*******************************************************************************
@purpose: Proccess one request to generate answer
@input: pointer to request structure
@return: answer to be sent
*******************************************************************************/
Request* procRequest(Request* req){
#endif
    Request *answ = malloc(sizeof(Request));
    memset(answ, 0, sizeof(Request));   // here: 'answ->status' is set to 'R_200_OK'
    
    if (req == NULL) answ->status = R_500_SERVER_ERROR;
    else if (req->status == PROCESS_ERROR) answ->status = R_500_SERVER_ERROR;
    
    //check URL
    else if (req->URL == NULL) answ->status = R_500_SERVER_ERROR;

    answ->version = req->version;
    if ( answ->status == R_200_OK ) // R_200_OK was set as default value, can be changed on errors
    {
        // change page to default if host root is requested
        char *tmp = defaultPage;
        if (req->URL[strlen(req->URL)-1] ==  '/') // TODO check if URL is folder
        {
            strcat(req->URL, defaultPage+1);
        }
        answ->URL = malloc(strlen(req->URL));
        strcpy(answ->URL, req->URL+1);

        // check if file is found
        char *fullPath = malloc(strlen(rootFolder)+strlen(answ->URL)+1);
        strcpy(fullPath, rootFolder);
        strcat(fullPath, answ->URL);
        /*if(isFolder(fullPath) == 1) // directory but not with '/' at the end
            {
                char *referer = getParamS(answ->lst, "Referer", "a");
                puts(referer);
                if (strcmp(referer, "a") != 0)
                {
                   strcat(answ->URL, referer+1);
                }
                strcat(answ->URL, defaultPage);
                answ->status = R_404_NOT_FOUND; // TODO fix this branch
        }
        else */
        if ( fileExists(fullPath) != 0)
        {
            answ->status = R_404_NOT_FOUND;
        }

        // check if client has permissions to file
        if (0 /* TODO Security */)
        {
            answ->status = R_403_FORBIDDEN;
        }
        // copy pointer to list of rest parameters
        answ->lst = req->lst;
    }
    else
    {
        answ->URL = NULL;
    }
#ifdef __FUNCTION_MODE__
    return answ;

}
/*******************************************************************************
@purpose: send answer chunk by chunk (if needed)
@input: connection handle, answer block
*******************************************************************************/
long sendAnswer(int connfd, Request* answ){
#endif
    char sendBuff[BUFF_LEN] = {0};
    long retSize = 0;
    char errorFlag = 0;
    
    // Check if answer was generated
    if (answ == NULL) errorFlag = 1;
    else if (answ->URL == NULL) answ->status = R_500_SERVER_ERROR;

    if (errorFlag != 0)
    {
        snprintf(sendBuff, sizeof(sendBuff), 
                    "%s %s\n\n", 
                    HTTPVersionText[1], 
                    ResponseText[R_500_SERVER_ERROR]
                );
        return write(connfd, sendBuff, strlen(sendBuff));
    }
    //HEADER
    char *header = getHeader(answ);
    snprintf(sendBuff, sizeof(sendBuff), 
                "%s %s\n%s\n", 
                HTTPVersionText[answ->version], 
                ResponseText[answ->status], 
                header
            );
    retSize += write(connfd, sendBuff, strlen(sendBuff));

    //BODY
    char *fullPath = 0;
    // Send requested file
    if ( answ->status == R_200_OK )
    {
        fullPath = malloc(strlen(rootFolder)+strlen(answ->URL)+1);
        strcpy(fullPath, rootFolder);
        strcat(fullPath, answ->URL);

        if (answ->version != HTTP_V_1_1)
        {
             retSize += sendFile(fullPath, connfd);
        }
        else
        {
             retSize += sendFileChunked(fullPath, connfd);
        }
    }
    else 
    {
        // Need to send any HTML-page for not-IE-browsers, 
        // even if send error code like 404
        char *errPage = getErrorPage(answ->status);
        if (answ->version != HTTP_V_1_1)
        {
             retSize += write(connfd, errPage, strlen(errPage));
        }
        else
        {
            char response_buff[BUFF_LEN] = {0};
            snprintf(response_buff, sizeof(response_buff), 
                  "%X\r\n%s\r\n0\r\n\r\n", strlen(errPage), errPage);
            
            retSize += write(connfd, response_buff, strlen(response_buff));
        }

    }

#ifndef __FUNCTION_MODE__
    logAccess(IP, req, retSize);
#endif
}
/*******************************************************************************
@purpose: Proccess one connection after 'accept'
@input: connection handle
*******************************************************************************/
void procConn(char *IP, int connfd){
    char *recvBuff = 0;
    Request* req  = 0;
    Request* answ = 0;
    UCHAR keepAlive = -1;
    
    int i;
    do
    {
        #ifdef __FUNCTION_MODE__
            recvRequest(connfd, &recvBuff);              // RECEIVE
            req = parseRequest(recvBuff);                // PARSE (into structure)
            answ= procRequest(req);                      // PROCCESS (generate response)
            long responseSize = sendAnswer(connfd, answ);// SEND RESPONSE
            logAccess(IP, req, responseSize);            // LOG REQUEST

            // free resources
            free(recvBuff);
            removeAll(req->lst);
            freeRequest(req);
            freeRequest(answ);
        #else
            handleRequest(IP, connfd);
        #endif
    } while(wait4Socket(connfd, KEEP_ALIVE_TIMEOUT) > 0);
    close(connfd);
}

/*******************************************************************************
@purpose: wait 20 seconds for changes in socket
@input: socket 
@return: -1 on error; 0 on timeout; else if some data has comming
*******************************************************************************/
int wait4Socket(int listenSocket, int time)
{
    fd_set set;
    struct timeval timeout;
    int rv;
    FD_ZERO(&set); // clear the set 
    FD_SET(listenSocket, &set); // add our file descriptor to the set

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    rv = select(listenSocket + 1, &set, NULL, NULL, &timeout);
    if(rv == -1) logError("Problem with 'select' near accept");
    return rv;
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
