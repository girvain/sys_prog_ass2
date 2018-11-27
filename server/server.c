// Cwk2: server.c - multi-threaded server using readn() and writen()

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


void get_and_send_employee(int, employee *);
void send_hello(int);
int getIp();
int recieve_menu_option(int socket);
void sendRandArray();
void get_and_send_ints(int);
void send_uts();
void send_file_names();
void send_time(int socket);
void *send_file(int *arg);


struct sockaddr_in serv_addr; 
struct sockaddr_in client_addr;
char fname[100];

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

    //===================================================================
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


   //===================================================================


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
      scanf("%s", fname);
      pthread_t tid; 
      int err;
      err = pthread_create(&tid, NULL, &send_file, &connfd);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        close(connfd);
      
      break;
    case 7 :
      printf("choice 7\n");
      break;
    default:
      printf("invalid selection\n");
      break;
        }


    }// end of while


    /* employee *employee1; */
    /* employee1 = (employee *) malloc(sizeof(employee)); */

    /* int i; */
    /* for (i = 0; i < 5; i++) { */
	/* printf("(Counter: %d)\n", i); */
	/* get_and_send_employee(connfd, employee1); */
	/* printf("\n"); */
    /* } */

    /* free(employee1); */

    //shutdown(connfd, SHUT_RDWR);
    //close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()

// how to send a string
void send_hello(int socket)
{
    char hello_string[] = "hello SP Gavin Ross S1821951";

    size_t n = strlen(hello_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) hello_string, n);	  

} // end send_hello()


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
    
} // end get_hello()


// as before...
void get_and_send_employee(int socket, employee * e)
{
    size_t payload_length;

    size_t n =
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
    printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    n = readn(socket, (unsigned char *) e, payload_length);

    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);
    printf("(%zu bytes)\n", n);

    // make arbitrary changes to the struct & then send it back
    e->age++;
    e->salary += 1.0;

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) e, payload_length);
}  // end get_and_send_employee()

int generateRandNum()
{
  int randNum = rand();
  int inRangeNum = (randNum % 1000);
  printf("random number is: %d\n", inRangeNum);
  return inRangeNum;
}


void get_and_send_ints(int socket)
{
  // make random number array
  int array[5];
  int i;
  for (i = 0; i < 6; i++) {
    array[i] = generateRandNum();
  }
    size_t payload_length = sizeof(array);
    
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) array, payload_length);
}  

void send_uts(int socket)
{
  struct utsname uts;

    if (uname(&uts) == -1) {
	perror("uname error");
	exit(EXIT_FAILURE);
    }

    printf("Node name:    %s\n", uts.nodename);
    printf("System name:  %s\n", uts.sysname);
    printf("Release:      %s\n", uts.release);
    printf("Version:      %s\n", uts.version);
    printf("Machine:      %s\n", uts.machine);

    //exit(EXIT_SUCCESS);

    size_t payload_length = sizeof(struct utsname); 
    
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t)); 
    writen(socket, (unsigned char *) &uts, payload_length);
}

void send_file_names(int socket)
{
    //stat stuff
    /* struct stat *buffer;  */
    /* buffer = malloc(sizeof(struct stat)); */
    /* int status; */
    /* // scandir stuff */
    /* struct dirent **namelist; */
    /* int n; */

    /* // string to concat filenames with */
    /* int char_count = 0;// this is to hold the length of how long the filelist[] will be */
    /* int current_file = scandir("./upload", &namelist, NULL, alphasort); */
    /* while (current_file--) { */
    /*   // get the lenght of the string */
    /*   int str_length = strlen(namelist[current_file]->d_name); */
    /*   //printf("%d\n", str_length); */
    /*   char_count += (str_length + 1); // plus 1 for each \n ?????? */
    /* } */
    /* printf("char count %d\n", char_count); */
    /* // This holds the total length of the char array of file names */
    /* char filelist[char_count]; */

    /* n = scandir("./upload", &namelist, NULL, alphasort); */
    /* if (n < 0) */
    /*     perror("scandir"); */
    /* else { */

    /*   while (n--) { */
    /*     printf("%s", namelist[n]->d_name); // print the name of the file */
    /*     strcat(filelist, namelist[n]->d_name); */
    /*     strcat(filelist, "\n"); */
    /*     // use stat to add bytes, maybe add more info later */
    /*     stat(namelist[n]->d_name, buffer); */
    /*     int size = buffer->st_size; */
    /*     printf(" %d bytes\n", size); */
    /*     free(namelist[n]); */
    /*   } */
    /*   free(namelist); */

    /* } */
    /* printf("%s", filelist); */



  
    struct dirent **namelist;
    int status;
    int n;

    /** get the length of each filename in the directory to make an array */

    /* int char_count = 0;// this is to hold the length of how long the filelist[] will be */
    /* int current_file = scandir("upload", &namelist, NULL, alphasort); */
    /* // counts the length of the filelist string */
    /* while (current_file--) { */
    /*   // get the lenght of the string */
    /*   int str_length = strlen(namelist[current_file]->d_name); */
    /*   //printf("%d\n", str_length); */
    /*   char_count += (str_length + 1); // plus 1 for each \n ?????? */
    /* } */
    /* //printf("char count %d\n", char_count); */

    /* // This holds the total length of the char array of file names */
    char filelist[500];

    n = scandir("upload", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
      while (n--) {
        printf("%s\n", namelist[n]->d_name); // print the name of the file
         strcat(filelist, namelist[n]->d_name);
         strcat(filelist, "\n");
        free(namelist[n]);
      }
      free(namelist);
    }

    strcat(filelist, "\0"); // add this to end the stirng
    //printf("char count = %d\n", char_count);
    printf("%s\n", filelist);

    /* // send the length of the string */
    /* size_t string_size = strlen(filelist) + 1; */
    /* writen(socket, (unsigned char *) &string, sizeof(size_t));	 */
    /* writen(socket, (unsigned char *) filelist, string);	   */

    char testArray[60] = "one\ntwo\nthree\n";
    // send the string
    size_t string = strlen(filelist) + 1;
    writen(socket, (unsigned char *) &string, sizeof(size_t));
    writen(socket, (unsigned char *) filelist, string);

  // send the string as payload
    /* size_t payload_length = sizeof(filelist); */
    
    /* writen(socket, (unsigned char *) &payload_length, sizeof(size_t)); */
    /* writen(socket, (unsigned char *) filelist, payload_length); */

    
} // end of send_file_names()

void send_time(int socket)
{
  time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
    if ((t = time(NULL)) == -1) {
	perror("time error");
	exit(EXIT_FAILURE);
    }

    // localtime() is in standard library so error conditions are
    //  here "man 3 localtime"
    struct tm *tm;
    if ((tm = localtime(&t)) == NULL) {
	perror("localtime error");
	exit(EXIT_FAILURE);
    }    
    
    printf("%s\n", asctime(tm));

    //char hello_string[100]; 
    size_t n = strlen(asctime(tm)) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) asctime(tm), n);	  


}

/* ================================== send files =============================*/

//char fname[100];
void* send_file(int *arg)
{
      int connfd=(int)*arg;
      printf("Connection accepted and id: %d\n",connfd);
      printf("Connected to Clent: %s:%d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
       write(connfd, fname,256);

        FILE *fp = fopen(fname,"rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return 1;   
        }   

        /* Read data from file and send it */
        while(1)
          {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[1024]={0};
            int nread = fread(buff,1,1024,fp);
            //printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
            if(nread > 0)
              {
                printf("Sending \n");
                write(connfd, buff, nread);
              }
            if (nread < 1024)
              {
                if (feof(fp))
                  {
                    printf("End of file\n");
		    printf("File transfer completed for id: %d\n",connfd);
                  }
                if (ferror(fp))
                    printf("Error reading\n");
                break;
              }
          }
        printf("Closing Connection for id: %d\n",connfd);
        close(connfd); 
        shutdown(connfd,SHUT_WR);
        sleep(2);
}



/* ================================== end of send files =======================*/
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
