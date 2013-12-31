// __DEBUG_MODE__ is defined in "server.h"

#include <netinet/in.h> 
#include "server.c"
#include "params.c"

extern char *defaultPage;
extern char *rootFolder;

/*******************************************************************************
@purpose: START POINT
*******************************************************************************/
int main(int argc, char *argv[])
{
    char ErrBuff[256] = {0};
    FILE* fileLogTmp;
    //##############  PRINT USAGE  ###############
    //--i check available interfaces
    //--t check configuration
    //--k start/stop/restart
    //
    
    
    //##############  CONFIGURATE SERVER  ###############
    
    // Read parameters from 'conf/httpd.conf' to list
    List *lstConf = initList();
    if (0 != getParamsFromFile("conf/httpd.conf", lstConf, '='))
    {
        puts("Could not read configuration file...");
        return 1;
    }
    // Apply parameters from list (or set to default, if they are absent)
    int port          = getParamInt(lstConf, "portListen",    80);
    int maxConn       = getParamInt(lstConf, "maxConnection", 100);
    char *logAccessPath = getParamS(lstConf, "logAccess",    "log/access.log");// maybe 
    char *logErrorPath  = getParamS(lstConf, "logError",     "log/error.log"); //  need 
    defaultPage         = getParamS(lstConf, "defaultPage",  "/index.htm");    //  to copy 
    rootFolder          = getParamS(lstConf, "rootFolder",   "www/");          //  values
    char *interface     = getParamS(lstConf, "interfaceToListen", "");
    // Open new log files
    if ( initErrorLog(logErrorPath) == 0 )
    {
        return 1;
    }
    if ( !( fileLogAccess = fopen(logAccessPath, "a") ) )
    {
        snprintf(ErrBuff, sizeof(ErrBuff), "Could not open '%s'!", logAccessPath);
        puts(ErrBuff);
        logError(ErrBuff);
        return 1;
    }
    removeAll(lstConf);     // Free data with configuration from memory
    
    //#############  START SERVER  ###############

    struct sockaddr_in saddr;       // used for getting IP _
    socklen_t len = sizeof( saddr );// of incomming connection _
    char addr_buf[INET_ADDRSTRLEN]; // defined in <netinet/in.h> .

    int listenSocket = 0, connSocket = 0;
    pid_t child_pid;
    
    // Try to start server
    listenSocket = startServer(interface, port, maxConn);

    if (listenSocket > 0)
    {
        #ifndef __DEBUG_MODE__
        logError("#Server started Successfully!");
        #endif
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
                #ifndef __DEBUG_MODE__
                close (STDIN_FILENO);     //child don't need to print anything,
                close (STDOUT_FILENO);    //  so close input/output streams
                #endif
                close (listenSocket);       //close a child-copy of listening port
                
                //TODO need to check
                if ( inet_ntop( AF_INET, &saddr.sin_addr, addr_buf, INET_ADDRSTRLEN ) == NULL )
                {
                    logError("Can't convert IP addr"); 
                    exit(1); 
                }
                procConn(addr_buf, connSocket);
                
                exit (0);       // close child process
            }
            else if (child_pid > 0) // ####  parent thread  ####
            {
                
                close (connSocket);
                #ifdef __DEBUG_MODE__
                if (getComm() == 0) //DEBUG FEATURE
                {
                    break;
                }
                #endif
            }
            else
                logError("Can't fork");     // if can't create child
        }
        close(listenSocket);    // close parent listenSocket


        #ifndef __DEBUG_MODE__
        logError("#Server closed Successfully!");
        #endif
        printf("\n\nServer closed Successfully!\n\n");
        
        return 0;
    }
    logError("#Server was not started!");
    printf("\n\nServer was not started!\n\n");
    return 1;
}
