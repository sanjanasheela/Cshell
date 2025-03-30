#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include "hop.h"
#include<fcntl.h>
#include "display.h"
char *resolve_path(char *path);
void reveal( char *path, int show_hidden, int long_format) ;
int compare_entries(const struct dirent **a, const struct dirent **b) ;
void print_file_info(const struct dirent *entry, const char *path, int show_hidden, int long_format) ;