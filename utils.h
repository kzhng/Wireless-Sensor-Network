#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void checkIPbuffer(char *IPbuffer);
void checkHostEntry(struct hostent * hostentry);
void checkHostName(int hostname);
char* GetHostDetails(char*);

#endif