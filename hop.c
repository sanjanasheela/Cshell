#include "display.h"
#include "hop.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char prev_path[MAX_PATH_LENGTH] = "";
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"

extern char b_dir[MAX_PATH_LENGTH];

void print_current_directory() {
    char cwd[MAX_PATH_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror(COLOR_RED "getcwd() error" COLOR_RESET);
    }
}

void resolve_path(const char *path, char *out_path, const char *home_directory, const char *prev_path) {
    if (path[0] == '~') {
        snprintf(out_path, MAX_PATH_LENGTH, "%s%s", home_directory, path + 1);
    } else if (strcmp(path, "-") == 0) {
        if (strlen(prev_path) == 0) {
            out_path[0] = '\0';
        } else {
            snprintf(out_path, MAX_PATH_LENGTH, "%s", prev_path);
        }
    } else {
        snprintf(out_path, MAX_PATH_LENGTH, "%s", path);
    }
}

void hop_to_directory(const char *path, char *prev_path, const char *home_directory) {
    char new_path[MAX_PATH_LENGTH];

    resolve_path(path, new_path, home_directory, prev_path);

    if (strlen(new_path) == 0) {
        printf("no previous directory\n");
        return;
    }

    if (getcwd(prev_path, MAX_PATH_LENGTH) == NULL) {
        perror("getcwd() error");
        return;
    }

    if (chdir(new_path) != 0) {
        perror("chdir() error");
        return;
    }

    print_current_directory();
}
