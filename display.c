#include "display.h"
#include "input.h"

#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

extern char b_dir[MAX_PATH_LENGTH];
extern char late[MAX_PATH_LENGTH];

int get_hostname(char **hostname) {
    static struct utsname buff;
    if (uname(&buff) == 0) {
        *hostname = buff.nodename;
        return 1;
    } else {
        perror("uname");
        return -1;
    }
}

int get_username(char **username) {
    struct passwd *pw = getpwuid(getuid());
    if (pw != NULL) {
        *username = pw->pw_name;
        return 1;
    } else {
        perror("getpwuid");
        return -1;
    }
}

char *replace_home_with_tilde(const char *cwd, const char *home) {
    if (strncmp(cwd, home, strlen(home)) == 0) {
        static char modified_path[4096];
        snprintf(modified_path, sizeof(modified_path), "~%s", cwd + strlen(home));
        return modified_path;
    } else {
        return (char *)cwd;
    }
}

void display(int t)
{
    char *hostname;
    char *username;
    char cwd[4096];

    if (get_username(&username) == -1) {
        exit(EXIT_FAILURE);
    }

    if (get_hostname(&hostname) == -1) {
        exit(EXIT_FAILURE);
    }

    getcwd(cwd, sizeof(cwd));
    char *shown_path = replace_home_with_tilde(cwd, b_dir);

    if (t > 2) {
        printf(COLOR_GREEN "%s@%s: " COLOR_BLUE "%s%s" COLOR_RESET "$ ", username, hostname, shown_path, late);
    } else {
        printf(COLOR_GREEN "%s@%s: " COLOR_BLUE "%s" COLOR_RESET"$ " ,username, hostname, shown_path);
    }
}