#ifndef DRY_H
#define DRY_H

#include "httpd.h"

int CheckFileGlobal(char *line, bool *has_pid_file);
int CheckFileVhosts(char *line, bool *mandatory);
int test_dryrun_command(char *conf);

#endif // ! DRY_H