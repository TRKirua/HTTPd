#include "serv.h"

struct s_serv *serv_init(struct s_global *glob, struct s_vhosts *vhos)
{
    struct s_serv *server = malloc(sizeof(struct s_serv));
    if (!server)
        return NULL;

    server->global = glob;
    glob->pid_file = NULL;
    glob->log_file = NULL;
    glob->log = true;

    server->vhosts = vhos;
    vhos->server_name = NULL;
    vhos->port = NULL;
    vhos->ip = NULL;
    vhos->root_dir = NULL;
    vhos->default_file = "index.html";
    vhos->next = NULL;

    return server;
}

int verify_and_complete(void)
{
    if (!g_serv->global->log_file)
        g_serv->global->log_file = "stdout";

    if (!g_serv->global->pid_file)
        return 2;

    struct s_vhosts *tmp = g_serv->vhosts;
    while (tmp)
    {
        if (!tmp->server_name || !tmp->port || !tmp->ip || !tmp->root_dir)
            return 2;

        if (!tmp->default_file)
            tmp->default_file = "index.html";

        tmp = tmp->next;
    }

    return 0;
}

int command_launch(char *action, char *file, bool daemon)
{
    int create = create_server(file);
    if (create == 2)
        return 2;

    if (!strcmp(action, "start"))
        return serv_start(g_serv, daemon);

    if (!strcmp(action, "stop"))
    {
        FILE *read = fopen(g_serv->global->pid_file, "r");
        if (!read)
            return 0;

        char *str = malloc(6);
        fgets(str, 6, read);

        pid_t pid = atoi(str);

        if (kill(pid, 0) == 0)
            kill(pid, SIGTERM);
        else
            printf("Process is already not alive\n");

        free(str);
        return 0;
    }
    /*if (!strcmp(action, "reload"))
        return serv_reload();
    if (!strcmp(action, "restart"))
        return serv_restart();*/
    else
        return -1;
    return 0;
}