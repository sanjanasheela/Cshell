#include "display.h"
#include "input.h"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"
extern char b_dir[MAX_PATH_LENGTH];
extern char late[MAX_PATH_LENGTH];
int hname(char **hname)
{
    struct utsname buff; 
    if (uname(&buff) == 0)
    {
        *hname = buff.nodename;
        return 1;
    }
    else
    {
        perror("uname");
        return -1;
    }
}
void display(int t)
{
    char *hostname;
    hostname = (char *)malloc(sizeof(char) * (100));
    char cwd[4096];
    char *username;
    struct passwd *pw;
    pw = getpwuid(getuid());
    username = pw->pw_name;
    if (hname(&hostname) == -1)
    {
        perror("name");
        exit(EXIT_FAILURE);
    }
    getcwd(cwd, sizeof(cwd));
    if (strncmp(cwd, b_dir, strlen(b_dir)) == 0)
    {
        if (t > 2)
        {
            printf(COLOR_GREEN "%s@%s:" COLOR_BLUE "~%s%s $" COLOR_RESET, username, hostname, cwd + strlen(b_dir), late);
            t=0;
        }
        else
        {
            printf(COLOR_GREEN "%s@%s:" COLOR_BLUE "~%s $" COLOR_RESET, username, hostname, cwd + strlen(b_dir));
        }
    }
    else
    {
        if (t > 2)
        {
            printf(COLOR_GREEN "%s@%s: " COLOR_BLUE "%s %d $" COLOR_RESET, username, hostname, cwd, t);
            t=0;
        }
        else
        {
            printf(COLOR_GREEN "%s@%s: " COLOR_BLUE "%s $" COLOR_RESET, username, hostname, cwd);
        }
    }
}
