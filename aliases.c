#include "aliases.h"
Alias aliases[MAX_ALIASES];
int alias_count = 0;
extern char b_dir[1024];
void add_alias(const char *alias, const char *command)
{
    if (alias_count < MAX_ALIASES)
    {
        strncpy(aliases[alias_count].alias, alias, MAX_ALIAS_LENGTH);
        strncpy(aliases[alias_count].command, command, MAX_COMMAND_LENGTH);
        alias_count++;
    }
    else
    {
        fprintf(stderr, "Maximum number of aliases reached.\n");
    }
}

char *find_alias_command(const char *alias)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].alias, alias) == 0)
        {
            return aliases[i].command;
        }
    }
    return NULL;
}
void parse_myshrc()
{
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%.1009s/myshrc.bashrc", b_dir);
    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        perror("Failed to open myshrc");
        return;
    }

    char line[MAX_COMMAND_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        char *alias_key = strtok(line, " ='\"\n");
        if (alias_key && strcmp(alias_key, "alias") == 0)
        {
            char *alias_name = strtok(NULL, " ='\"\n");
            char *alias_command = strtok(NULL, "='\"\n");
            if (alias_name && alias_command)
            {
                add_alias(alias_name, alias_command);
            }
        }
    }
    fclose(file);
}

char *aliase(char *command)
{
    command[strcspn(command, "\n")] = 0;

    char *alias_command = find_alias_command(command);
    return alias_command;
}
