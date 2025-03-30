#include "iman.h"
#define COLOR_RED "\033[31m"
void remove_tags(char *str)
{
    regex_t regex;
    regmatch_t match;
    const char *pattern = "<[^>]*>";
    char *src = str;
    char *dst = str;
    int offset = 0;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Regex compilation failed\n");
        return;
    }

    while (src[offset] != '\0')
    {
        int rc = regexec(&regex, src + offset, 1, &match, 0);
        if (rc == REG_NOMATCH)
        {
            strcpy(dst, src + offset);
            break;
        }
        else if (rc != 0)
        {
            fprintf(stderr, "Regex execution failed\n");
            regfree(&regex);
           return;
        }

        strncpy(dst, src + offset, match.rm_so);
        dst += match.rm_so;

        offset += match.rm_eo;
    }

    *dst = '\0'; 
    regfree(&regex);
}

void iman(char* command_name)
{
    char buffer[BUFFER_SIZE];
    char request[MAX_REQUEST_LEN];
    struct protoent *protoent;
    const char *hostname = "man.he.net";
    int request_len;
    int socket_file_descriptor;
    ssize_t nbytes_total, nbytes_last;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 80;
    request_len = snprintf(request, MAX_REQUEST_LEN,
                           "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\nConnection: close\r\n\r\n",
                           command_name);
    if (request_len >= MAX_REQUEST_LEN)
    {
        fprintf(stderr, "Request length too large: %d\n", request_len);
       return ;
    }

    protoent = getprotobyname("tcp");
    if (protoent == NULL)
    {
        perror("getprotobyname");
       return ;
    }

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (socket_file_descriptor == -1)
    {
        perror("socket");
       return ;
    }

    struct hostent *hostent = gethostbyname(hostname);
    if (hostent == NULL)
    {
        fprintf(stderr, "Error: gethostbyname(\"%s\")\n", hostname);
       return ;
    }

    memcpy(&sockaddr_in.sin_addr, hostent->h_addr_list[0], hostent->h_length);
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    if (connect(socket_file_descriptor, (struct sockaddr *)&sockaddr_in, sizeof(sockaddr_in)) == -1)
    {
        perror("connect");
       return ;
    }

    nbytes_total = 0;
    while (nbytes_total < request_len)
    {
        nbytes_last = write(socket_file_descriptor, request + nbytes_total, request_len - nbytes_total);
        if (nbytes_last == -1)
        {
            perror("write");
           return ;
        }
        nbytes_total += nbytes_last;
    }
    int body_offset = 175;
    int flag = 0;
    char *search_sentence = "Search Again";
    while ((nbytes_total = read(socket_file_descriptor, buffer, BUFFER_SIZE)) > 0)
    {
        remove_tags(buffer);
        if (strstr(buffer, search_sentence) != NULL)
        {
            flag = 1;
        }
        write(STDOUT_FILENO, buffer + body_offset, strlen(buffer + body_offset));
    }
    close(socket_file_descriptor);
    if (flag)
    {
        printf(COLOR_RED "ERROR: No matches for %s command\n", command_name );
    }
    return ;
}
