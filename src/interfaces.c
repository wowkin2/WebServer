#ifndef __INTERFACES_C__
#define __INTERFACES_C__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*int main(int argc, char *argv[])
{
    printInterfaces();
}*/

/*******************************************************************************
@purpose: to show available interfaces (ip-addresses)
@return: 0 if everything is ok
*******************************************************************************/
void printInterfaces()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        puts("Error in call getifaddrs().");
        return;
    }

    puts("\nAvailable interfaces:");
    
    /* Walk through linked list, maintaining head pointer so we
       can free list later */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        /* Display interface name and family (including symbolic
           form of the latter for the common families) */

        /*printf("%s  address family: %d%s\n",
                ifa->ifa_name, family,
                (family == AF_PACKET) ? " (AF_PACKET)" :
                (family == AF_INET) ?   " (AF_INET)" :
                (family == AF_INET6) ?  " (AF_INET6)" : "");
         */
        /* For an AF_INET* interface address, display the address */

        if (family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                          sizeof(struct sockaddr_in6),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return;
            }
            printf("\t%s\n", host);
        }
    }
    freeifaddrs(ifaddr);
    return;
}
#endif
