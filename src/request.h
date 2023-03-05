#ifndef REQUEST_H
#define REQUEST_H

#include "httpd.h"

int request(char *reponse, char *buff, struct s_serv *g_serv);

#endif // ! REQUEST_H