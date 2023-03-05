#include "parser.h"

void ParseFileGlobal(char *line, struct s_global *s_global)
{
    bool has_pid_file = false;
    bool has_log_file = false;
    bool has_log = false;

    char *saveptr = NULL;
    char *token = NULL;

    if (!(token = strtok_r(line, " ", &saveptr)))
        return;

    if (strcmp(token, "log_file") == 0)
        has_log_file = true;

    else if (strcmp(token, "pid_file") == 0)
        has_pid_file = true;

    else if (strcmp(token, "log") == 0)
        has_log = true;

    while (*saveptr == ' ' || *saveptr == '=')
        saveptr++;

    token = strtok_r(NULL, " ", &saveptr);

    char *s = malloc(strlen(token) + 1);
    s = strcpy(s, token);

    if (has_log_file)
        s_global->log_file = s;

    else if (has_log)
    {
        if (strcmp(token, "false") == 0)
            s_global->log = false;
        free(s);
    }

    else if (has_pid_file)
        s_global->pid_file = s;
}

void ParseFileVhosts(char *line, struct s_vhosts *s_vhosts)
{
    bool has_server_name = false;
    bool has_port = false;
    bool has_ip = false;
    bool has_root_dir = false;
    bool has_default_file = false;

    char *saveptr = NULL;
    char *token = NULL;

    if (!(token = strtok_r(line, " ", &saveptr)))
        return;

    if (strcmp(token, "server_name") == 0)
        has_server_name = true;

    else if (strcmp(token, "port") == 0)
        has_port = true;

    else if (strcmp(token, "ip") == 0)
        has_ip = true;

    else if (strcmp(token, "root_dir") == 0)
        has_root_dir = true;

    else if (strcmp(token, "default_file") == 0)
        has_default_file = true;

    while (*saveptr == ' ' || *saveptr == '=')
        saveptr++;

    if (!(token = strtok_r(NULL, " ", &saveptr)))
        return;

    char *s = malloc(strlen(token) + 1);
    s = strcpy(s, token);

    if (has_server_name)
        s_vhosts->server_name = s;

    else if (has_port)
        s_vhosts->port = s;

    else if (has_ip)
        s_vhosts->ip = s;

    else if (has_root_dir)
        s_vhosts->root_dir = s;

    else if (has_default_file)
        s_vhosts->default_file = s;
}

void print_server(void)
{
    printf("----------------------GLOBAL----------------------");

    printf("\nPID FILE : %s\n", g_serv->global->pid_file);
    printf("LOG FILE : %s\n", g_serv->global->log_file);
    printf("LOG : %s\n", g_serv->global ? "TRUE" : "FALSE");

    printf("----------------------VHOSTS----------------------");

    struct s_vhosts *tmp = g_serv->vhosts;
    while (tmp)
    {
        printf("\nSERVER NAME  : %s\n", tmp->server_name);
        printf("PORT : %s\n", tmp->port);
        printf("IP : %s\n", tmp->ip);
        printf("ROOT DIR : %s\n", tmp->root_dir);
        printf("DEFAULT FILE : %s\n", tmp->default_file);
        printf("--------------------------------------------------\n\n");
        tmp = tmp->next;
    }
}

int create_server(char *conf)
{
    FILE *file = fopen(conf, "r");
    if (!file)
    {
        printf("Cannot open file\n");
        return 2;
    }

    char *line_buf = NULL;
    size_t line_buf_size = 0;

    ssize_t line_size;
    line_size = getline(&line_buf, &line_buf_size, file);

    line_buf[strcspn(line_buf, "\r\n")] = 0;

    if (strcmp(line_buf, "[global]") != 0)
    {
        free(line_buf);
        printf("Configuration file is invalid\n");
        return 2;
    }

    bool isvhosts = false;

    line_size = getline(&line_buf, &line_buf_size, file);
    line_buf[strcspn(line_buf, "\r\n")] = 0;

    struct s_global *glob = malloc(sizeof(struct s_global));
    if (!glob)
    {
        free(line_buf);
        printf("Error in allocating global struct\n");
        return 2;
    }

    struct s_vhosts *vhos = malloc(sizeof(struct s_vhosts));
    if (!vhos)
    {
        free(glob);
        free(line_buf);
        printf("Error in allocating vhosts struct\n");
        return 2;
    }

    g_serv = serv_init(glob, vhos);

    while (line_size >= 0)
    {
        if (strcmp(line_buf, "[[vhosts]]") == 0)
            isvhosts = true;

        if (!isvhosts)
            ParseFileGlobal(line_buf, glob);
        else
            ParseFileVhosts(line_buf, vhos);

        line_size = getline(&line_buf, &line_buf_size, file);
        line_buf[strcspn(line_buf, "\r\n")] = 0;
    }

    free(line_buf);
    fclose(file);

    if (verify_and_complete())
    {
        printf("Missing mandatory key. Not a valid config file\n");
        return 2;
    }

    // print_server();
    return 0;
}