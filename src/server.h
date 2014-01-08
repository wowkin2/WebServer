#ifndef __SERVER_H__
#define __SERVER_H__
//###################
#define __DEBUG_MODE__
#define __FUNCTION_MODE__
// #define __RELEASE__
//###################
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

#define RECV_TIMEOUT 10         // wait x-seconds any connections, else continue
#define KEEP_ALIVE_TIMEOUT 5    // wait x-seconds any requests to dedicated socket, else close socket

/**************************************  HTTP STATUS CODE  ********************/
const char *ResponseText[] = {
// 1xx: Informational
// 2xx: Success 
    "200 OK",              // correctly processed
// 3xx: Redirection
    "304 Not Modified",    // will send if client use 'If-Modified-Since' (will be used sometime)
// 4xx: Client Error
    "400 Bad Request",     // syntax error in request
    "403 Forbidden",       // access to this resource is denied
    "404 Not Found",       // no such file or resource
    "408 Request Timeout", // if there is no connection for a long time
    "414 Request-URL Too Long", // if request URL is too long, TODO: need to check in RFC
// 5xx: Server Error
    "500 Internal Server Error",    // other server error 
    "501 Method Not Implemented",   // unknown method (for current server all except GET)
    "503 Service Unavailable",      // server can't correctly handle requests
    "505 HTTP Version Not Supported"// unknown HTTP version number 
};
const int ResponseCode[] = {
    200,
    304, 
    400, 403, 404, 408, 414,
    500, 501, 503, 505
};
typedef enum _ResponseStatus{
    PROCESS_ERROR = -1,
    R_200_OK,
    R_304_NOT_MODIFIED,
    R_400_BAD_REQUEST,
    R_403_FORBIDDEN,
    R_404_NOT_FOUND,            
    R_408_TIMEOUT,
    R_414_TOO_LONG_URL,
    R_500_SERVER_ERROR,
    R_501_BAD_METHOD,
    R_503_SERVICE_UNAVAILABLE,
    R_505_BAD_HTTP_VERSION
}ResponseStatus;

/**************************************  METHODS  *****************************/
const char *RequestMethodText[] = {
    "GET",
    //"POST",
    //"..."
};
typedef enum _RequestMethod{
    METHOD_GET,
    //METHOD_POST,
    //METHOD_...,
}RequestMethod;

/**************************************  VERSIONS  ****************************/
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

/**************************************  REQUEST STRUCTURE  *******************/
typedef struct _Request
{
    RequestMethod method; // GET
    char* URL;            // /index.htm
    UCHAR version;        // HTTP/1.x
    ResponseStatus status;// OK
    
    void* lst;            // pointer to 'List' type (void because List is undefined here)
                          // key: value
                          // Host: localhost.com
                          // Connection: Keep-Alive/Closed
                          // Ranges: 
    //CRLF+CRLF
    char* data; //or
    char* filePath;
}Request;

/**************************************  MIME TYPES  **************************/
enum mime_types{
    HASH_HTM,
    HASH_HTML,
    HASH_CSS,
    HASH_JS,
    HASH_ICO,
    HASH_PNG,
    HASH_JPEG,
    HASH_JPG,
    HASH_GIF,
};
/******************************************************************************/
#endif
