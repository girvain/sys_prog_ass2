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
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
/* TODO:
 * 1) basic error handling
 * 7) signal interupts
 * 4) fix server loop repeat command on interrupt
 * 5) add comments
 */

struct timeval *start_time;

// thread function
void *client_handler(void *);
int recieve_menu_option(int);

/* /\* set CPU time at start by passing in global variable. *\/ */
void set_start_time()
{
  struct timeval tv1;
  clock_t start;

  // get "wall clock" time at start
  if (gettimeofday(&tv1, NULL) == -1) {
    perror("gettimeofday error");
	exit(EXIT_FAILURE);
  }
  
  if ((start = clock()) == -1) {
    perror("clock start error");
	exit(EXIT_FAILURE);
  }

  clock_t *start_heap = (clock_t*) malloc(sizeof(start));
  start_time =  (struct timeval *)malloc(sizeof(struct timeval));
  start_time = &tv1;
}

void set_end_time(struct timeval *start_time)
{
  struct timeval tv1 = *start_time;
  struct timeval tv2;

      // set CPU time at start
    //clock_t end;

    // set CPU time at end
    /* if ((end = clock()) == -1) { */
    /*   perror("clock end error"); */
    /*   exit(EXIT_FAILURE); */
    /* } */

    /* printf("Time on CPU = %f seconds\n", */
	/*    ((double) (end - start)) / CLOCKS_PER_SEC); */

    // get "wall clock" time at end
    if (gettimeofday(&tv2, NULL) == -1) {
	perror("gettimeofday error");
	exit(EXIT_FAILURE);
    }

    // in microseconds...
    printf("Total execution time = %f seconds\n",
	   (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	   (double) (tv2.tv_sec - tv1.tv_sec));

    //exit(EXIT_SUCCESS);
}

// signal handler to be called on receipt of (in this case) SIGTERM
void handler(int sig, siginfo_t *siginfo, void *context)
{
  //set_start_time();
  set_end_time(start_time);
  exit(1);
}
/* function to simplify the setting up of a signal handler */
int catch_signal(int sig, void (*handler)(int))
{
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  return sigaction (sig, &action, NULL);
}


// you shouldn't need to change main() in the server except the port number
int main(void)
{

  set_start_time();
  // test code to prove server exec works
  
  /* int i; */
  /* for (i = 0; i < 1000000000; ++i) { */
    
  /* } */
  /* set_end_time(start_time); */






  /*======================  signal handler ==================== */
  if (catch_signal(SIGINT, &handler) == -1) {
    fprintf(stderr, "Can't map the handler");
    exit(2);
  }    
 
  // display ip on server
    getIp();
  
    int listenfd = 0, connfd = 0;
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);
    
 
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
	pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");

    } // end of while 

    // never reached...
    // ** should include a signal handler to clean up
   
    exit(EXIT_SUCCESS);

} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
/** This funcion runs a while loop infinately until the user either uses ctl-c
    or selects option 7 to close the connection, achieved by checking exit codes */
void *client_handler(void *socket_desc )
{
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;
    while(1) {
      /* if this function returns -1 it has recieved a ctrl-c (FIN signal) 
         from client */
      int result = recieve_menu_option(connfd);
      if (result == -1) {
        fprintf(stderr, "The user has closed connection\n");
        break;
      }
    }
    
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
    /* readn(socket, (unsigned char *) &k, sizeof(size_t)); */
    /* readn(socket, (unsigned char *) hello_string, k); */
     
    if (read(socket, hello_string, (sizeof(&k))) != 0) {
     printf("Option: %s selected\n", hello_string);
    //printf("Received: %zu bytes\n\n", k);
    //printf("first char of arr is: %c \n", hello_string[0]);

    char option = (char)hello_string[0];
    if (option != NULL) {
      
    switch (option) {
    case '1':
      send_hello(socket);
      return 1;
    case '2':
      get_and_send_ints(socket);
      return 2;
    case '3':
      send_uts(socket);
      return 3;
    case '4':
      print_file_sizes();
      send_file_names(socket);
      return 4;
    case '5':
      send_time(socket);
      return 5;
    case '6':
      //file_check(socket);
      //send_file3(socket);
      if (file_check(socket) == 0) {
        send_file3(socket);
      }
      fprintf(stderr, "error: file not on server\n");
      return 6;
    case '7':
      return 7;
    case ' ':
      return 0;
    default :
      return 0;
    }
      }
 
    }
    else 
      return -1;
} // end get_hello() */


