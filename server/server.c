// Cwk2: server.c - multi-threaded server using readn() and writen()
#include "server_functions.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"
#include <sys/utsname.h>

#include <dirent.h>
#include <sys/stat.h>

// includes for getIp
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
// includes for stat etc


#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>

/* TODO:
 * 1) basic error handling
 * 2) stat 
 * 3) ip address display
 * 5) error handling for file send
 * 6) upload file to client on client selection
 * 7) signal interupts
 * 8) sort default on server loop switch
 * 9) sort global variables in send_file
 *10) clean up files into headers
 */
// thread function
void *client_handler(void *);

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

struct sockaddr_in serv_addr; 
struct sockaddr_in client_addr;
char fname[100] = "test.txt";

// you shouldn't need to change main() in the server except the port number
int main(void)
{
    int listenfd = 0, connfd = 0;

/* struct sockaddr_in serv_addr;  */
/* struct sockaddr_in client_addr; */
socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);

    //====================== IP section ===========================================
int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 /* display result */
 printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

   //=========================END====================================================


    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup

    
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
	printf("Waiting for a client to connect...\n");
	connfd =
	    accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
	printf("Connection accepted...\n");

	pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
	    (&sniffer_thread, NULL, client_handler,
	     (void *) &connfd) < 0) {
	    perror("could not create thread");
	    exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }

    
    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);

} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;

    //send_hello(connfd);

    /* ============================ Switch interface ==================== */
    
    int choice;

    while (choice != 7) {

    choice = recieve_menu_option(connfd);
    switch(choice) {
    case 1 :
      printf("choice 1\n");
      send_hello(connfd);
      break;
    case 2 :
      printf("choice 2\n");
      get_and_send_ints(connfd);
      break;
    case 3 :
      printf("choice 3\n");
      send_uts(connfd);
      break;
    case 4 :
      printf("choice 4\n");
      send_file_names(connfd);
      break;
    case 5 :
      printf("choice 5\n");
      send_time(connfd);
      break;
    case 6 :
      printf("choice 6\n");
      //char fname[100];
      // enter the name of file to send on the server, change this later!!
      //scanf("%s", fname);
      /* pthread_t tid;  */
      /* int err; */
      /* err = pthread_create(&tid, NULL, &send_file, &connfd); */
      /*   if (err != 0) */
      /*       printf("\ncan't create thread :[%s]", strerror(err)); */
      /*   close(connfd); */
      break;
    case 7 :
      printf("choice 7\n");
      break;
    default:
      printf("invalid selection\n");
      break;
        }
    }// end of while

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()

int recieve_menu_option(int socket)
{
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));
    readn(socket, (unsigned char *) hello_string, k);

    printf("Option: %s selected\n", hello_string);
    //printf("Received: %zu bytes\n\n", k);
    //printf("first char of arr is: %c \n", hello_string[0]);

    char option = hello_string[0];

    switch (option) {
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    default :
      return 0;
    }
    
} // end get_hello() */


int getIp()
{
int fd;
 struct ifreq ifr;
fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

// I want IP address attached to "eth0"
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

close(fd);

 /* display result */
 printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
return 0;
}
