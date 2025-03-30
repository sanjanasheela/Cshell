#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALIASES 1024
#define MAX_ALIAS_LENGTH 1024
#define MAX_COMMAND_LENGTH 1024

typedef struct
{
    char alias[MAX_ALIAS_LENGTH];
    char command[MAX_COMMAND_LENGTH];
} Alias;
char *aliase(char *command);

void parse_myshrc();