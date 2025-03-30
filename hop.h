#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#define MAX_PATH_LENGTH 4096
#define MAX_INPUT_LENGTH 4096
void print_current_directory();

void hop_to_directory(const char *path, char *prev_path, const char *home_directory);