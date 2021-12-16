#ifndef IDENT_H_INCLUDED
#define IDENT_H_INCLUDED


#include "lion.h"

/* Defines */


#define IDENT_REPLY 256

#define FROM_UNKNOWN "NONE"
#define RFC931_PORT     113             /* Semi-well-known port */

#define IDENT_TIMEOUT 10

#define IDENT_PENDING 1
#define IDENT_ACTIVE 2


/* Variables */

struct ident_node {

	lion_t *handle;

	char *reply; /*where to put reply,set to 0 before request, filled in l8r */

	unsigned long ident;
	int ident_port;
	int local_port, remote_port;

	time_t time; /* for timeout */

	struct ident_node *next;
};


/* Functions */

void ident_init(void);
void ident_request(struct login_node *, char *);
void ident_timeout(void);
void ident_remove(char *);
void ident_close(struct ident_node *);






#endif
