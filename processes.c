#include "processes.h"
#include "syscom.h"
extern int bg_count;
extern BgProcess bg_processes[MAX_BG_PROCS];
extern int foreground_pgid;
extern char *fore;
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
void bg(pid_t pid)
{
    int flag = 0;
    for (int i = 0; i < bg_count; i++)
    {
        if (bg_processes[i].pid == pid)
        {
            flag = 1;
            break;
        }
    }
    if (flag)
    {

        if (kill(pid, SIGCONT) == -1)
        {
            perror("Failed to continue the process");
            return;
        }
    }
}

void fg(pid_t pid)
{

    BgProcess *process = NULL;
    foreground_pgid = pid;
    for (int i = 0; i < bg_count; i++)
    {
        if (bg_processes[i].pid == pid)
        {
            process = &bg_processes[i];
            fore = bg_processes[i].command;
            break;
        }
    }

    if (process == NULL)
    {
        printf(COLOR_RED"No such process found\n"COLOR_RESET);
        return;
    }

    for (int i = 0; i < bg_count; i++)
    {
        if (bg_processes[i].pid == pid)
        {
            for (int j = i; j < bg_count - 1; j++)
            {
                bg_processes[j] = bg_processes[j + 1];
            }
            remove_p(pid);
            bg_count--;
            break;
        }
    }

    if (kill(pid, SIGCONT) == -1)
    {
        perror("Failed to continue the process");
        return;
    }
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
}
