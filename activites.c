#include "activites.h"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
typedef struct process
{
    int pid;
    char *command;
    int state;
    struct process *next;
} process;

process *p_head = NULL;
void add_p(int pid, char *command, int state)
{
    process *p = (process *)malloc(sizeof(process));
    p->pid = pid;
    p->command = (char *)malloc(sizeof(char) * (1024));
    strcpy(p->command, command);
    p->state = state;
    p->next = p_head;
    p_head = p;
}

void update_p(int pid, int state)
{
    process *c = p_head;
    while (c)
    {
        if (c->pid == pid)
        {
            c->state = state;
            return;
        }
        c = c->next;
    }
}

void remove_p(int pid)
{
    process *c = p_head;
    process *prev = NULL;

    while (c)
    {
        if (c->pid == pid)
        {
            if (prev)
            {
                prev->next = c->next;
            }
            else
            {
                p_head = c->next;
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

int compare_processes(const void *a, const void *b)
{
    process *procA = *(process **)a;
    process *procB = *(process **)b;
    return strcmp(procA->command, procB->command);
}
char get_process_state(pid_t pid)
{
    char path[1024], line[1024], state;
    FILE *status_file;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    status_file = fopen(path, "r");
    if (status_file == NULL)
    {
        perror(COLOR_RED "Failed to open status file" COLOR_RESET);
        return 'n';
    }

    while (fgets(line, sizeof(line), status_file))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            sscanf(line, "State: %c", &state);
            fclose(status_file);
            return state;
        }
    }

    fclose(status_file);
    return 'n';
}

void print_activities()
{
    int count = 0;
    for (process *c = p_head; c; c = c->next)
    {
        count++;
    }

    process **processes = malloc(count * sizeof(process *));
    process *c = p_head;
    for (int i = 0; i < count; i++)
    {
        processes[i] = c;
        c = c->next;
    }

    qsort(processes, count, sizeof(process *), compare_processes);
    for (int i = 0; i < count; i++)
    {

        char c = get_process_state(processes[i]->pid);

        if (c == 'R' || (c == 'S'))
        {
            printf("%d : %s - Running\n", processes[i]->pid, processes[i]->command);
        }
        else if (c == 'T')
        {
            printf("%d : %s - Stopped\n", processes[i]->pid, processes[i]->command);
        }
        else
        {
            remove_p(processes[i]->pid);
        }
    }
    free(processes);
}
