#include "signal.h"
void send_signal(int pid, int signal_number)
{
    signal_number = signal_number % 32;

    if (kill(pid, signal_number) == -1)
    {
        if (errno == ESRCH)
        {
            printf("No such process found\n");
           return ;
        }
        else if (errno == EINVAL)
        {
            printf("Invalid signal number\n");
           return ;
        }
        else if (errno == EPERM)
        {
            printf("Permission denied to send signal\n");
           return ;
        }
        else
        {
            perror("Error sending signal");
        }
    }
}

void send_sig(int pid, int signal_number)
{
    send_signal(pid, signal_number);
}
