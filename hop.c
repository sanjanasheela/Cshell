#include "display.h"
#include "hop.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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

int build_resolved_path(const char *path, char *resolved_path, const char *home_directory, const char *prev_path) {
    char new_path[MAX_PATH_LENGTH];

    if (path[0] == '~') {
        snprintf(new_path, sizeof(new_path), "%s%s", home_directory, path + 1);
    } else if (strcmp(path, "-") == 0) {
        if (strlen(prev_path) == 0) {
            printf("no previous directory\n");
            return 0;
        }
        snprintf(new_path, sizeof(new_path), "%s", prev_path);
    } else {
        snprintf(new_path, sizeof(new_path), "%s", path);
    }

    if (realpath(new_path, resolved_path) == NULL) {
        perror("realpath() error");
        return 0;
    }

    return 1;
}

void hop_to_directory(const char *path, char *prev_path, const char *home_directory) {
    char resolved_path[MAX_PATH_LENGTH];

    if (!build_resolved_path(path, resolved_path, home_directory, prev_path)) {
        return;
    }

    if (getcwd(prev_path, MAX_PATH_LENGTH) == NULL) {
        perror("getcwd() error");
        return;
    }

    if (chdir(resolved_path) != 0) {
        perror("chdir() error");
        return;
    }

    print_current_directory();
}
