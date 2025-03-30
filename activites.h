#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define running 1
#define stopped 0
#define terminated 2

void add_p(int pid, char *command, int state);
void update_p(int pid, int state);
void remove_p(int pid);
void print_activities();