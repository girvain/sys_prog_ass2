// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module
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

    /** Use juse the file transfer */
    //get_file2(sockfd);


    //your own application code will go here and replace what is below... 
    //i.e. your menu etc. /
    char user_input[124];
    //int user_input;
    while (user_input[0] != '7')
    {
      displayOptions();
        scanf("%s", user_input);

      //send_menu_option(sockfd, user_input);

      switch(user_input[0]) {
      case '1' :
       send_menu_option(sockfd, "1");
        get_hello(sockfd);
        break;
      case '2' :
        send_menu_option(sockfd, "2");
        send_and_get_ints(sockfd);
        break;
      case '3' :
        send_menu_option(sockfd, "3");
        get_uts(sockfd);
        break;
      case '4' :
        send_menu_option(sockfd, "4");
        get_filenames(sockfd);
        break;
      case '5' :
        send_menu_option(sockfd, "5");
        get_time(sockfd);
        break;
      case '6' :
        send_menu_option(sockfd, "6");
        // send the name of file you want to the server
        char fname[256] = "";
        bzero(fname, 256); // clear the buffer
        printf("Enter the file name you want \n");
        scanf("%s", fname);
        printf("This is whats in the buffer: %s\n", fname);

        int condition = file_check(sockfd, fname);
        printf("%d", condition);
        if (condition == 0) {
          //printf("file check exited right");
          get_file2(sockfd, &fname);
        }
        
        //get_file2(sockfd, &fname);
       
        break;
      case '7' :
        send_menu_option(sockfd, "7");
        break;
      default:
          printf("Option not recognised\n");
      }
      //printf("Would you like another option[Y/n]\n");
      }// end of while

      
    /*** make sure sockets are cleaned up */
    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()
