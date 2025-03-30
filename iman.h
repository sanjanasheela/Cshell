#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <regex.h>

#define MAX_REQUEST_LEN 1024
#define BUFFER_SIZE 8192
#define COMMAND_NAME_LEN 1024
void iman(char* command_name);
void remove_tags(char *str);