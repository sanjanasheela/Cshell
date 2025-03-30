#include "proclore.h"
#include "input.h"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
int is_foreground_process(int pid)
{
    int tty_fd = open("/dev/tty", O_RDONLY);
    if (tty_fd == -1)
    {
        return 0;
    }
    int  fg_pgid = tcgetpgrp(tty_fd);
    close(tty_fd);

    char path[MAX_PATH_LENGTH];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror(COLOR_RED"Error opening status file"COLOR_RESET);
        return 0;
    }

    char line[256];
    int  pgid = 0;
    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "Tgid:\t%d", &pgid);
    }
    fclose(file);

    return (fg_pgid == pgid);
}

void print_process_info(int  pid)
{
    char path[MAX_PATH_LENGTH];
    char buffer[MAX_PATH_LENGTH];
    FILE *file;
    char state[4], line[256];
    int pgid;
    long vm_size = 0;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    file = fopen(path, "r");
    if (!file)
    {
        perror(COLOR_RED"Error opening status file"COLOR_RESET);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "Pid:\t%d", &pid) == 1)
        {
            continue;
        }
        else if (sscanf(line, "State:\t%s", state) == 1)
        {
            continue;
        }
        else if (sscanf(line, "VmSize:\t%ld", &vm_size) == 1)
        {
            continue;
        }
        else if (sscanf(line, "Tgid:\t%d", &pgid) == 1)
        {
            continue;
        }
    }
    fclose(file);

    if (is_foreground_process(pid))
    {
        strcat(state, "+");
}

    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
    }
    else
    {
        strcpy(buffer, "Executable path not found");
    }

    printf("pid : %d\n", pid);
    printf("process status : %s\n", state);
    printf("Process Group : %d\n", pgid);
    printf("Virtual Memory : %ld KB\n", vm_size);
    printf("executable path : %s\n", buffer);
}

void proclore(int pid)
{
    print_process_info(pid);
}
