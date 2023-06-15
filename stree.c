#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <sys/types.h>    /* Type definitions used by many programs */
#include <stdio.h>        /* Standard I/O functions */
#include <stdlib.h>       /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>       /* Prototypes for many system calls */
#include <errno.h>        /* Declares errno and defines error constants */
#include <string.h>   
#include <pwd.h>          /* For getting user Name*/
#include <grp.h>          /* For getting group Name*/


// First version Created for using ftw - not needed
int callback(const char *name, const struct stat *status, int type);

// Second version Created for nftw
int callback_nftw(const char *name, const struct stat *status, int type, struct FTW *ftwb); 

// returns the file permission and file type given stat(2) and a string
char* file_permissions(const struct  stat *fileStat, char* res);

// Counts the length of a number to make the tree more astetic
int decimal_count(long number);


// Creating a tree structure from the current folder moving down the tree
int main(int argc, char const *argv[])
{
    int flags = 0;
    // a normal call will just start from the current folder
    if (nftw(".", callback_nftw, 10, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    return 0;
}


// The callback function used by ftw
int callback(const char *name, const struct stat *status, int type) {

   /* Could not stat() file */
   if(type == FTW_NS)
      return 0;
    
   // print information about the current file 
   if(strcmp(".",name) != 0){
      // print permissions 
      char res[10];
      printf("[%s ",file_permissions(status,res));
      // print user name
      struct passwd *pw = getpwuid(status->st_uid);
      printf("%s ",pw->pw_name);
      // print group name
      struct group  *gr = getgrgid(status->st_gid);
      printf("%s ",gr->gr_name);
      // print the file size in bytes
      printf("%ld] %s\n",status->st_size,name);
   }

   return 0;
}

// The callback function used by nftw
int callback_nftw(const char *name, const struct stat *status, int type, struct FTW *ftwb) {

   /* Could not stat() file */
   if(type == FTW_NS)
      return 0;
    
   // print information about the current file 
   if(strcmp(".",name) != 0){

      //print the tree structure
      if(ftwb->level == 1){
        printf("├── ");
      }
      else{
        printf("    ");
        for (size_t i = 0; i < ftwb->level-2 && i<10; i++) // hard limit for checks
        {
            printf("│   ");
        }
        printf("├── ");
      }

      // print permissions 
      char res[10];
      printf("[%s ",file_permissions(status,res));
      // print user name
      struct passwd *pw = getpwuid(status->st_uid);
      printf("%s     ",pw->pw_name);
      // print group name
      struct group  *gr = getgrgid(status->st_gid);
      printf("%s",gr->gr_name);
      // give just the right amount of space to make it more astetic
      printf("%*s",16-decimal_count(status->st_size),"");
      // print the file size in bytes
      printf("%ld] %s\n",status->st_size,&name[ftwb->base]);
   }

   return 0;
}

// prints the file permissions
char* file_permissions(const struct  stat *fileStat, char* res){

    // print file type
    switch (fileStat->st_mode & S_IFMT) {
        case S_IFREG:  res[0]='-'; break;
        case S_IFDIR:  res[0]='d'; break;
        case S_IFCHR:  res[0]='c'; break;
        case S_IFBLK:  res[0]='b'; break;
        case S_IFLNK:  res[0]='l'; break;
        case S_IFIFO:  res[0]='p'; break;
        case S_IFSOCK: res[0]='s'; break;
        default:       res[0]='?'; break; /* Should never happen (on Linux) */
    }
    // print permission for user,group and other. 3 each, for read,write and execute permissions
    res[1] = (fileStat->st_mode & S_IRUSR) ? 'r' : '-';
    res[2] = (fileStat->st_mode & S_IWUSR) ? 'w' : '-';
    res[3] = (fileStat->st_mode & S_IXUSR) ? 'x' : '-';
    res[4] = (fileStat->st_mode & S_IRGRP) ? 'r' : '-';
    res[5] = (fileStat->st_mode & S_IWGRP) ? 'w' : '-';
    res[6] = (fileStat->st_mode & S_IXGRP) ? 'x' : '-';
    res[7] = (fileStat->st_mode & S_IROTH) ? 'r' : '-';
    res[8] = (fileStat->st_mode & S_IWOTH) ? 'w' : '-';
    res[9] = (fileStat->st_mode & S_IXOTH) ? 'x' : '-';
    
    return res;
}

// Counts the length of a given number
int decimal_count(long number){
    if(number==0){
        return 1;
    }
    int count=0;
    while(number!=0){
        count++;
        number = number/10;
    }

    return count;
}
