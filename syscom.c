#include "syscom.h"
#include "input.h"
#include "pipe.h"
#include "io.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
extern int foreground_pgid;
extern char late[MAX_PATH_LENGTH];
extern int c;
BgProcess bg_processes[MAX_BG_PROCS];
int bg_count = 0;
int fg_pid = -1;
char *trimspe(char *str)
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

void handle_sigchld(int sig)
{
    int status;
    int pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
    {
        for (int i = 0; i < bg_count; i++)
        {
            if (bg_processes[i].pid == pid)
            {
                if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    update_p(pid, terminated);
                    remove_p(pid);
                    printf("\n%s exited normally (%d)\n", bg_processes[i].command, pid);
                    for (int j = i; j < bg_count - 1; j++)
                    {
                        bg_processes[j] = bg_processes[j + 1];
                    }
                    bg_count--;
                    break;
                }
                else
                {
                    update_p(pid, terminated);
                    remove_p(pid);
                    printf("\n%s exited abnormally (%d)\n", bg_processes[i].command, pid);
                    for (int j = i; j < bg_count - 1; j++)
                    {
                        bg_processes[j] = bg_processes[j + 1];
                    }
                    bg_count--;
                    break;
                }
                fflush(stdout);
            }
        }
    }
}

void trim_newline(char *str)
{
    str[strcspn(str, "\n")] = '\0';
}

void run_foreground(char **args, char *command)
{
    int pid = fork();

    fg_pid = pid;
    if (pid == 0)
    {
        execute_pipeline(command);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        int start = time(NULL);
        int status;
        waitpid(pid, &status, WUNTRACED);
        int end = time(NULL);
        
        int elapsed = difftime(end, start);
        if (elapsed > 2.0)
        {
            char a[strlen(command) + 1];
            strcpy(a, command);
            char *args[MAX_ARGS];
            char *token = strtok(a, " ");
            int argc = 0;

            while (token != NULL && argc < MAX_ARGS - 1)
            {
                args[argc++] = token;
                token = strtok(NULL, " ");
            }
            args[argc] = NULL;
            int t = (int)elapsed;
            strcpy(late, args[0]);
            int h = t;
            char ti[1000];

            strcat(late, " : ");
            sprintf(ti, "%d", h);
            strcat(late, ti);
            strcat(late, " s");

            c = t;
        }
    }
    else
    {
        perror("fork failed");
    }
}

void run_background(char **args, char *command)
{
    int pid = fork();
    if (pid == 0)
    {
        setpgid(0, 0);
        execute_pipeline(command);
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0)
    {
        printf("%d\n", pid);
        if (bg_count < MAX_BG_PROCS)
        {
            bg_processes[bg_count].pid = pid;
            strncpy(bg_processes[bg_count].command, command, 1024);
            add_p(pid, command, 1);
            bg_count++;
        }
        else
        {
            printf(COLOR_RED"Max background processes limit reached.\n"COLOR_RESET);
        }
    }
    else
    {
        perror("fork failed");
    }
}



void kill_all_bg_processes()
{
    for (int i = 0; i < bg_count; i++)
    {
        kill(bg_processes[i].pid, SIGKILL);
        waitpid(bg_processes[i].pid, NULL, 0);
    }
    bg_count = 0;
    return;
}

void command(char *input, int bg)
{
    char *args[MAX_ARGS];
    if (bg)
    {
        run_background(args, input);
    }
    else
    {
        run_foreground(args, input);
    }
}

void syscom(char *input)
{
    signal(SIGCHLD, handle_sigchld);
    trim_newline(input);
    if (strcmp(input, "exit") == 0)
    {
        kill_all_bg_processes();
        exit(0);
    }
    int flag = 1;
    trimspe(input);
    if (input[strlen(input) - 1] == '&')
    {
        flag = 0;
    }

    char *bg_commands[MAX_ARGS];
    int count = 0;
    char *bg_command = strtok(input, "&");
    while (bg_command != NULL)
    {
        bg_commands[count++] = trimspe(bg_command);
        bg_command = strtok(NULL, "&");
    }
    int v = count - 1;
    if (flag == 0)
    {
        v++;
    }
    for (int i = 0; i < v; i++)
    {
        command(bg_commands[i], 1);
    }
    if (flag)
    {
        command(bg_commands[v], 0);
    }
}
