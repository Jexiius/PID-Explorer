#include <iostream>      /* for std::cerr */
#include <stdlib.h>      /* for exit, system */
#include <unistd.h>      /* for read, write */
#include <errno.h>       /* for errno */
#include <string.h>      /* for strerror */
#include <dirent.h>      /* for DIR */
#include <sys/stat.h>
#include <time.h>        /* for nanosleep */
#include <sys/types.h>   /* for pid_t      */
#include <sys/socket.h>  /* for socketpair */
#include <sys/wait.h>    /* for wait        */

using std::cerr;

ssize_t Read(int fildes, void *buffer, size_t nbytes){ 
    ssize_t result  = read(fildes, buffer, nbytes);
    if(result == -1){
      cerr << "Error while reading: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

ssize_t Write(int fildes, const void *buffer, size_t nbytes){
  ssize_t result  = write(fildes, buffer, nbytes);
    if(result == -1){
      cerr << "Error while writing: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

int Pipe(int filedes[2]){
    int result = pipe(filedes);
    if(result == -1){
      cerr << "Error in pipe: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

int Nanosleep(const struct timespec *req, struct timespec *rem){
    int result = nanosleep(req, rem);
    if(result == -1){
      cerr << "Error in nanosleep: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

pid_t Fork(void){
    int result = fork();
    if(result == -1){
      cerr << "Error in fork: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

int Close(int fd){
    int result = close(fd);
    if(result == -1){
      cerr << "Error in close: " << strerror(errno) << '\n';
      exit(errno);
    }
    return result;
}

//fstat() is identical to stat(), except that the file about which information is to be retrieved is specified by the file descriptor fd.
//return  information  about  a  file,  in  the buffer pointed to by statbuf.
//On success, zero is returned.  On error, -1 is returned,
int Stat(const char *pathname, struct stat *statbuf){
    int temp;
    temp = stat(pathname, statbuf);
    if(temp == -1){
        cerr << "Error in stat for file " << pathname << ": " << strerror(errno) << '\n';
        exit(errno);
    }
    return temp;
}

//The opendir() and fdopendir() functions return a pointer to the directory stream.  On error, NULL is returned, and errno is set appropriately.
DIR *Opendir(const char *name){
    DIR* temp;
    temp = opendir(name);
    if(temp == NULL){
        cerr << "Error in opendir: " << strerror(errno) << '\n';
        exit(errno);
    }
    return temp;
}

/*On success, readdir() returns a pointer to a dirent structure.  (This structure may be statically allocated; do not attempt to free(3) it.)
  If the end of the directory stream is reached, NULL is returned and errno is not changed.  If an error occurs, NULL is returned and errno is set appropriately.  
  To distinguish end of stream  and  from an error, set errno to zero before calling readdir() and then check the value of errno if NULL is returned.
  */
struct dirent *Readdir(DIR *dirp){
    errno = 0;
    struct dirent *temp;
    temp = readdir(dirp);
    if(temp == NULL && errno != 0){
        cerr << "Error in readdir: " << strerror(errno) << '\n';
    }
    return temp;
}

// The closedir() function returns 0 on success.  On error, -1 is returned, and errno is set appropriately.
int Closedir(DIR *dirp){
    int temp;
    temp = closedir(dirp);
    if(temp != 0){
        cerr << "Error in closedir: " << strerror(errno) << '\n';
        exit(errno);
    }
    return temp;
}

int Fclose(FILE *fp){  /* ok to have this not return anything */
    fclose(fp);
    return 0;
}

void *Malloc(size_t size){
    void *pointer = malloc(size);
    if(pointer == NULL){
        return NULL;
    }
    return pointer;
}

FILE *Fopen(const char *path, const char *mode) {
    FILE* tempFile = fopen(path, mode);
    if (tempFile == NULL) {
        //cmd file
        if(strcmp(mode, "r")==0){
            cerr << "cmd file not found\n";
            exit(99);
        }
        // create a new file
        tempFile = fopen("1MBbinaryFile", "wb");
        if (tempFile == NULL) {
            cerr << "Failed to create fallback file\n";
            exit(99);
        }
        
        //size = 1MB
        fseek(tempFile, 1048575, SEEK_SET); // 1MB
        fputc('\0', tempFile);
        fclose(tempFile);

        //reopen fallback
        tempFile = fopen("1MBbinaryFile", "rb+");
        if (tempFile == NULL) {
            cerr << "Failed to reopen fallback file\n";
            exit(99);
        }
    }
    return tempFile;
}

int Fseek(FILE *fp, long offset, int whence){ //return 0 , otherwise, -1 is returned  and  errno  is  set  to indicate the error.
    if (offset > 1048575){ // checking if seeking out of bounds
        cerr << "Seek offset is too large\n";
        return -1;
    }
    int result = fseek(fp, offset, whence);
    if (result != 0) {
        cerr << "Error in fseek: " << strerror(errno) << '\n';
    }
    return result;
}

long Ftell(FILE *fp){ // returns the current offset, otherwise, -1 is returned  and  errno  is  set  to indicate the error.
    long result = ftell(fp);
    if (result == -1) {
        cerr << "Error in ftell: " << strerror(errno) << '\n';
    }
    return result;
}

size_t Fread(void *ptr, size_t item_size, size_t num_items, FILE *fp){
    size_t result = fread(ptr, item_size, num_items, fp);
    if (ferror(fp)) {
        cerr << "Error in fread: " << strerror(errno) << '\n';
        clearerr(fp); // from "man ferror"
        return 0; 
    }
    return result;
}

size_t Fwrite(const void *ptr, size_t item_size, size_t num_items, FILE *fp){
    size_t result = fwrite(ptr, item_size, num_items, fp);
    if (ferror(fp)) {
        cerr << "Error in fwrite: " << strerror(errno) << '\n';
        clearerr(fp); // from "man ferror"
        return 0; 
    }
    return result;
} 

int System(const char *command){
    int result = system(command);
    if (result == -1) {
        cerr << "Error in system() cmd: " << strerror(errno) << '\n';
    }
    return result;
}

// Upon successful completion, this function shall return 0; otherwise, −1 shall be returned and errno set to indicate the error.
int Socketpair(int domain, int type, int protocol, int socket_vector[2]){
    int result = socketpair(domain, type, protocol, socket_vector);
    if (result == -1) {
        cerr << "Error in socketpair: " << strerror(errno) << '\n';
    }
    return result;
}

pid_t Wait(int *stat_loc){
    int result = wait(stat_loc);
    if (result == -1) {
        cerr << "Error in wait: " << strerror(errno) << '\n';
    }
    return result;
}
