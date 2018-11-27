// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

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

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

// how to send and receive structs
void send_and_get_employee(int socket, employee *e)  
{
    size_t payload_length = sizeof(employee);

    // send the original struct
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));	
    writen(socket, (unsigned char *) e, payload_length);	 		

    // get back the altered struct
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));	   
    readn(socket, (unsigned char *) e, payload_length);

    // print out details of received & altered struct
    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);    
} // end send_and_get_employee()

// how to receive a string
void get_hello(int socket)
{
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);

    printf("Hello String: %s\n", hello_string);
    printf("Received: %zu bytes\n\n", k);
} // end get_hello()

void displayOptions()
{
  puts("\nEnter an option:\n1. Display name/ID\n2. Display array of random ints\n3. Display the uname\n4. List files\n5. Get time\n6. Download file\n7. Exit\n");
}

void send_menu_option(int socket, char *hello_string)
{
  //char hello_string[32]; 
    size_t n = strlen(hello_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) hello_string, n);	  
}

void recieve_welcome_msg(int socket)
{
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) hello_string, k);

    printf("Recieved menu option: %s\n", hello_string);
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

int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50001);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(1);
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.

    int user_input;
    while (user_input != 7) {
      displayOptions();
      scanf("%i", &user_input);

      //send_menu_option(sockfd, user_input);

      switch(user_input) {
      case 1 :
        send_menu_option(sockfd, "1");
        // send/recieve function !!!
        get_hello(sockfd);
        break;
      case 2 :
        send_menu_option(sockfd, "2");
        send_and_get_ints(sockfd);
        break;
      case 3 :
        send_menu_option(sockfd, "3");
        get_uts(sockfd);
        break;
      case 4 :
        send_menu_option(sockfd, "4");
        get_filenames(sockfd);
        break;
      case 5 :
        send_menu_option(sockfd, "5");
        get_time(sockfd);
        break;
      case 6 :
        send_menu_option(sockfd, "6");
        get_file(sockfd);
        break;

      case 7 :
        send_menu_option(sockfd, "7");
        break;

      } 
      //printf("Would you like another option[Y/n]\n");
    }// end of while


    
    //send_menu_option(sockfd);

        // get a string from the server

    //get_hello(sockfd);

    // send and receive a changed struct to/from the server
    /* employee *employee1;		 */
    /* employee1 = (employee *) malloc(sizeof(employee)); */

    /* // arbitrary values */
    /* employee1->age = 23; */
    /* employee1->id_number = 3; */
    /* employee1->salary = 13000.21; */

    /* int i; */
    /* for (i = 0; i < 5; i++) { */
    /*      printf("(Counter: %d)\n", i); */
	/*  send_and_get_employee(sockfd, employee1); */
    /*      printf("\n"); */
    /* } */

    /* free(employee1); */

    // *** make sure sockets are cleaned up

    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()
