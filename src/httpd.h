#ifndef HTTPD_H
#define HTTPD_H
#define GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

struct s_serv *g_serv;

struct s_serv
{
    struct s_global *global;
    struct s_vhosts *vhosts;
};

struct s_global
{
    char *pid_file;
    char *log_file;
    bool log;
};

struct s_vhosts
{
    char *server_name;
    char *port;
    char *ip;
    char *root_dir;
    char *default_file;
    struct s_vhosts *next;
};

#include "actions.h"
#include "dry.h"
#include "parser.h"
#include "request.h"
#include "serv.h"

#endif // ! HTTPD_H