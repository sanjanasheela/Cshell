#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
void proclore(int pid);
int is_foreground_process(int pid);
void print_process_info(int  pid);