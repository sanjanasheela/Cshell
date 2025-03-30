#include "Neonate.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>  
#include <signal.h>     

int stop = 0;
extern int foreground_pgid;

int get_last_pid()
{
    FILE *file = fopen("/proc/loadavg", "r");
    if (!file)
    {
        perror("fopen");
        return -1;
    }

    char buffer[1023];
    if (!fgets(buffer, sizeof(buffer), file))
    {
        perror("fgets");
        fclose(file);
        return -1;
    }
    fclose(file);

    char *last_field = strrchr(buffer, ' ');
    if (last_field == NULL)
    {
        fprintf(stderr, "Failed to find the last field in /proc/loadavg\n");
        return -1;
    }
    last_field++;

    int pid;
    if (sscanf(last_field, "%d", &pid) != 1)
    {
        fprintf(stderr, "Failed to parse PID from /proc/loadavg\n");
        return -1;
    }

    return pid;
}

void set_terminal_mode(int fd, struct termios *old_termios)
{
    struct termios new_termios;

    if (tcgetattr(fd, old_termios) != 0)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    new_termios = *old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    if (tcsetattr(fd, TCSANOW, &new_termios) != 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void reset_terminal_mode(int fd, struct termios *old_termios)
{
    if (tcsetattr(fd, TCSANOW, old_termios) != 0)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}



void neno(int interval)
{
    struct termios old_termios;
    set_terminal_mode(STDIN_FILENO, &old_termios);

    while (!stop)
    {
        int fd = STDIN_FILENO;
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000; 

        int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);
        if (result > 0 && FD_ISSET(fd, &read_fds))
        {
            char c;
            if (read(fd, &c, 1) > 0)
            {
                if (c == 'x')
                {
                    stop = 1;
                }
            }
        }

        int pid = get_last_pid();
        if (pid != -1)
        {
            printf("%d\n", pid);
        }
        else
        {
            printf("No PID found.\n");
        }
        fflush(stdout);
        sleep(interval);
    }

    reset_terminal_mode(STDIN_FILENO, &old_termios);
}

int nenoate(int interval)
{
    stop = 0;
    neno(interval);
    return EXIT_SUCCESS;
}
