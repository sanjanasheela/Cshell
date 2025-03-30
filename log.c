#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <wait.h>
#include "log.h"
#define MAX_PATH 1024
#include "input.h"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
int count = 0;
extern char b_dir[1000];
void loadcommands(char **log)
{
    FILE *file;
    char filepath[MAX_PATH];
    snprintf(filepath, sizeof(filepath), "%s/log.txt", b_dir);
    char buffer[1024];
    file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror(COLOR_RED"Error opening file"COLOR_RESET);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (strlen(buffer) > 0)
        {
            strcpy(log[count++], buffer);
        }
    }
    fclose(file);
    fflush(stdout);
}
void add(char **log, char command[MAX_INPUT_LENGTH])
{
    if (count < 15)
    {
        int flag = 1;
        if (count > 0)
        {
            if (strcmp(log[count - 1], command) == 0)
            {
                flag = 0;
            }
        }
        if (strstr(command, "log") != NULL)
        {
            flag = 0;
        }

        if (flag)
        {
            strcpy(log[count], command);
            count++;
        }
    }
    else
    {
        int flag = 1;
        if (count > 0)
        {
            if (strcmp(log[count - 1], command) == 0)
            {
                flag = 0;
            }
        }
        if (strstr(command, "log") != NULL)
        {
            flag = 0;
        }

        if (flag)
        {
            int v = count;

            for (int i = 0; i < v - 1; i++)
            {
                strcpy(log[i], log[i + 1]);
            }
            strcpy(log[count - 1], command);
            // count--;
        }
    }
}

void clear()
{
    char filepath[MAX_PATH];
    snprintf(filepath, sizeof(filepath), "%s/log.txt", b_dir);
    int o = open(filepath, O_WRONLY | O_TRUNC|O_CREAT, 0644);
    close(o);
}
void execute_c(int cno)
{
    count = 0;
    char **log;
    log = (char **)malloc(sizeof(char *) * (16));
    for (int i = 0; i < 16; i++)
    {
        log[i] = (char *)malloc(sizeof(char) * (MAX_INPUT_LENGTH));
    }
    loadcommands(log);
    if (cno <= count)
    {
        execute(log[cno - 1]);
    }
    else
    {
        printf(COLOR_RED"there is no command in the log with that number :%d"COLOR_RESET, cno);
        return;
    }
}
void print()
{
    FILE *file;
    char buffer[MAX_PATH];
    char filepath[MAX_PATH];
    snprintf(filepath, sizeof(filepath), "%s/log.txt", b_dir);
    file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening file");
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (strlen(buffer) > 0)
        {
            printf("%s", buffer);
        }
    }
    fclose(file);
    fflush(stdout);
}
void addtofile(char **log)
{
    char filepath[MAX_PATH];
    snprintf(filepath, sizeof(filepath), "%s/log.txt", b_dir);
    int o = open(filepath, O_WRONLY | O_TRUNC|O_CREAT, 0644);
    for (int i = 0; i < count; i++)
    {
        write(o, log[i], strlen(log[i]));
    }
    close(o);
}
void store(char input[MAX_INPUT_LENGTH])
{
    count = 0;
    char **log;
    log = (char **)malloc(sizeof(char *) * (16));
    for (int i = 0; i < 16; i++)
    {
        log[i] = (char *)malloc(sizeof(char) * (MAX_INPUT_LENGTH));
    }
    loadcommands(log);
    add(log, input);
    addtofile(log);
}