#include "seek.h"
#include "input.h"
extern char b_dir[MAX_PATH_LENGTH];

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"
char prevpath[MAX_PATH_LENGTH]="";
int search_files = 0;
int search_dirs = 0;
int match_count = 0;
int enable_effective = 0;

char single_file[MAX_PATH_LENGTH];
char single_dir[MAX_PATH_LENGTH];

int is_directory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
    {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

void print_file_content(const char *filepath)
{
    FILE *file;
    char buffer[256];
    file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
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
    printf("\n");
}

void seek(const char *base_path, const char *relative_path, const char *search_name)
{
    char path[MAX_PATH_LENGTH];
    struct dirent *dp;
    DIR *dir = opendir(base_path);
    if (!dir)
    {
        perror("opendir");
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", base_path, dp->d_name);
        char new_relative_path[MAX_PATH_LENGTH];
        snprintf(new_relative_path, sizeof(new_relative_path), "%s/%s", relative_path, dp->d_name);

        int is_dir = is_directory(path);

        if ((strstr(dp->d_name, search_name) != NULL) ||
            (strchr(search_name, '.') == NULL && strstr(dp->d_name, search_name) != NULL))
        {

            if ((is_dir && search_dirs) || (!is_dir && search_files) || (search_dirs && search_files))
            {
                match_count++;
                if (is_dir)
                {
                    printf(COLOR_BLUE "%s" COLOR_RESET "\n", new_relative_path);
                    if (enable_effective && match_count == 1)
                    {
                        strncpy(single_dir, path, sizeof(single_dir));
                        single_file[0] = '\0';
                    }
                }
                else
                {
                    printf(COLOR_GREEN "%s" COLOR_RESET "\n", new_relative_path);
                    if (enable_effective && match_count == 1)
                    {
                        strncpy(single_file, path, sizeof(single_file));
                        single_dir[0] = '\0';
                    }
                }
            }
        }

        if (is_dir)
        {
            seek(path, new_relative_path, search_name);
        }
    }
    closedir(dir);
}
void seekl(int files, int dirs, int enable, char *target_dir, char *search_name)
{
    if(strcmp(target_dir,"-") == 0){
        if(strlen(prevpath) == 0){
            printf("no prev path \n");
            return;
        }
        else{
            strcpy(target_dir,prevpath);
        }
    }
    search_dirs=0;
    search_files=0;
    enable_effective=0;
    match_count=0;
    if (files)
    {
        search_dirs = 1;
    }
    if (dirs)
    {
        search_files = 1;
    }
    if (enable)
    {
        enable_effective = 1;
    }
    if (!search_dirs && !search_files)
    {
        search_dirs = 1;
        search_files = 1;
    }
    if (strlen(target_dir) == 0)
    {
        char path1[MAX_PATH_LENGTH];
        getcwd(path1,sizeof(path1));
        strcpy(target_dir, path1);
    }
    if (strcmp(target_dir, "~") == 0)
    {
        strcpy(target_dir, b_dir);
    }
    seek(target_dir, "", search_name);
    strcpy(prevpath,target_dir);
    if (match_count == 0)
    {
        printf(COLOR_RED "No match found!\n" COLOR_RESET);
    }
    else if (enable_effective && match_count == 1)
    {
        if (single_file[0] != '\0')
        {
            print_file_content(single_file);
        }
        else if (single_dir[0] != '\0')
        {
            if (chdir(single_dir) == 0)
            {
                printf("Changed to directory: %s\n", single_dir);
            }
            else
            {
                printf(COLOR_RED "Missing permissions for task!\n" COLOR_RESET);
            }
        }
    }
}