#include "actions.h"

volatile sig_atomic_t loop = 1;

void sig_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
        loop = 0;
}

int starting(struct s_vhosts *vhos)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addr_list;

    int error = getaddrinfo(vhos->ip, vhos->port, &hints, &addr_list);
    if (error == -1)
        return -1;

    struct addrinfo *tmp;
    int sock;

    for (tmp = addr_list; tmp != NULL; tmp = tmp->ai_next)
    {
        sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
        if (sock == -1)
            continue;

        if (bind(sock, tmp->ai_addr, tmp->ai_addrlen) != -1)
            break;

        close(sock);
    }

    freeaddrinfo(addr_list);
    if (!tmp)
        return -1;

    return sock;
}

int serv_start(struct s_serv *g_serv, bool daemon)
{
    struct s_vhosts *vhos = g_serv->vhosts;

    printf("Preparing server on %s:%s ...\n\n", vhos->ip, vhos->port);

    FILE *f;

    if ((f = fopen(g_serv->global->pid_file, "r")))
    {
        char *str = malloc(6);
        fgets(str, 6, f);

        if (kill(atoi(str), 0) == 0)
        {
            free(str);
            return 1;
        }

        free(str);
        fclose(f);
    }

    int socket = starting(g_serv->vhosts);

    if (socket == -1)
    {
        printf("Failed to launch server\n");
        return 2;
    }

    if (listen(socket, 50) == -1)
    {
        printf("Too much people waiting. Please try later\n");
        return 2;
    }

    if (daemon)
    {
        int frk = fork();

        if (frk == -1)
        {
            printf("Fork failed\n");
            return 2;
        }

        if (frk != 0)
            return 0;
    }

    f = fopen(g_serv->global->pid_file, "w");
    if (!f)
    {
        close(socket);
        return 2;
    }

    int pid = getpid();
    fprintf(f, "%d", pid);

    fclose(f);

    printf("[SERVER LAUNCHED SUCCESSFULLY]\n---- Waiting for clients ----\n");

    if (daemon)
    {
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);
    }

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = &sig_handler;

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    char buff[4096] = { 0 };
    char buff2[4096] = { 0 };

    while (loop)
    {
        int client = accept(socket, NULL, NULL);
        if (client == -1)
            continue;

        for (size_t i = 0; i < 4096; i++)
            buff[i] = '\0';

        int r = recv(client, buff, 512, MSG_NOSIGNAL);

        if (r == -1)
        {
            close(client);
            continue;
        }

        for (size_t j = 0; j < 4096; j++)
            buff2[j] = '\0';

        request(buff2, buff, g_serv);
        send(client, buff2, strlen(buff2), MSG_NOSIGNAL);
        close(client);
    }

    close(socket);
    printf("Server has been stopped correctly\n");
    fflush(0);
    return 0;
}

int serv_restart(void);
int serv_reload(void);