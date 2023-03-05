#ifndef PARSER_H
#define PARSER_H

#include "httpd.h"

void ParseFileGlobal(char *line, struct s_global *s_global);
void ParseFileVhosts(char *line, struct s_vhosts *s_vhosts);
int create_server(char *conf);

#endif // ! PARSER_H