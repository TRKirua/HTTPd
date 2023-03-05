CC= gcc
CFLAGS= -std=c99 -Werror -Wall -Wextra -Wvla

SRC= src/httpd.c src/serv.c src/parser.c src/actions.c src/dry.c src/request.c

all: clean httpd

httpd:
	$(CC) $(CFLAGS) -o httpd $(SRC)

clean:
	$(RM) httpd *.o *.a *.swp server.log
