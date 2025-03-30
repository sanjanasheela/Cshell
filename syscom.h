#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include "display.h"

#include "input.h"
#include "activites.h"
typedef struct
{
    pid_t pid;
    char command[1024];
} BgProcess;
#define MAX_ARGS 4096
#define MAX_BG_PROCS 4096
void syscom(char *input);
void command(char *input, int bg);
void run_background(char **args, char *command);
void run_foreground(char **args,char* command);
void handle_sigchld(int sig);
void trim_newline(char *str);
char *trimspe(char *str);
void kill_all_bg_processes();