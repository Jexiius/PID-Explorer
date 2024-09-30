#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <dirent.h>         /* for DIR */
#include <stdio.h>
#include <sys/types.h>      /* for pid_t      */
#include <sys/stat.h>
#include <stddef.h>
#include <sys/socket.h>     /* for socketpair */
#include <sys/wait.h>    /* for wait        */

FILE *Fopen(const char *pathname, const char *mode);
int Fclose(FILE *stream);
int Stat(const char *pathname, struct stat *statbuf);
DIR *Opendir(const char *name); //man 3
struct dirent *Readdir(DIR *dirp);
int Closedir(DIR *dirp);
ssize_t Read(int fd, void *buf, size_t nbyte);
ssize_t Write(int fd, const void *buf, size_t nbyte);
int Pipe(int filedes[2]);
int Nanosleep(const struct timespec *req, struct timespec *rem);
pid_t Fork(void);
int Close(int fd);
void *Malloc(size_t size);
int Fseek(FILE *fp, long offset, int whence);
long Ftell(FILE *fp);
int System(const char *command);
size_t Fread(void *ptr, size_t item_size, size_t num_items, FILE *fp);
size_t Fwrite(const void *ptr, size_t item_size, size_t num_items, FILE *fp);
int Socketpair(int domain, int type, int protocol, int socket_vector[2]);
pid_t Wait(int *stat_loc);

#endif