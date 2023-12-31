

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#define FLIGHT_NUM_SIZE            15

#define SERVER_PORT                "8080"
#define STORE_FLIGHT               1
#define FLIGHT_TIME_STORED         2
#define FLIGHT_TIME                3
#define FLIGHT_TIME_RESULT         4
#define FLIGHT_NOT_FOUND           5
#define ERROR_IN_INPUT             9

#define BACKLOG                   10
#define NUM_FDS                    5

#define MAX_EVENTS                10

void error (char *msg);

unsigned int min(unsigned int a, unsigned int b){
    if(a<b) return a;
    return b;
}

unsigned int fact(unsigned int n){
    unsigned int i=1;
    for(unsigned int j=2;j<=min(n,20);j++){
        i=i*j;
    }
    return i;
}

struct message {
    int32_t message_id;
    char flight_no [FLIGHT_NUM_SIZE + 1];
    char departure [1 + 1]; // 'D': departure, 'A': arrival
    char date [10 + 1]; // dd/mm/yyyy
    char time [5 + 1];   // hh:mm
};

struct tnode {
    char *flight_no;
    bool departure; // true: departure, false: arrival
    time_t flight_time;
    struct tnode *left;
    struct tnode *right;
};

struct message recv_message, send_message;

struct tnode *add_to_tree (struct tnode *p, char *flight_no, bool departure, time_t flight_time);
struct tnode *find_flight_rec (struct tnode *p, char *flight_no);
void print_tree (struct tnode *p);
void trim (char *dest, char *src); 
void error (char *msg);

int main (int argc, char **argv)
{
    const char * const ident = "flight-time-server";

    openlog (ident, LOG_CONS | LOG_PID | LOG_PERROR, LOG_USER);
    syslog (LOG_USER | LOG_INFO, "%s", "Hello world!");
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_PASSIVE;    /* for wildcard IP address */

    struct addrinfo *result;
    int s; 
    if ((s = getaddrinfo (NULL, SERVER_PORT, &hints, &result)) != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        exit (EXIT_FAILURE);
    }

    /* Scan through the list of address structures returned by 
       getaddrinfo. Stop when the the socket and bind calls are successful. */

    int listener, optval = 1;
    socklen_t length;
    struct addrinfo *rptr;
    for (rptr = result; rptr != NULL; rptr = rptr -> ai_next) {
        listener = socket (rptr -> ai_family, rptr -> ai_socktype,
                       rptr -> ai_protocol);
        if (listener == -1)
            continue;

        if (setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
            error("setsockopt");

        if (bind (listener, rptr -> ai_addr, rptr -> ai_addrlen) == 0)  // Success
            break;

        if (close (listener) == -1)
            error ("close");
    }

    if (rptr == NULL) {               // Not successful with any address
        fprintf(stderr, "Not able to bind\n");
        exit (EXIT_FAILURE);
    }

    freeaddrinfo (result);

    // Mark socket for accepting incoming connections using accept
    if (listen (listener, BACKLOG) == -1)
        error ("listen");

    int efd;
    if ((efd = epoll_create1 (0)) == -1)
	error ("epoll_create1");
    struct epoll_event ev, ep_event [MAX_EVENTS];

    ev.events = EPOLLIN;
    ev.data.fd = listener;
    if (epoll_ctl (efd, EPOLL_CTL_ADD, listener, &ev) == -1)
	error ("epoll_ctl");

    int nfds = 0;

    socklen_t addrlen;
    struct sockaddr_storage client_saddr;
    char str [INET6_ADDRSTRLEN];
    struct sockaddr_in  *ptr;
    struct sockaddr_in6  *ptr1;
    struct tnode *root = NULL;

    while (1) {
        // monitor readfds for readiness for reading
	if ((nfds = epoll_wait (efd, ep_event, MAX_EVENTS,  -1)) == -1)
	    error ("epoll_wait");

        // Some sockets are ready. Examine readfds
        for (int i = 0; i < nfds; i++) {

	    if 	((ep_event [i].events & EPOLLIN) == EPOLLIN) {
                if (ep_event [i].data.fd == listener) {  // request for new connection
                    addrlen = sizeof (struct sockaddr_storage);
                    int fd_new;
                    if ((fd_new = accept (listener, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
                        error ("accept");
                    // add fd_new to epoll
                    ev.events = EPOLLIN;
                    ev.data.fd = fd_new;
                    if (epoll_ctl (efd, EPOLL_CTL_ADD, fd_new, &ev) == -1)
	                error ("epoll_ctl");
	
                    // print IP address of the new client
                    if (client_saddr.ss_family == AF_INET) {
                        ptr = (struct sockaddr_in *) &client_saddr;
                        inet_ntop (AF_INET, &(ptr -> sin_addr), str, sizeof (str));
                    }
                    else if (client_saddr.ss_family == AF_INET6) {
                        ptr1 = (struct sockaddr_in6 *) &client_saddr;
	                inet_ntop (AF_INET6, &(ptr1 -> sin6_addr), str, sizeof (str));
                    }
                    else
                    {
                        ptr = NULL;
                        fprintf (stderr, "Address family is neither AF_INET nor AF_INET6\n");
                    }
                    if (ptr) 
                        syslog (LOG_USER | LOG_INFO, "%s %s", "Connection from client", str);
                
                }
                else  // data from an existing connection, receive it
                {
                	char buffer[1024];
                    memset (&recv_message, '\0', sizeof (struct message));
                    ssize_t numbytes = recv (ep_event [i].data.fd, buffer, sizeof(buffer), 0);
   
                    if (numbytes == -1)
                        error ("recv");
                    else if (numbytes == 0) {
                        // connection closed by client
                        fprintf (stderr, "Socket %d closed by client\n", ep_event [i].data.fd);
			// delete fd from epoll
                        if (epoll_ctl (efd, EPOLL_CTL_DEL, ep_event [i].data.fd, &ev) == -1)
	                    error ("epoll_ctl");
                        if (close (ep_event [i].data.fd) == -1)
                            error ("close");
                    }
                    else 
                    {
                        // data from client
                        buffer[numbytes]='\0';
                    	unsigned int num=atoi(buffer);
                    	unsigned int ans=fact(num);
            		unsigned int ans2=ans,l=0;
            		while(ans2>0){
                		ans2=ans2/10;
                		l++;
            		}
		    char charArray[l+1];
		    sprintf(charArray, "%u", ans);
		    charArray[l]='\0';
                        bool valid;
                        char temp_buf [FLIGHT_NUM_SIZE + 1];

                           if (send (ep_event [i].data.fd, charArray, sizeof(charArray), 0) == -1)
                               error ("send");
                    }
                }
            } // if (fd == ...
        } // for
    } // while (1)

    exit (EXIT_SUCCESS);
} // main

// record the flight departure / arrival time    
struct tnode *add_to_tree (struct tnode *p, char *flight_no, bool departure, time_t flight_time)
{
    int res;

    if (p == NULL) {  // new entry
        if ((p = (struct tnode *) malloc (sizeof (struct tnode))) == NULL)
            error ("malloc");
        p -> flight_no = strdup (flight_no);
        p -> departure = departure;
        p -> flight_time = flight_time;
        p -> left = p -> right = NULL;
    }
    else if ((res = strcmp (flight_no, p -> flight_no)) == 0) { // entry exists
        p -> departure = departure;
        p -> flight_time = flight_time;
    }
    else if (res < 0) // less than flight_no for this node, put in left subtree
        p -> left = add_to_tree (p -> left, flight_no, departure, flight_time);
    else   // greater than flight_no for this node, put in right subtree
        p -> right = add_to_tree (p -> right, flight_no, departure, flight_time);
    return p;
}

// find node for the flight for which departure or arrival time is queried
struct tnode *find_flight_rec (struct tnode *p, char *flight_no)
{
    int res;

    if (!p) 
        return p;
    res = strcmp (flight_no, p -> flight_no);
    
    if (!res)
        return p;

    if (res < 0)
        return find_flight_rec (p -> left, flight_no);
    else 
        return find_flight_rec (p -> right, flight_no);
}

// print_tree: print the tree (in-order traversal)
void print_tree (struct tnode *p)
{
    if (p != NULL) {
        print_tree (p -> left);
        printf ("%s: %d %s\n\n", p -> flight_no, (int) p -> departure, ctime (&(p -> flight_time)));
        print_tree (p -> right);
    }
}

void error (char *msg)
{
    perror (msg);
    exit (1);
}

// trim: leading and trailing whitespace of string
void trim (char *dest, char *src)
{
    if (!src || !dest)
       return;

    int len = strlen (src);

    if (!len) {
        *dest = '\0';
        return;
    }
    char *ptr = src + len - 1;

    // remove trailing whitespace
    while (ptr > src) {
        if (!isspace (*ptr))
            break;
        ptr--;
    }

    ptr++;

    char *q;
    // remove leading whitespace
    for (q = src; (q < ptr && isspace (*q)); q++)
        ;

    while (q < ptr)
        *dest++ = *q++;

    *dest = '\0';
}
