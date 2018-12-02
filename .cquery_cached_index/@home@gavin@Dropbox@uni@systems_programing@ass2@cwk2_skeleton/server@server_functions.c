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
#include <netdb.h> 
// includes for getIp
#include <net/if.h>
#include <time.h>
// includes for stat etc
#include <sys/ioctl.h>
#include <net/if.h>
#include <time.h>

#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>


// how to send a string
void send_hello(int socket)
{
  char hostbuffer[256]; 
	char *IPbuffer; 
	struct hostent *host_entry; 
	int hostname; 

	// To retrieve hostname 
	hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    if (hostname == -1) { 
		perror("gethostname"); 
		exit(1); 
	}

	// To retrieve host information 
	host_entry = gethostbyname(hostbuffer); 
    if (host_entry == NULL) { 
		perror("gethostbyname"); 
		exit(1); 
	}

	// To convert an Internet network 
	// address into ASCII string 
	IPbuffer = inet_ntoa(*((struct in_addr*) 
						host_entry->h_addr_list[0])); 



    char hello_string_full[250];
    char IPbuffer_copy[40];
    char hello_string[] = "hello SP Gavin Ross S1821951 ";
    //copy strings to new strings
    strcpy(hello_string_full, hello_string);
    strcpy(IPbuffer_copy, IPbuffer);
    printf("%s\n", IPbuffer_copy);
    // cat IP to the full string
    strcat(hello_string_full, IPbuffer_copy);
    //strcat(hello_string_full, "\0");
    printf("%s\n", hello_string_full);
    //strcat(hello_string_full, "\0");
    
    size_t n = strlen(hello_string_full) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));
    writen(socket, (unsigned char *) hello_string_full, n);

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

void stat_file(char *file)
{

    struct stat sb;

    if (stat(file, &sb) == -1) {
      perror("stat");
      //exit(EXIT_FAILURE);
      printf("error: stat failed\n");
    }

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
	printf("block device\n");
	break;
    case S_IFCHR:
	printf("character device\n");
	break;
    case S_IFDIR:
	printf("directory\n");
	break;
    case S_IFIFO:
	printf("FIFO/pipe\n");
	break;
    case S_IFLNK:
	printf("symlink\n");
	break;
    case S_IFREG:
	printf("regular file\n");
	break;
    case S_IFSOCK:
	printf("socket\n");
	break;
    default:
	printf("unknown?\n");
	break;
    }

    printf("I-node number:            %ld\n", (long) sb.st_ino);

    printf("Mode:                     %lo (octal)\n",
	   (unsigned long) sb.st_mode);

    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Ownership:                UID=%ld   GID=%ld\n",
	   (long) sb.st_uid, (long) sb.st_gid);

    printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    printf("File size:                %lld bytes\n",
	   (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));

    printf("\n");
}


int print_file_sizes()
{
  printf("This is all the files with info in the uploads folder");
    struct dirent **namelist;
    int n;
    
    if ((n = scandir("./upload", &namelist, NULL, alphasort)) == -1)
	perror("scandir");
    else {
	while (n--) {
	    printf("File name:		  %s\n", namelist[n]->d_name);
	    stat_file(namelist[n]->d_name);
	    free(namelist[n]);	//NB
	}
	free(namelist);		//NB
    }

    //exit(EXIT_SUCCESS);

  return 0;
}

void send_file_names(int socket)
{
    struct dirent **namelist;
    int status;
    int n;

    /** get the length of each filename in the directory to make an array */
    int char_count = 0;// this is to hold the length of how long the filelist[] will be
    int current_file = scandir("upload", &namelist, NULL, alphasort);
    // counts the length of the filelist string
    while (current_file--) {
      // get the lenght of the string
      int str_length = strlen(namelist[current_file]->d_name);
      //printf("%d\n", str_length);
      char_count += (str_length + 1); // plus 1 for each \n ??????
    }
    //printf("char count %d\n", char_count);

    /* // This holds the total length of the char array of file names */
    char filelist[500] = "";

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


    // send the string
    size_t string = strlen(filelist) + 1;
    writen(socket, (unsigned char *) &string, sizeof(size_t));
    writen(socket, (unsigned char *) filelist, string);

    
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
  //int connfd = (int)arg;
  int connfd = (int*)arg;
  char fname[] = "text.txt";
  printf("Connection accepted and id: %d\n",connfd);
      //printf("Connected to Clent: %s:%d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
  //write(connfd, fname,256);

  FILE *fp = fopen(fname, "rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return 1;
        }

        /* Read data from file and send it */
        while(1)
          {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[256]={0};
            int nread = fread(buff, sizeof(char), sizeof(buff), fp);
            //int nread = readn(connfd, buff, 1024);
            printf("Bytes read %d \n", nread);
            
            
            if (nread == 0)
              break;


            /* If read was success, send data. */
            if(nread > 0)
              {
                printf("Sending \n");
                write(connfd, buff, nread);
                //if (write(connfd, buff, nread) == -1)
                // printf("error writting to socket");
              }
            if (nread < 256)
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
        //close(connfd);
        //shutdown(connfd,SHUT_WR);
        //        sleep(2);

}

void* send_file2(int *arg)
{
  char fname[] = "text.txt";
      int connfd=(int)*arg;
      printf("Connection accepted and id: %d\n",connfd);
      //printf("Connected to Clent: %s:%d\n",inet_ntoa(serv_addr.sin_addr),ntohs(serv_addr.sin_port));
      //write(connfd, fname,256);
       
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
        //shutdown(connfd,SHUT_WR);
        //        sleep(2);
}
void send_file3(int arg)
{
  //char fname[] = "text.txt";
      int connfd=arg;
      printf("Connection accepted and id: %d\n",connfd);
      //write(connfd, fname,256);


    /* Get file name and Create file where data will be stored */
    char fname[256];
    size_t k;
    readn(connfd, (unsigned char *) &k, sizeof(size_t));	
    readn(connfd, (unsigned char *) fname, k);

    // add the file name to the fname_with_dir to search for the file
    char fname_with_dir[256] = "./upload/";
      strcat(fname_with_dir, fname);
      
        FILE *fp = fopen(fname_with_dir,"rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return 1;   
        }   
        




        /* // code to send filesize */
        struct stat *buf;
        buf = malloc(sizeof(struct stat));
        
        stat(fname, buf);
        int size = buf->st_size;
        printf("file size is %d\n",size);
        //free(buf);

        /* Get file stats */
        char convert_int[64];
        sprintf(convert_int, "%d", size);
        size_t payload_length = sizeof(convert_int);
        
        writen(connfd, (unsigned char *) &payload_length, sizeof(size_t));
        writen(connfd, (unsigned char *) convert_int, payload_length);


        /* int len = writen(connfd, size, sizeof(size)); */
        /* if (len < 0) */
        /* { */
        /*       fprintf(stderr, "Error on sending greetings --> %s", strerror(errno)); */

        /*       exit(EXIT_FAILURE); */
        /* } */

        /* fprintf(stdout, "Server sent %d bytes for the size\n", len); */
        // end of trial




        
        /* Read data from file and send it*/
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
        //close(connfd); 
        //shutdown(connfd,SHUT_WR);
        //        sleep(2);
}


int file_check(int socket)
{
     struct dirent **namelist;
    int status;
    int n;

    char filelist[500] = "";
    char is_file = 'n';
    char not_file = 'n';

    
 /* Get file name and Create file where data will be stored */
    char fname[256] = "";
    size_t k;
    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) fname, k);

   
    
    n = scandir("upload", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
      while (n--) {
        printf("%s\n", namelist[n]->d_name); // print the name of the file
        if(strcmp(namelist[n]->d_name, fname) == 0) {
          is_file = 'y';
           break;
         }
        else
          is_file = 'n';
        free(namelist[n]);
      }
      free(namelist);
    }

    //printf("char count = %d\n", char_count);
    printf("%c\n", is_file);


    // send the string
    char is_file_string[] = "File present\n";
    char not_file_string[] = "File not found\n";

    size_t is_file_ln = strlen(is_file_string) + 1;
    size_t not_file_ln = strlen(not_file_string) + 1;

    if (is_file == 'y') {
      writen(socket, (unsigned char *) &is_file_ln, sizeof(size_t));
      writen(socket, (unsigned char *) is_file_string, is_file_ln);
      return 0;
    }
    writen(socket, (unsigned char *) &not_file_ln, sizeof(size_t));
    writen(socket, (unsigned char *) not_file_string, not_file_ln);
    return 1;
}

int get_time_running() {
        fprintf (stdout, "The current epoch time / ms: %ld\n", get_time());
        long start_time = get_time();
        int i;
        for (i = 0; i < 1e8; ++i);
        fprintf (stdout, "It took %ld ms to count to 10^8.\n", \
                        get_time() - start_time);
        return 0;
}
unsigned long get_time() {
  
struct timeval tv1, tv2;

    // get "wall clock" time at start
    if (gettimeofday(&tv1, NULL) == -1) {
	perror("gettimeofday error");
	exit(EXIT_FAILURE);
    }
    // set CPU time at start
    clock_t start, end;
    if ((start = clock()) == -1) {
	perror("clock start error");
	exit(EXIT_FAILURE);
    }
    // do something CPU bound - anything really...
    int i;
    int j = 55;
    for (i = 0; i < 3000000; i++) {
	j++;
    }

    // set CPU time at end
    if ((end = clock()) == -1) {
	perror("clock end error");
	exit(EXIT_FAILURE);
    }

    printf("Time on CPU = %f seconds\n",
	   ((double) (end - start)) / CLOCKS_PER_SEC);

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

unsigned long get_time_running_svr(unsigned long tv) {
  unsigned long tv_start = get_time();
  return tv - tv_start;
}

int getIp()
{

  char hostbuffer[256]; 
	char *IPbuffer; 
	struct hostent *host_entry; 
	int hostname; 

	// To retrieve hostname 
	hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    if (hostname == -1) { 
		perror("gethostname"); 
		exit(1); 
	}

	// To retrieve host information 
	host_entry = gethostbyname(hostbuffer); 
    if (host_entry == NULL) { 
		perror("gethostbyname"); 
		exit(1); 
	}

	// To convert an Internet network 
	// address into ASCII string 
	IPbuffer = inet_ntoa(*((struct in_addr*) 
						host_entry->h_addr_list[0])); 

	//printf("Hostname: %s\n", hostbuffer); 
	printf("Host IP: %s\n", IPbuffer); 

	return 0; 
}

