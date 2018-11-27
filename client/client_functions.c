#include "client_functions.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"
#include <sys/utsname.h>

void get_hello(int socket)
{
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);

    printf("Hello String: %s\n", hello_string);
    printf("Received: %zu bytes\n\n", k);
} // end get_hello()

void send_and_get_ints(int socket)  
{
  int array[5];
  size_t payload_length;
  size_t n = readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
  printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
  n = readn(socket, (unsigned char *) array, payload_length);

  // print the contents of the array
  int i;
  for (i = 0; i < 5; i++) {
    printf("the array of ints: %i\n", array[i]);
  }

} 

void get_random_numbers()
{
  char hello_string[32];
  size_t k;

  readn(socket, (unsigned char *) &k, sizeof(size_t));	
  readn(socket, (unsigned char *) hello_string, k);
}

void get_uts(int socket)
{
  struct utsname uts;
  size_t payload_length;
  size_t n = readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
  printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
  n = readn(socket, (unsigned char *) &uts, payload_length);

  printf("Node name:    %s\n", uts.nodename);
    printf("System name:  %s\n", uts.sysname);
    printf("Release:      %s\n", uts.release);
    printf("Version:      %s\n", uts.version);
    printf("Machine:      %s\n\n", uts.machine);

}

/* Add an extra send/recieve to this function to get the size of the data being sent
 * then make the hello_string that size.
 */
void get_filenames(int socket)
{
  char hello_string[512];
    size_t k;
   
    readn(socket, (unsigned char *) &k, sizeof(size_t));
    //char hello_string[k];
    readn(socket, (unsigned char *) hello_string, k);

    printf("filenames in upload directory:\n%s", hello_string);
    //printf("Received: %zu bytes\n\n", k);

    // payload way, not working with extra chars in results
/* int array[256]; */
/*   size_t payload_length; */
/*   size_t n = readn(socket, (unsigned char *) &payload_length, sizeof(size_t)); */
/*   printf("payload_length is: %zu (%zu bytes)\n", payload_length, n); */
/*   n = readn(socket, (unsigned char *) array, payload_length); */

/*  printf("filenames in upload directory:\n%s", array); */


} // end get_hello()

void get_time(int socket)
{
  char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);
    
    printf("%s\n", hello_string);
}

/* ============================= recieve file ============================== */
void gotoxy(int x,int y)
 {
 printf("%c[%d;%df",0x1B,y,x);
 }

void get_file(int sockfd)
{
  system("clear");
  //int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[1024];
    memset(recvBuff, '0', sizeof(recvBuff));

 /* Create file where data will be stored */
  FILE *fp;
	char fname[100];
	read(sockfd, fname, 256);
	//strcat(fname,"AK");
	printf("File Name: %s\n",fname);
	printf("Receiving file...");
   	 fp = fopen(fname, "ab"); 
    	if(NULL == fp)
    	{
       	 printf("Error opening file");
         return 1;
    	}
    long double sz=1;
    /* Receive data in chunks of 256 bytes */
    while((bytesReceived = read(sockfd, recvBuff, 1024)) > 0)
    { 
        sz++;
        gotoxy(0,4);
        printf("Received: %llf Mb",(sz/1024));
	fflush(stdout);
        // recvBuff[n] = 0;
        fwrite(recvBuff, 1,bytesReceived,fp);
        printf("%s \n", recvBuff);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }
    printf("\nFile OK....Completed\n");

}

