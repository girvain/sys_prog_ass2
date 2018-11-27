#include "server_functions.h"
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
#include <net/if.h>
#include <time.h>
// includes for stat etc

#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>

// how to send a string
void send_hello(int socket)
{
    char hello_string[] = "hello SP Gavin Ross S1821951";

    size_t n = strlen(hello_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));
    writen(socket, (unsigned char *) hello_string, n);

} // end send_hello()


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

/* //char fname[100]; */
/* void* send_file(int *arg) */
/* { */
/*       int connfd=(int)*arg; */
/*       printf("Connection accepted and id: %d\n",connfd); */
/*       printf("Connected to Clent: %s:%d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port)); */
/*        write(connfd, fname,256); */

/*         FILE *fp = fopen(fname,"rb"); */
/*         if(fp==NULL) */
/*         { */
/*             printf("File opern error"); */
/*             return 1;    */
/*         }    */

/*         /\* Read data from file and send it *\/ */
/*         while(1) */
/*           { */
/*             /\* First read file in chunks of 256 bytes *\/ */
/*             unsigned char buff[1024]={0}; */
/*             int nread = fread(buff,1,1024,fp); */
/*             //printf("Bytes read %d \n", nread);         */

/*             /\* If read was success, send data. *\/ */
/*             if(nread > 0) */
/*               { */
/*                 printf("Sending \n"); */
/*                 writen(connfd, buff, nread); */
/*               } */
/*             if (nread < 1024) */
/*               { */
/*                 if (feof(fp)) */
/*                   { */
/*                     printf("End of file\n"); */
/* 		    printf("File transfer completed for id: %d\n",connfd); */
/*                   } */
/*                 if (ferror(fp)) */
/*                     printf("Error reading\n"); */
/*                 break; */
/*               } */
/*           } */
/*         printf("Closing Connection for id: %d\n",connfd); */
/*         close(connfd);  */
/*         shutdown(connfd,SHUT_WR); */
/*         sleep(2); */
/* } */
