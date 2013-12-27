#ifndef __SERVER_H__
#define __SERVER_H__

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

#include <sys/fcntl.h>

#include "my_aux.c"

#define ULI unsigned int
#define SLI signed int
#define UCHAR unsigned char

#define BUFF_LEN 1025
#define CHUNK_SIZE 1025

#define RECV_TIMEOUT 10
#define KEEP_ALIVE_TIMEOUT 5

const char *HTTP_Ver = "HTTP/1.1";
const char *ResponseText[] = {
    "200 OK",
    "400 Bad Request",
    "404 Not Found",
    "501 Method Not Implemented"
};
const int ResponseCode[] = {
    200,
    400,
    404,
    501
};
typedef enum _ResponseStatus{
    PROCESS_ERROR = -1,
    R_200_OK,
    R_400_BAD_REQUEST,
    R_404_NOT_FOUND,
    R_501_BAD_METHOD
}ResponseStatus;

const char *RequestMethodText[] = {
    "GET",
    //"POST",
    //"..."
};
typedef enum _RequestMethod{
    METHOD_GET,
    //METHOD_POST,
    //METHOD_...,l
}RequestMethod;

const char *HTTPVersionText[] = {
    "HTTP/0.9",
    "HTTP/1.0",
    "HTTP/1.1"
};
typedef enum _HTTPVersion{
    HTTP_V_0_9,
    HTTP_V_1_0,
    HTTP_V_1_1,
}HTTPVersion;
typedef struct _Request
{
    RequestMethod method; // GET
    char* URL;            // /index.htm
    UCHAR version;        // HTTP/1.x
    ResponseStatus status;// OK
    //char**variables;    // key: value
    char* host;           // Host: localhost.com
    UCHAR keepAlive;      // Connection: Keep-Alive
                          // Ranges
    //CRLF+CRLF
    char* data; //or
    char* filePath;
}Request;

#endif
