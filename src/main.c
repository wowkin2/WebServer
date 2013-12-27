#include <netinet/in.h> 
#include "server.c"

int main(int argc, char *argv[])
{
    struct sockaddr_in saddr;       // used for getting IP _
    socklen_t len = sizeof( saddr );// of incomming connection _
    char *addr_buf = malloc(INET_ADDRSTRLEN); // defined in <netinet/in.h> .

    int listenSocket = 0, connSocket = 0;
    long numb = 0;
    pid_t child_pid;
        
    listenSocket = startServer(80, 100);

    if (listenSocket > 0)
    {
        do
        {
            connSocket = accept( listenSocket, ( struct sockaddr* )&saddr, &len );
            if ( listenSocket < 0 ) {
                logError("Accept Error...");
                close(listenSocket);
                break;
            }

            printf("%lu %s\n", numb++, getTimeS());
            //procConn(connSocket);
            
            /* Fork a child process to handle the connection. */
            child_pid = fork ();
            if (child_pid == 0) 
            {
                /* This is the child process. It shouldn't use stdin or stdout, so close them. */
                close (STDIN_FILENO);
                //close (STDOUT_FILENO);
                /* Also this child process shouldn't do anything with the listening socket. */
                close (listenSocket);
                /* Handle a request from the connection. We have our own copy of the connected socket descriptor. */
                
// *********** GET IP *************** //
if ( inet_ntop( AF_INET, &saddr.sin_addr, addr_buf, INET_ADDRSTRLEN ) == NULL )
{ logError( "inet_ntop" ); exit( 1 ); }
//printf( "[%s:%d]\t", addr_buf, ntohs( saddr.sin_port ));
// ********************************** //
                procConn(addr_buf, connSocket);
                /* All done; close the connection socket, and end the child process. */
                close (connSocket);
                exit (0);
            }
            else if (child_pid > 0) 
            {
                /* This is the parent process. The child process handles the
                    connection, so we don't need our copy of the connected socket
                    descriptor. Close it. Then continue with the loop and
                    accept another connection. */
                close (connSocket);
                /*if (getComm() == 0) //DEBUG FEATURE
                {
                    break;
                }*/
            }
            else
                /* Call to fork failed. */
                logError("Can't fork");
        } while(1);
    }
    close(listenSocket);
    logError("#Server closed Successfully!");
    printf("\n\nServer closed Successfully!\n\n");
    
    return 0;
}
