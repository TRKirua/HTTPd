#ifndef SERV_H
#define SERV_H

#include "httpd.h"

struct s_serv *serv_init(struct s_global *glob, struct s_vhosts *vhos);
int verify_and_complete(void);
int command_launch(char *action, char *file, bool daemon);

#endif // ! SERV_H
