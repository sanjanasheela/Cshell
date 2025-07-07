#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "aliases.h"
#include "hop.h"
#include "display.h"
#include "log.h"
#include "reveal.h"
#include "seek.h"
#include "proclore.h"
#include "syscom.h"
#include "time.h"
#include "io.h"
#include "pipe.h"
#include "activites.h"
#include "iman.h"
#include "signal.h"
#include "Neonate.h"
#include "processes.h"
extern int c;
extern BgProcess bg_processes[MAX_BG_PROCS];
extern int bg_count;
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
extern int foreid;
#define maxpathlength 4096
extern char b_dir[maxpathlength];
extern char prev_path[MAX_PATH_LENGTH];
char late[maxpathlength] = "";
extern int fid;
extern char *fore;
extern int foreground_pgid;
void sys(char **args, int i)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        printf(COLOR_RED "please enter vaild command\n" COLOR_RESET);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        foreground_pgid = pid;
        int start = time(NULL);
        int status;
        waitpid(pid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
        {
            bg_processes[bg_count].pid = pid;
            strncpy(bg_processes[bg_count].command, fore, 1024);
            add_p(pid, fore, 1);
            bg_count++;
            return;
        }
        int end = time(NULL);
        int elapsed = difftime(end, start);
        if (elapsed > 2)
        {
            int t = (int)elapsed;

            strcpy(late, args[0]);
            int h = t;
            char ti[maxpathlength];
            strcat(late, " : ");
            sprintf(ti, "%d", h);
            strcat(late, ti);
            strcat(late, " s");
            c = t;
        }
    }
    else
    {
        perror(COLOR_RED "fork failed" COLOR_RESET);
        exit(EXIT_FAILURE);
    }
    return;
}

char *trimspace(char *str)
{
    char *end;
    while (isspace((char)*str))
    {
        str++;
    }
    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((char)*end))
    {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}

void execute_command(char *argv[maxcommandlength], int i)
{
   
    fid = getpid();
    if (i == 0)
    {
        printf(COLOR_RED "please enter a valid command\n" COLOR_RESET);
        return;
    }

    if (strcmp(argv[0], "hop") == 0)
    {
        if (i == 1)
        {
            hop_to_directory(b_dir, prev_path, b_dir);
        }

        for (int j = 1; j < i; j++)
        {
            hop_to_directory(argv[j], prev_path, b_dir);
        }
    }
    else if (strcmp(argv[0], "log") == 0)
    {
        if (i == 1)
        {
            print();
        }
        else
        {
            if (strcmp(argv[1], "purge") == 0)
            {
                clear();
            }

            if (strcmp(argv[1], "execute") == 0)
            {
                if (i <= 2)
                {
                    printf(COLOR_RED "please enter the command number to be executed\n" COLOR_RESET);
                    return;
                }
                else
                {
                    char c;
                    c = argv[2][0];
                    int a = c - '0';
                    execute_c(a);
                }
            }
        }
    }
    else if (strcmp(argv[0], "reveal") == 0)
    {
        char *path = (char *)malloc(sizeof(char) * (maxpathlength));
        int hidden = 0;
        int all = 0;
        path = getcwd(path, maxpathlength);
        for (int j = 1; j < i; j++)
        {
            if (argv[j][0] == '-')    
            {
                if (strlen(argv[j]) == 1)
                {
                    strcpy(path, argv[j]);
                    break;
                }
                for (int l = 1; l < strlen(argv[j]); l++)
                {
                    if (argv[j][l] == 'l')
                    {
                        all = 1;
                    }
                    if (argv[j][l] == 'a')
                    {
                        hidden = 1;
                    }
                }
            }
            else
            {
                strcpy(path, argv[j]);
            }
        }
        reveal(path, hidden, all);
    }
    else if (strcmp(argv[0], "seek") == 0)
    {
        char *search = (char *)malloc(sizeof(char) * (maxpathlength));
        char *path = (char *)malloc(sizeof(char) * (maxpathlength));
        int files = 0;
        int dirs = 0;
        int effective = 0;
        int s = 0;
        char cwd[maxpathlength];
        getcwd(cwd, sizeof(cwd));
        strcpy(path, cwd);
        for (int j = 1; j < i; j++)
        {
            if (strcmp(argv[j], "-f") == 0)
            {
                files = 1;
            }
            else if (strcmp(argv[j], "-d") == 0)
            {
                dirs = 1;
            }
            else if (strcmp(argv[j], "-e") == 0)
            {
                effective = 1;
            }
            else if (argv[j][0] == '-')
            {
                if (strlen(argv[j]) != 1)
                {
                    printf(COLOR_RED "INVALID FLAG\n" COLOR_RESET);
                    return;
                }
                else
                {
                    strcpy(path, argv[j]);
                }
            }
            else
            {
                if (s == 0)
                {
                    strcpy(search, argv[j]);
                    s = 1;
                }
                else
                {
                    strcpy(path, argv[j]);
                }
            }
        }
        if (dirs == 1 && (files == 1))
        {
            printf(COLOR_RED "INVALID FLAG\n" COLOR_RESET);
            return;
        }
        seekl(dirs, files, effective, path, search);
    }
    else if (strcmp(argv[0], "proclore") == 0)
    {
        int id = getpid();
        char pid[maxcommandlength];
        if (i > 2)
        {
            printf(COLOR_RED "Please enter vaild command\n" COLOR_RESET);
            return;
        }
        if (i == 1)
        {
            proclore(id);
        }
        else
        {
            strcpy(pid, argv[1]);
            id = atoi(pid);
            proclore(id);
        }
    }
    else if (strcmp(argv[0], "activities") == 0)
    {
        if (i > 1)
        {
            printf(COLOR_RED "enter the vaild command" COLOR_RESET);
        }
        else
        {
            print_activities();
        }
    }
    else if (strcmp(argv[0], "iMan") == 0)
    {
        if (i > 1)
        {
            iman(argv[1]);
        }
        else
        {
            printf(COLOR_RED "Enter valid command\n" COLOR_RESET);
        }
    }
    else if (strcmp(argv[0], "ping") == 0)
    {
        if(i <= 2 ){
            printf(COLOR_RED"please enter vaild command\n"COLOR_RESET);
            return;
        }
        int pid = atoi(argv[1]);
        int sig = atoi(argv[2]);
        send_sig(pid, sig);
    }
    else if (strcmp(argv[0], "neonate") == 0)
    {
        if (strcmp(argv[1], "-n") == 0)
        {
            if (i > 2)
            {
                nenoate(atoi(argv[2]));
            }
            else
            {
                printf(COLOR_RED "Enter vaild command\n" COLOR_RESET);
            }
        }
        else
        {
            printf(COLOR_RED "enter vaild command\n" COLOR_RESET);
        }
    }
    else if (strcmp(argv[0], "fg") == 0)
    {
        if (i > 1)
        {
            fg(atoi(argv[1]));
        }
        else
        {
            printf(COLOR_RED "Enter vaild command\n" COLOR_RESET);
        }
    }
    else if (strcmp(argv[0], "bg") == 0)
    {
        if (i > 1)
        {
            bg(atoi(argv[1]));
        }
        else
        {
            printf(COLOR_RED "Enter vaild command\n" COLOR_RESET);
        }
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
        exit(0);
    }
    else
    {
        sys(argv, i);
    }
    return;
}

void execute(char input[MAX_INPUT_LENGTH])
{
    char *commands[maxcommandlength];
    char *cmd;
    int cmd_count, i;
    trimspace(input);
    cmd_count = 0;
    cmd = strtok(input, ";");
    while (cmd != NULL)
    {
        commands[cmd_count++] = trimspace(cmd);
        cmd = strtok(NULL, ";");
    }
    for (i = 0; i < cmd_count; i++)
    {
        if (strchr(commands[i], '&'))
        {
            syscom(commands[i]);
        }
        else
        {
            execute_pipeline(commands[i]);
        }
    }
    return;
}
