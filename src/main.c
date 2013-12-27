#include <netinet/in.h> 
#include "server.c"
#include "params.c"

static FILE* fileLogAccess;
static FILE* fileLogError;
/*********************
@purpose: open file for appending in next functions
@input: full path and file name
@return: pointer to file
*********************/
FILE* openFile(char *filePath)
{
    return fopen(filePath, "a");
}


/*********************
@purpose: START POINT
*********************/
int main(int argc, char *argv[])
{
    char ErrBuff[256] = {0};
    int wasStarted = 0;
    FILE* fileLogTmp;
    
    //##############  CONFIGURATE  ###############
    
    // Read parameters from 'conf/httpd.conf' to list
    List *lstConf = initList();
    if (0 != getParamsFromFile("conf/httpd.conf", lstConf, '='))
    {    
        puts("Could not start server...");
        return 1;
    }
    // Apply parameters from list (or default, if they are absent)
    int port    = getParamInt(lstConf, "portListen",    80);
    int maxConn = getParamInt(lstConf, "maxConnection", 100);
    char *logAccessPath   = getParamS(lstConf, "logAccess",   "log/access.log");// maybe 
    char *logErrorPath    = getParamS(lstConf, "logError",    "log/error.log"); //  need 
    char *defaultPage = getParamS(lstConf, "defaultPage", "index.htm");         //  to copy 
    char *rootFolder  = getParamS(lstConf, "rootFolder",  "www/");              //  values
    
    // Open new log files
    if ( !( fileLogAccess = openFile(logAccessPath) ) )
    {
        snprintf(ErrBuff, sizeof(ErrBuff), "Could not open '%s'!", logAccessPath);
        puts(ErrBuff);
        logError(ErrBuff);
    }
    if ( !( fileLogError  = openFile(logErrorPath) ) )
    {
        snprintf(ErrBuff, sizeof(ErrBuff), "Could not open '%s'!", logErrorPath);
        puts(ErrBuff);
        logError(ErrBuff);
    }
    // Delete list with 'conf/httpd.conf' from memory
    removeAll(lstConf);
    
    //#############  START SERVER  ###############

    struct sockaddr_in saddr;       // used for getting IP _
    socklen_t len = sizeof( saddr );// of incomming connection _
    char *addr_buf = malloc(INET_ADDRSTRLEN); // defined in <netinet/in.h> .

    int listenSocket = 0, connSocket = 0;
    pid_t child_pid;
    
    // Try to start server
    listenSocket = startServer(port, maxConn);

    if (listenSocket > 0)
    {
        wasStarted = 1;
        logError("#Server started Successfully!");
        printf("\n\nServer started Successfully!\n\n");
        //#############  MAIN PROCESSING LOOP  ###############
        while(1)
        {
            connSocket = accept( listenSocket, ( struct sockaddr* )&saddr, &len );
            if ( listenSocket < 0 ) {
                logError("Accept Error...");
                close(listenSocket);
                break;
            }
            
            child_pid = fork ();
            if (child_pid == 0)     //####  child thread  ####
            {
                //close (STDIN_FILENO);     //child don't need to print anything,
                //close (STDOUT_FILENO);    //  so close input/output streams
                
                close (listenSocket);       //close a child-copy of listening port
                
//TODO
// *********** GET IP *************** // 
if ( inet_ntop( AF_INET, &saddr.sin_addr, addr_buf, INET_ADDRSTRLEN ) == NULL )
{ logError( "inet_ntop" ); exit( 1 ); }
//printf( "[%s:%d]\t", addr_buf, ntohs( saddr.sin_port ));
// ********************************** //
                procConn(addr_buf, connSocket);
                
                close (connSocket);     //close processed connection
                exit (0);       // close child process
            }
            else if (child_pid > 0) // ####  parent thread  ####
            {
                
                close (connSocket);
                /*if (getComm() == 0) //DEBUG FEATURE
                {
                    break;
                }*/
            }
            else
                logError("Can't fork");     // if can't create child
        }
    }
    close(listenSocket);    // close parent listenSocket
    if (wasStarted)
    {
        logError("#Server closed Successfully!");
        printf("\n\nServer closed Successfully!\n\n");
    }
    else
    {
        logError("#Server closed without starting!");
        printf("\n\nServer closed without starting!\n\n");
        return 1;
    }
    return 0;
}
