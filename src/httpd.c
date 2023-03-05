#include "httpd.h"

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        bool daemon = 0;
        char *action = NULL;
        char *file = NULL;

        if (!strcmp(argv[1], "--dry-run"))
        {
            if (argc == 2)
                errx(2, "\"--dry-run\" with not enough parameter");

            if (argc >= 6)
                errx(2, "\"--dry-run\" with too much parameters");

            char *arg = argv[2];

            if (argc != 3)
            {
                if (argc == 5)
                {
                    if (strcmp(argv[2], "-a")
                        || (strcmp(argv[3], "start") && strcmp(argv[3], "stop")
                            && strcmp(argv[3], "reload")
                            && strcmp(argv[3], "restart")))
                        errx(2, "Format is not valid");

                    arg = argv[4];
                }

                else
                {
                    if (strcmp(argv[2], "start") && strcmp(argv[2], "stop")
                        && strcmp(argv[2], "reload")
                        && strcmp(argv[2], "restart"))
                        errx(2, "Format is not valid");

                    arg = argv[3];
                }
            }

            if (test_dryrun_command(arg) == 2)
                errx(2, "Config file is not valid");
            printf("Config file is valid\n");
            return 0;
        }

        else if (!strcmp(argv[1], "-a"))
        {
            daemon = 1;

            if (argc <= 3)
                errx(2, "\"-a\" with not enough parameter");
            if (argc >= 5)
                errx(2, "\"-a\" with too much parameters");

            action = argv[2];
            file = argv[3];
        }

        else if (argc == 2)
        {
            action = "start";
            file = argv[1];
        }

        else
        {
            action = argv[1];
            file = argv[2];
        }

        int com = command_launch(action, file, daemon);

        if (com == -1)
            errx(2, "Command \"%s\" don't exist", action);

        if (com == 1)
            return 1;

        if (com == 2)
            errx(2, "Command \"%s\" failed", action);
    }

    else
        errx(1,
             "Usage : ./httpd [--dry-run] [-a (start | stop | reload | "
             "restart)] server.conf");

    remove(g_serv->global->pid_file);
    return 0;
}