#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "io.h"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
void execute_com(char *cmd)
{
    char *argv[maxcommandlength];
    char *token = strtok(cmd, " \t");
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
    int i = 0;
    int stdin_backup = dup(STDIN_FILENO);
    int stdout_backup = dup(STDOUT_FILENO);

    while (token != NULL)
    {
        if (strcmp(token, ">") == 0)
        {
            output_file = strtok(NULL, " ");
        }
        else if (strcmp(token, ">>") == 0)
        {
            output_file = strtok(NULL, " ");
            append = 1;
        }
        else if (strcmp(token, "<") == 0)
        {

            input_file = strtok(NULL, " ");
        }
        else if (strchr(token, '>'))
        {
            char *s = NULL;
            s = strtok(token, ">");

            if (token != NULL)
            {
                while (*token == ' ')
                    token++; // Trim leading spaces
                char *end = token + strlen(token) - 1;
                while (*end == ' ')
                    end--; // Trim trailing spaces
                *(end + 1) = '\0';

                output_file = strtok(NULL, ">");

                if (output_file != NULL)
                {
                    while (*output_file == ' ')
                        output_file++; // Trim leading spaces
                    end = output_file + strlen(output_file) - 1;
                    while (*end == ' ')
                        end--; // Trim trailing spaces
                    *(end + 1) = '\0';
                    argv[i++] = s;
                }
                if (output_file == NULL)
                {
                    output_file = s;
                }
            }
        }
        else if (strchr(token, '<'))
        {
            char *s = NULL;
            s = strtok(token, "<");
            
            if (token != NULL)
            {
                while (*token == ' ')
                    token++; // Trim leading spaces
                char *end = token + strlen(token) - 1;
                while (*end == ' ')
                    end--;
                *(end + 1) = '\0';

                input_file = strtok(NULL, "<");

                if (input_file != NULL)
                {
                    while (*input_file == ' ')
                        input_file++;
                    end = input_file + strlen(input_file) - 1;
                    while (*end == ' ')
                        end--;
                    *(end + 1) = '\0';
                    argv[i++] = s;
                }
                if(input_file == NULL){
                    input_file=s;
                }
            }
        }
        else
        {
            argv[i++] = token;
        }
        token = strtok(NULL, " \t");
    }
    argv[i] = NULL;

    if (input_file)
    {
        int fd_in = open(input_file, O_RDONLY);
        if (fd_in == -1)
        {
            perror("No such input file found!");
            return;
        }
        dup2(fd_in, STDIN_FILENO);
        close(fd_in);
    }
    if (output_file)
    {
        int fd_out;
        if (append)
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd_out == -1)
        {
            perror(COLOR_RED"Error opening output file"COLOR_RESET);
            // exit(1);
            return;
        }
        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    execute_command(argv, i);
    dup2(stdin_backup, STDIN_FILENO);
    dup2(stdout_backup, STDOUT_FILENO);
    close(stdin_backup);
    close(stdout_backup);
    fflush(stdout);
}
