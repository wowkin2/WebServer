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

#define CHUNK_SIZE 1025         // size of one chunk
#define ACCEPT_TIMEOUT 10       // wait x-seconds any connections, else continue
#define KEEP_ALIVE_TIMEOUT 5    // wait x-seconds any requests to dedicated socket, else close socket
#define MAX_URL_LEN 255         // max size of url (err.code 414)
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
    UCHAR version;        // HTTP/x.x
    ResponseStatus status;// 200 OK
    
    void* lst;            // pointer to 'List' type (void because List is undefined here)
                          // key: value
                          // Host: localhost.com
                          // Connection: Keep-Alive/closed
                          // Ranges: 
    //CRLF+CRLF
    char* data;        //some data
}Request;

/**************************************  MIME TYPES  **************************/
// TODO dynamic reading them from file
char *mimeText[] = {
    "text/plain",
    "application/msword",
    "application/pdf",
    "application/vnd.ms-excel",
    "application/vnd.ms-powerpoint",
    "application/x-javascript",
    "application/x-shockwave-flash",
    "application/xml",
    "application/zip",
    "audio/mpeg",
    "audio/x-mpegurl",
    "audio/x-wav",
    "image/bmp",
    "image/gif",
    "image/jpeg",
    "image/jpeg",
    "image/png",
    "image/svg+xml",
    "image/tiff",
    "image/tiff",
    "image/vnd.djvu",
    "image/vnd.djvu",
    "image/x-icon",
    "text/css",
    "text/html",
    "text/html",
    "text/rtf",
    //last
};
unsigned long mimeHash[] = {
    0,
    193489659,
    193502367,
    193511356,
    193502777,
      5863522,
    193506261,
    193511382,
    193513432,
    193499445,
    193497498,
    193509907,
    193487428,
    193492731,
   2090408331,
    193496230,
    193502698,
    193506229,
   2090760110,
    193506888,
   2090186750,
    193489513,
    193494720,
    193488718,
   2090341082,
    193494190,
    193505073,
    //0
};
enum mimeType{
    HASH_DEFAULT,
    HASH_DOC,
    HASH_PDF,
    HASH_XLS,
    HASH_PPT,
    HASH_JS,
    HASH_SWF,
    HASH_XML,
    HASH_ZIP,
    HASH_MP3,
    HASH_M3U,
    HASH_WAV,
    HASH_BMP,
    HASH_GIF,
    HASH_JPEG,
    HASH_JPG,
    HASH_PNG,
    HASH_SVG,
    HASH_TIFF,
    HASH_TIF,
    HASH_DJVU,
    HASH_DJV,
    HASH_ICO,
    HASH_CSS,
    HASH_HTML,
    HASH_HTM,
    HASH_RTF,
    
    // used for loop processing
    HASH_LAST
};
/******************************************************************************/
#endif
