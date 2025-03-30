#include "display.h"
#include "hop.h"
#include "input.h"
#include "log.h"
#include "seek.h"
#include "reveal.h"
#include "proclore.h"
#include <signal.h>
#include "syscom.h"
#include "aliases.h"
#include <unistd.h>
extern BgProcess bg_processes[MAX_BG_PROCS];
extern int bg_count;
char b_dir[MAX_PATH_LENGTH];
char *fore;
int c;
int fid;
int foreground_pgid;
extern int stop;
void sigint_handler(int sig)
{
    if (foreground_pgid > 0)
    {
        kill(foreground_pgid, SIGINT);
        printf("\n");
    }
    else
    {
        printf("\n");
        return;
    }
}
void sigstp_handler(int sig)
{
    if (foreground_pgid > 0)
    {
        kill(foreground_pgid, sig);
        printf("\n");
    }
    else
    {
        printf("\n");
        return;
    }
}

int main()
{
    c = 0;
    fore = (char *)malloc(sizeof(char) * (1024));
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigstp_handler);
    char input[MAX_INPUT_LENGTH];
    getcwd(b_dir, sizeof(b_dir));
    fid = -1;
    parse_myshrc();
    while (1)
    {
        foreground_pgid = -1;
        display(c);
        c = 0;
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            kill_all_bg_processes();
            printf("\n");
            break;
        }
        if (strcmp(input, "\n") == 0)
        {
            continue;
        }
        store(input);
        char *s = aliase(input);
        if (s == NULL)
        {
            execute(input);
        }
        else
        {
            execute(s);
        }
    }

    return 0;
}