#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>


int main(void)
{
    //stat stuff
    struct stat buffer;
    int status;
    // scandir stuff
    struct dirent **namelist;
    int n;

    // string to concat filenames with 
    int char_count = 0;// this is to hold the length of how long the filelist[] will be
    int current_file = scandir("test-dir", &namelist, NULL, alphasort);
    while (current_file--) {
      // get the lenght of the string
      int str_length = strlen(namelist[current_file]->d_name);
      //printf("%d\n", str_length);
      char_count += (str_length + 1); // plus 1 for each \n ??????
    }
    printf("%d\n", char_count);
    // This holds the total length of the char array of file names
    char filelist[char_count];

    n = scandir("test-dir", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
      while (n--) {
        printf("%s", namelist[n]->d_name); // print the name of the file
        strcat(filelist, namelist[n]->d_name);
        strcat(filelist, "\n");
        // use stat to add bytes, maybe add more info later
        stat(namelist[n]->d_name, &buffer);
        int size = buffer.st_size;
        printf(" %d bytes\n", size);
        free(namelist[n]);
      }
      free(namelist);
    }
    printf("%s", filelist);
      
}
