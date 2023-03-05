#include "dry.h"

int CheckFileGlobal(char *line, bool *has_pid_file)
{
    char *saveptr = NULL;
    char *token = NULL;

    if (!(token = strtok_r(line, " ", &saveptr)))
        return 0;

    if (!(strcmp(token, "log_file") == 0) && (!(strcmp(token, "log") == 0)))
    {
        if (strcmp(token, "pid_file") == 0)
            *has_pid_file = true;
        else
            return 2;
    }

    return 0;
}

int CheckFileVhosts(char *line, bool *mandatory)
{
    bool has_server_name = false;
    bool has_port = false;
    bool has_ip = false;
    bool has_root_dir = false;

    char *saveptr = NULL;
    char *token = NULL;

    if (!(token = strtok_r(line, " ", &saveptr)))
        return 0;

    if (strcmp(token, "server_name") == 0)
        has_server_name = true;

    else if (strcmp(token, "port") == 0)
        has_port = true;

    else if (strcmp(token, "ip") == 0)
        has_ip = true;

    else if (strcmp(token, "root_dir") == 0)
        has_root_dir = true;

    else if (!(strcmp(token, "default_file") == 0))
        return 2;
    else
        return 0;

    if (!has_server_name && !has_port && !has_root_dir && !has_ip)
        return 2;

    *mandatory = true;
    return 0;
}

int test_dryrun_command(char *conf)
{
    FILE *file = fopen(conf, "r");
    if (!file)
        return 2;

    char *line_buf = NULL;
    size_t line_buf_size = 0;

    ssize_t line_size;
    line_size = getline(&line_buf, &line_buf_size, file);

    line_buf[strcspn(line_buf, "\r\n")] = 0;

    if (strcmp(line_buf, "[global]") != 0)
    {
        free(line_buf);
        return 2;
    }

    bool isvhosts = false;
    bool mandatories[4] = { false };
    bool has_pid_file = false;

    line_size = getline(&line_buf, &line_buf_size, file);
    line_buf[strcspn(line_buf, "\r\n")] = 0;
    int i = 0;

    while (line_size >= 0)
    {
        if (strcmp(line_buf, "[[vhosts]]") == 0)
        {
            line_size = getline(&line_buf, &line_buf_size, file);
            line_buf[strcspn(line_buf, "\r\n")] = 0;
            i = 0;
            if (isvhosts)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (!mandatories[i])
                        return 2;
                    else
                        mandatories[i] = false;
                }
            }
            isvhosts = true;
        }

        if (!isvhosts)
        {
            if (CheckFileGlobal(line_buf, &has_pid_file) == 2)
                return 2;
        }
        else if (CheckFileVhosts(line_buf, &mandatories[i]) == 2)
            return 2;
        else
            i++;

        line_size = getline(&line_buf, &line_buf_size, file);
        line_buf[strcspn(line_buf, "\r\n")] = 0;
    }

    free(line_buf);
    fclose(file);

    if (isvhosts)
    {
        for (int i = 0; i < 4; i++)
        {
            if (!mandatories[i])
                return 2;
        }
    }
    else
        return 2;

    if (!has_pid_file)
        return 2;

    return 0;
}