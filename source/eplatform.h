#ifndef EVENT_PLATFORM_H
#define EVENT_PLATFORM_H

#include "configi.h"

#ifdef SNF_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>   // for inet_pton,inet_ntop
#include <windows.h>
#include <process.h>    // for getpid,exec
#include <direct.h>     // for mkdir,rmdir,chdir,getcwd
#include <io.h>         // for open,close,read,write,lseek,tell

#define sleep_e(s)     Sleep((s) * 1000)
#define msleep_e(ms)   Sleep(ms)
#define usleep_e(us)   Sleep((us) / 1000)
#define delay_e(ms)    msleep_e(ms)
#define mkdir_e(dir)   mkdir(dir)

// access
#ifndef F_OK
#define F_OK            0       /* test for existence of file */
#endif
#ifndef X_OK
#define X_OK            (1<<0)  /* test for execute or search permission */
#endif
#ifndef W_OK
#define W_OK            (1<<1)  /* test for write permission */
#endif
#ifndef R_OK
#define R_OK            (1<<2)  /* test for read permission */
#endif

// stat
#ifndef S_ISREG
#define S_ISREG(st_mode) (((st_mode) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(st_mode) (((st_mode) & S_IFMT) == S_IFDIR)
#endif
#else

#include <unistd.h>
#include <dirent.h>     // for mkdir,rmdir,chdir,getcwd

// socket
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netdb.h>  // for gethostbyname

#define sleep_e(s)     sleep(s)
#define msleep_e(ms)   usleep((ms) * 1000)
#define usleep_e(us)   usleep(us)
#define delay_e(ms)    msleep_e(ms)
#define mkdir_e(dir)   mkdir(dir, 0777)
#endif

#ifdef _MSC_VER
typedef int pid_t;
typedef int gid_t;
typedef int uid_t;
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#else
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef void *HANDLE;

#include <strings.h>

#define stricmp     strcasecmp
#define strnicmp    strncasecmp
#endif

// ANSI C
#include <assert.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#endif //EVENT_PLATFORM_H