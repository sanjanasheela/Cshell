#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
void seek(const char *base_path, const char *relative_path, const char *search_name);
int is_directory(const char *path);
void print_file_content(const char *filepath);
void seekl(int files, int dirs, int enable, char *target_dir, char *search_name);