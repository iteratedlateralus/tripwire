#ifndef __TRIPWIRE_SMTP_HEADER__
#define __TRIPWIRE_SMTP_HEADER__
#include <libetpan/libetpan.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef WIN32
#   include "win_etpan.h"
#ifdef _MSC_VER
#   include "../src/bsd/getopt.h"
#   define STDIN_FILENO _fileno(stdin)
#else
#   include <getopt.h>
#endif
#else
#   include <netdb.h>
#   include <netinet/in.h>
#   include <sys/socket.h>
#   include <sys/mman.h>
#   include <unistd.h>
#   include <sys/ioctl.h>
#   include <pwd.h>

#   define _GNU_SOURCE
#   include <getopt.h>
#endif

int smtp_send_message(
    char* smtp_server,
    uint16_t smtp_port,
    char* smtp_user,
    char* smtp_password,
    char* smtp_from,
    char *data, size_t len, char**rcpts);
int build_smtp_message(char*,char*,int);
#endif
