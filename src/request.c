#include "request.h"

char *verify_host(char *s, struct s_serv *g_serv, int *status)
{
    char *message = "OK";
    char *ip = strtok(s, ":\r\n");

    if (ip)
        ip[strcspn(ip, "\r\n")] = 0;

    if (ip && strcmp(ip, "localhost") != 0)
    {
        if (strcmp(ip, g_serv->vhosts->ip) != 0 && *status == 200)
        {
            message = "Bad Request";
            *status = 400;
        }

        char *port = strtok(NULL, "\n");
        if (port)
            port[strcspn(port, "\r\n")] = 0;

        if (port && strcmp(port, g_serv->vhosts->port) != 0 && *status == 200)
        {
            message = "Bad Request";
            *status = 400;
        }
    }

    return message;
}

char *concat(const char *s1, const char *s2)
{
    char *s3 = malloc(strlen(s1) + strlen(s2) + 1);

    if (!s3)
        return NULL;

    strcpy(s3, s1);
    strcat(s3, s2);
    return s3;
}

void log_write(struct s_serv *g_serv, int status, char **list)
{
    char *method = list[0];
    char *uri = list[1];
    char *date = list[2];

    if (g_serv->global->log)
    {
        FILE *fp = fopen(g_serv->global->log_file, "ab");

        if (fp)
        {
            if (status != 200)
            {
                fprintf(fp, "%s [%s] received Bad Request from %s\n", date,
                        g_serv->vhosts->server_name, g_serv->vhosts->ip);

                if (status == 405)
                {
                    fprintf(fp,
                            "%s [%s] responding with %d to %s for UNKNOWN on "
                            "\'%s\'\n",
                            date, g_serv->vhosts->server_name, status,
                            g_serv->vhosts->ip, uri);
                }

                else
                {
                    fprintf(fp, "%s [%s] responding with %d to %s\n", date,
                            g_serv->vhosts->server_name, status,
                            g_serv->vhosts->ip);
                }
            }

            else
            {
                fprintf(fp, "%s [%s] received %s on \'%s\' from %s\n", date,
                        g_serv->vhosts->server_name, method, uri,
                        g_serv->vhosts->ip);

                fprintf(fp,
                        "%s [%s] responding with 200 to %s with %s on \'%s\'\n",
                        date, g_serv->vhosts->server_name, g_serv->vhosts->ip,
                        method, uri);
            }
        }

        fclose(fp);
    }
}

char *IsNumber(char *number, int *status)
{
    char *message = "OK";

    for (size_t i = 0; i < strlen(number); i++)
    {
        if (number[i] >= '0' && number[i] <= '9')
            continue;
        else
        {
            message = "Bad Request";
            *status = 400;
        }
    }

    return message;
}

int request(char *reponse, char *buff, struct s_serv *g_serv)
{
    int status = 200;
    char *message = "OK";
    char *list[3];

    time_t timestamp = time(NULL);
    struct tm *pTime = localtime(&timestamp);

    char date[80];
    strftime(date, 80, "%a, %d %b %Y %T GMT", pTime);

    char *saveptr = NULL;
    char *method = strtok_r(buff, " ", &saveptr);
    list[0] = method;
    list[2] = date;

    if (!method || strcmp(method, "\n") == 0)
    {
        message = "Bad Request";
        status = 400;
    }

    if ((strcmp(method, "GET") != 0) && (strcmp(method, "HEAD") != 0)
        && status == 200)
    {
        message = "Method Not Allowed";
        status = 405;
    }

    char *uri = strtok_r(NULL, " ", &saveptr);
    list[1] = uri;

    if ((!uri || strcmp(uri, "\n") == 0) && status == 200)
    {
        message = "Bad Request";
        status = 400;
    }

    char text[4096];
    int cpt = 0;

    if (status == 200)
    {
        if (strcmp(uri, "/") == 0)
            uri =
                concat(g_serv->vhosts->root_dir, g_serv->vhosts->default_file);
        else
            uri = concat(g_serv->vhosts->root_dir, uri);

        FILE *f = fopen(uri, "r");
        free(uri);

        if (!f)
        {
            if (errno == EACCES)
            {
                message = "Forbidden";
                status = 403;
            }
            else
            {
                message = "Not Found";
                status = 404;
            }
        }

        char *prot = strtok_r(NULL, " \r\n", &saveptr);

        if ((!prot || strcmp(prot, "\n") == 0) && status == 200)
        {
            message = "Bad Request";
            status = 400;
        }

        else if ((strcmp(prot, "HTTP/1.1") != 0) && status == 200)
        {
            message = "HTTP Version Not Supported";
            status = 505;
        }

        int length = -1;
        int counter = -1;

        bool has_content_length = false;
        char *content = strtok_r(NULL, "\n", &saveptr);

        if (content != NULL)
        {
            content[strcspn(content, "\r\n")] = 0;

            while (content != NULL && status == 200)
            {
                if (strcmp(content, "") == 0)
                    break;

                char *debut = strtok_r(NULL, " ", &content);
                char *suite = content;

                if ((debut != NULL) && (strcmp(debut, "Host:") == 0))
                {
                    message = verify_host(suite, g_serv, &status);
                }

                if ((debut != NULL) && (strcmp(debut, "Content-length:") == 0)
                    && (status == 200))
                {
                    suite[strcspn(suite, "\r\n")] = 0;
                    message = IsNumber(suite, &status);

                    if (status == 200)
                    {
                        has_content_length = true;

                        length = atoi(suite);
                    }

                    else
                        break;
                }

                if ((debut != NULL) && (strcmp(debut, "Connection:") == 0)
                    && (status == 200))
                {
                    suite[strcspn(suite, "\r\n")] = 0;

                    if (strcmp(suite, "close") != 0 && status == 200)
                    {
                        message = "Bad Request";
                        status = 400;
                    }

                    if (status != 200)
                        break;
                }

                content = strtok_r(NULL, "\n", &saveptr);
                if (content != NULL && (status == 200))
                {
                    content[strcspn(content, "\r\n")] = 0;
                }
            }

            if (content != NULL)
            {
                if (strcmp(content, "") == 0)
                {
                    if (has_content_length)
                    {
                        content = strtok_r(NULL, "\r\n", &saveptr);

                        if (content == NULL && status == 200)
                        {
                            message = "Bad Request";
                            status = 400;
                        }

                        else
                        {
                            for (size_t i = 0; i < strlen(content); i++)
                                counter++;

                            counter += 2;

                            if (length != counter && status == 200)
                            {
                                message = "Bad Request";
                                status = 400;
                            }
                        }
                    }

                    else
                    {
                        content = strtok_r(NULL, "\r\n", &saveptr);
                        if (content != NULL && status == 200)
                        {
                            for (size_t i = 0; i < strlen(content); i++)
                                counter++;
                        }
                    }
                }
            }

            if (status == 200
                && ((has_content_length && counter == -1)
                    || (!has_content_length && counter != -1)))
            {
                message = "Bad Request";
                status = 400;
            }
        }

        if (f)
        {
            int j = 0;

            char *line_buf = NULL;

            size_t line_buf_size = 0;
            ssize_t line_size;

            line_size = getline(&line_buf, &line_buf_size, f);

            while (line_size >= 0)
            {
                for (size_t i = 0; i < strlen(line_buf); i++)
                {
                    text[j] = line_buf[i];
                    cpt++;
                    j++;
                }

                line_size = getline(&line_buf, &line_buf_size, f);
            }

            text[j] = '\0';
            fclose(f);
            free(line_buf);
        }
    }

    if (status == 200 && strcmp(method, "GET") == 0)
    {
        sprintf(reponse,
                "HTTP/1.1 %d %s\r\nDate: %s\r\nServer: %s\r\nContent-Length: "
                "%d\r\nConnection: close\r\n\r\n%s",
                status, message, date, g_serv->vhosts->server_name, cpt, text);
    }

    else
    {
        sprintf(reponse,
                "HTTP/1.1 %d %s\r\nDate: %s\r\nServer: %s\r\nContent-Length: "
                "%d\r\nConnection: close\r\n",
                status, message, date, g_serv->vhosts->server_name, cpt);
    }

    log_write(g_serv, status, list);
    return status;
}