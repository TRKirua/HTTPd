#ifndef ACTIONS_H
#define ACTIONS_H

#include "httpd.h"

int serv_restart(void);
int serv_reload(void);

int starting(struct s_vhosts *vhos);
int serv_start(struct s_serv *g_serv, bool daemon);

#endif // ! ACTIONS_H