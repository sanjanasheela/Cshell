#include "reveal.h"
extern char b_dir[4096];
#define GREEN "\x1b[32m"
#define BLUE "\x1b[38;5;18m"
#define WHITE "\x1b[37m"
#define RED "\033[31m"
#define RESET "\x1b[0m"
char prepath[MAX_INPUT_LENGTH] = "";
int compare_entries(const struct dirent **a, const struct dirent **b)
{
    return strcmp((*a)->d_name, (*b)->d_name);
}

void print_permissions(mode_t mode)
{
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}
void print_file_info(const struct dirent *entry, const char *path, int show_hidden, int long_format)
{
    if (!show_hidden && entry->d_name[0] == '.')
        return;

    char full_path[4096];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    struct stat file_stat;
    if (stat(full_path, &file_stat) < 0)
    {
        perror("stat");
        return;
    }

    if (long_format)
    {
        char mode[11];
        mode[0] = S_ISDIR(file_stat.st_mode) ? 'd' : '-';
        mode[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        mode[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        mode[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
        mode[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        mode[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        mode[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
        mode[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
        mode[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
        mode[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
        mode[10] = '\0';

        struct passwd *pw = getpwuid(file_stat.st_uid);
        struct group *gr = getgrgid(file_stat.st_gid);
        char time_buf[20];
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));

        printf("%s %ld %s %s %6ld %s ", mode, file_stat.st_nlink, pw->pw_name, gr->gr_name, file_stat.st_size, time_buf);
    }

    if (S_ISDIR(file_stat.st_mode))
    {
        printf(BLUE "%s" RESET "\n", entry->d_name);
    }
    else if (file_stat.st_mode & S_IXUSR)
    {
        printf(GREEN "%s" RESET "\n", entry->d_name);
    }
    else
    {
        printf(WHITE "%s" RESET "\n", entry->d_name);
    }
}
void print_file_permissions(mode_t mode)
{
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void print_long_format(const char *filename, const struct stat *path_stat)
{
    print_file_permissions(path_stat->st_mode);

    printf(" %lu", path_stat->st_nlink);

    struct passwd *pw = getpwuid(path_stat->st_uid);
    printf(" %s", pw ? pw->pw_name : "UNKNOWN");

    struct group *gr = getgrgid(path_stat->st_gid);
    printf(" %s", gr ? gr->gr_name : "UNKNOWN");

    printf(" %ld", path_stat->st_size);

    char time_str[20];
    struct tm *tm_info = localtime(&path_stat->st_mtime);
    strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm_info);
    printf(" %s", time_str);

    printf(" %s\n", filename);
}

void reveal(char *path, int show_hidden, int long_format)
{
    struct dirent **entries;
    struct stat path_stat;

    if (strcmp(path, "-") == 0)
    {
        if (strcmp(prepath, "") == 0)
        {
            printf(RED "NO previous path\n" RESET);
            return;
        }
        else
        {
            strcpy(path, prepath);
        }
    }
    if (path[0] == '~')
    {
        strcpy(path, b_dir);
    }
    if (stat(path, &path_stat) != 0)
    {
        perror("stat");
        return;
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        int count = scandir(path, &entries, NULL, compare_entries);
        if (count < 0)
        {
            perror("scandir");
            return;
        }
        long total_blocks = 0;
        strcpy(prepath, path);
        for (int i = 0; i < count; i++)
        {
            if (!show_hidden && entries[i]->d_name[0] == '.')
                continue;

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]->d_name);

            struct stat file_stat;
            if (stat(full_path, &file_stat) < 0)
            {
                perror("stat");
                continue;
            }

            total_blocks += file_stat.st_blocks;
        }
        if (long_format)
        {
            printf("total %ld\n", total_blocks / 2);
        }
        for (int i = 0; i < count; i++)
        {
            print_file_info(entries[i], path, show_hidden, long_format);
            free(entries[i]);
        }

        free(entries);
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        if (S_ISREG(path_stat.st_mode))
        {
            if (long_format)
            {
                print_long_format(path, &path_stat);
            }
            else
            {
                printf("%s\n", path);
            }
        }
    }
}
char *resolve_path(char *path)
{
    char new_path[MAX_INPUT_LENGTH];
    char *resolved;
    resolved = (char *)malloc(sizeof(char) * (MAX_INPUT_LENGTH));
    if (path[0] == '~')
    {
        snprintf(new_path, sizeof(new_path), "%s%s", b_dir, path + 1);
    }
    else
    {
        snprintf(new_path, sizeof(new_path), "%s", path);
    }

    if (realpath(new_path, resolved) == NULL)
    {
        perror("realpath() error");
        return NULL;
    }
    return resolved;
}
