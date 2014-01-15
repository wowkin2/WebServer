// __DEBUG_MODE__ is defined in "server.h"

#include <netinet/in.h> 
#include "server.c"
#include "params.c"
#include "interfaces.c"
extern char *defaultPage;
extern char *rootFolder;

void printInterfaces();
void checkConfiguration();
int InitServer();
/*******************************************************************************
@purpose: START POINT
*******************************************************************************/
int main(int argc, char *argv[])
{
    switch(getStartupParams(argc, argv))
    {
    case SHOW_INTERFACES:
            printInterfaces();
            break;
    case SERVER_START:
            InitServer();
            break;
   /*case SERVER_STOP:      //TODO
            break;
    case SERVER_RESTART:
            break;*/
    case CHECK_CONFIG:
            checkConfiguration();
            break;
        case SHOW_HELP:
        case SHOW_HELP_LONG:
    default:
            printf("\tUsage:\n"
                   "\t--i    get available interfaces\n"
                   "\t--t    check configuration\n"
                   "\t--k    to start webserver\n"
                   "\t--h    print this text\n");
            return 0;
    }
}
void checkConfiguration(){
    List *lstConf = initList();
    if (0 != getParamsFromFile("conf/httpd.conf", lstConf, '='))
    {
        puts("Could not read configuration file...");
    }else{
        puts("Configuration file 'conf/httpd.conf' is OK.");
    }
    removeAll(lstConf);
}
int InitServer()
{
    char ErrBuff[256] = {0};
    FILE* fileLogTmp;
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
    char *logAccessPath = getParamS(lstConf, "logAccess",    "log/access.log");
    char *logErrorPath  = getParamS(lstConf, "logError",     "log/error.log");
    defaultPage         = getParamS(lstConf, "defaultPage",  "/index.htm");
    rootFolder          = getParamS(lstConf, "rootFolder",   "www/");
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
    // Check if default page exists
    char *fullPath = malloc(strlen(rootFolder)+strlen(defaultPage)+1);
    strcpy(fullPath, rootFolder);
    strcat(fullPath, defaultPage);
    if (fileExists(fullPath) != 0)
    {
        snprintf(ErrBuff, sizeof(ErrBuff), 
            "Could not find default page following next path: '%s'!",
            fullPath);
        puts(ErrBuff);
        logError(ErrBuff);
    }
    // Free data with configuration from memory
    removeAll(lstConf);
    
    //#############  START SERVER  ###############

    struct sockaddr_in saddr;       // used for getting IP _
    socklen_t len = sizeof( saddr );// of incomming connection _
    char IP_Buff[INET_ADDRSTRLEN]; // defined in <netinet/in.h> .

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
            // Wait few second, if there's no connections - continue
            if (wait4Socket(listenSocket, ACCEPT_TIMEOUT) > 0)
            {
                connSocket = accept( listenSocket, ( struct sockaddr* )&saddr, &len );
                if ( listenSocket < 0 ) {
                    logError("Accept Error...");
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
                    
                    if ( inet_ntop( AF_INET, &saddr.sin_addr, IP_Buff, INET_ADDRSTRLEN ) == NULL )
                    {
                        logError("Can't convert IP addr");
                        close(connSocket);
                        exit(1);
                    }
                    procConn(IP_Buff, connSocket);
                    
                    exit (0);       // close child process
                }
                else if (child_pid > 0) // ####  parent thread  ####
                {
                    
                    close (connSocket);
                    #ifdef __DEBUG_MODE__
                    if (getComm() == 0)
                    {
                        break;
                    }
                    #endif
                }
                else
                    logError("Can't fork");     // if can't create child
            }
        }
        close(listenSocket);    // close parent listenSocket


        #ifndef __DEBUG_MODE__
        logError("#Server closed Successfully!");
        #endif
        printf("\n\nServer closed Successfully!\n\n");
        
        return 0;
    }
    logError("#Server was not started!");
    printf("\n\nServer was not started! See error.log to get more information!\n\n");
    return 1;
}
