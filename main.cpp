/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

//////////Added ///////////////////
#include <iostream>
#include <fstream>

using namespace std;
///////////////////////

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 50 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
//The kernel supplies this value.  You can get IPV4 or IPV6.
//IPV4 example: 192.0.2.11
//IPV6 example: 2001:0db8:c9d2:aee5:73e3:934a:a5ae:9551
void *get_in_addr(struct sockaddr *sa)
{
    //If AF_INET == IPV4 then we are using protocol IPV4.
    if (sa->sa_family == AF_INET) {//sa_family: address family, of socket address.  IPV4 or IPV6?
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;   // listen on sock_fd
    char buf[MAXDATASIZE + 1];//Add 1 since the last char must be a null char.
    struct addrinfo hints, *servinfo, *p;
    /*
     * Used to prep the socket address structures for invention.
    struct addrinfo {
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname

        struct addrinfo *ai_next;      // linked list, next node
    };
    */

    int rv;
    char s[INET6_ADDRSTRLEN]; //Create a char array of size 46 (max length of ipv6 address).

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints); //Variable, value, size.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


    //addrinfo struct
    //getaddrinfo returns a pointer to a new linked list of these structures filled with info we need.
    /*
    int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                    const char *service,  // e.g. "http" or port number
                    const struct addrinfo *hints,
                    struct addrinfo **res);

       Translate name of a service location and/or a service name to set of
       socket addresses.

       This function is a possible cancellation point and therefore not
       marked with __THROW.
    */
    //Needs to return 0 to be successful.
    //Returns a pointer to a linked list.
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    //Grab the first socket that is available.
    for(p = servinfo; p != NULL; p = p->ai_next) {
        //Create a socket, set it to sockfd, if we can't assign to that location
        //we get -1 back.
        /*
         Create a new socket of type TYPE in domain DOMAIN, using
         protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
         Returns a file descriptor for the new socket, or -1 for errors.
         */
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        /* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
           For connectionless socket types, just set the default address to send to
           and the only address from which to accept transmissions.
           Return 0 on success, -1 for errors. */
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    //p is the serverinfo.
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }


    /* Convert a Internet address in binary network format for interface
       type AF in buffer starting at CP to presentation form and place
       result in buffer of length LEN astarting at BUF.  */
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);\


    /* Free `addrinfo' structure AI including associated storage.  */
    freeaddrinfo(servinfo); // all done with this structure

    /* Read N bytes into BUF from socket FD.
        Returns the number read or -1 for errors.*/
    /*if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv failed to read bytes into BUF from socket FD");
        exit(1);
    }

    buf[numbytes] = '\0'; //Clear last character of buf

    printf("client: received '%s'\n",buf);

    close(sockfd);*/

    ifstream ifs("/home/kristopher/Documents/TestTextFile/ZeroToNine.txt", ios::in | ios::ate);
    int fileLength = ifs.tellg();
    int numOfTransmissions = (fileLength % MAXDATASIZE == 0) ? fileLength / MAXDATASIZE : fileLength / MAXDATASIZE + 1;

    ifs.seekg(0, ios::beg);//Go back to beginning.
    if (ifs.good())
    {
        int overallCharLoc = 0;
        unsigned char *charArray = new unsigned char[MAXDATASIZE + 1];
        charArray[MAXDATASIZE] = '\0';
        for (int currTrans = 0; currTrans < numOfTransmissions; currTrans++)
        {
            char character;
            int charNum = 0;
            while(ifs.get(character) && charNum != MAXDATASIZE)
            {
                charArray[charNum++] = character;
            }
            if (send(sockfd, charArray, charNum, 0) == -1)
                perror("Sending failed.");
            sleep(2);

        }
        close(sockfd);
        exit(0);
    }


    /*
    if (send(sockfd, "", , 0) == -1)
        perror("send");
    close(sockfd);
    exit(0);
    */

    return 0;
}