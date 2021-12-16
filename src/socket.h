#include "local.h"  /* include local site designs */

/* #defines */

//#define DATA_BUFSZ     1024 /* Any > 1024 breaks on BSDI */

#define DATA_BUFSZ     server_data_buffer_size /* Any > 1024 breaks on BSDI */
#define DATA_BUFSZ_STATIC     8192 /* Any > 1024 breaks on BSDI */

/* variables */
extern int socket_accept_fd;
extern unsigned long localhost;
extern char localhost_a[80];

/* functions */
char *ul2a(unsigned long);
int socket_make(int *, int dolisten);
void socket_load(void);
int socket_accept(int, unsigned long *, int *);
int socket_connect(unsigned long, unsigned int);

#ifndef login_node
struct login_node;
#endif
void socket_print(struct login_node *, const char *, ...);
void socket_number(struct login_node *, int, char *);
/* socket_number is now in local.c !! */

