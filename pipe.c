#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "io.h"
extern char* fore; 
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
void trim_whitespace(char *str)
{
    while (*str == ' ' || *str == '\t')
    {
        str++;
    }

    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t'))
    {
        *end = '\0';
        end--;
    }
}

int validate_input(const char *input)
{
    int len = strlen(input);

    if (input[0] == '|' || input[len - 1] == '|')
    {
        return 0;
    }

    for (int i = 0; i < len - 1; i++)
    {
        if (input[i] == '|' && input[i + 1] == '|')
        {
            return 0;
        }
    }

    return 1;
}

void execute_pipeline(char *input)
{
    strcpy(fore,input);
    if (strchr(input, '|') == NULL)
    {
        execute_com(input);
    }
    else
    {

        int stdin_backup = dup(STDIN_FILENO);
        int stdout_backup = dup(STDOUT_FILENO);

        if (!validate_input(input))
        {
            fprintf(stderr, COLOR_RED"Invalid use of pipe: missing command on one side of the pipe\n"COLOR_RESET);
            return;
        }

        char *commands[1024];
        int cmd_count = 0;

        char *token = strtok(input, "|");
        while (token != NULL && cmd_count < 1024)
        {
            commands[cmd_count++] = token;
            token = strtok(NULL, "|");
        }
        commands[cmd_count] = NULL;

        if (cmd_count == 0)
        {
            fprintf(stderr, COLOR_RED"Invalid use of pipe: no commands\n"COLOR_RESET);
            return;
        }

        int pipe_fds[2 * (cmd_count - 1)];

        for (int i = 0; i < cmd_count - 1; i++)
        {
            if (pipe(pipe_fds + 2 * i) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < cmd_count; i++)
        {
            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0)
            {
                if (i > 0)
                {
                    if (dup2(pipe_fds[2 * (i - 1)], STDIN_FILENO) == -1)
                    {
                        perror("dup2 input");
                        exit(EXIT_FAILURE);
                    }
                }
                if (i < cmd_count - 1)
                {
                    if (dup2(pipe_fds[2 * i + 1], STDOUT_FILENO) == -1)
                    {
                        perror("dup2 output");
                        exit(EXIT_FAILURE);
                    }
                }

                for (int j = 0; j < 2 * (cmd_count - 1); j++)
                {
                    close(pipe_fds[j]);
                }
                execute_com(commands[i]);
                exit(EXIT_SUCCESS);
            }
            int status;
            waitpid(pid, &status, 0);
            if (i > 0)
            {
                close(pipe_fds[2 * (i - 1)]);
            }
            if (i < cmd_count - 1)
            {
                close(pipe_fds[2 * i + 1]);
            }
        }
        
        for (int j = 0; j < 2 * (cmd_count - 1); j++)
        {
            close(pipe_fds[j]);
        }
        dup2(stdin_backup, STDIN_FILENO);
        dup2(stdout_backup, STDOUT_FILENO);
        close(stdin_backup);
        close(stdout_backup);
    }
}