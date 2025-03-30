#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<sys/wait.h>
void bg(pid_t pid);
void fg(pid_t pid);