#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#else
#define HAVE_STDARG_H 1
#endif

#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#if HAVE_STDARG_H
#  include <stdarg.h>
#  define VA_START(a, f)        va_start(a, f)
#else
#  if HAVE_VARARGS_H
#    include <varargs.h>
#    define VA_START(a, f)      va_start(a)
#  endif
#endif
#ifndef VA_START
#  warning "no variadic api"
#endif

#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif


/* socket_number is now in local.c !! */


#include "global.h"
#include "misc.h"
#include "login.h"
#include "socket.h"
#include "log.h"
#include "colour.h"
#include "data.h"
#include "file.h"


unsigned long localhost = 0;
int socket_accept_fd = -1;
char localhost_a[80];


#if 0
char *ul2a(unsigned long q)
{
  static char host[64];

  sprintf(host, "%u.%u.%u.%u",
	  ((unsigned char *)&q)[0], /* Assume network order */
	  ((unsigned char *)&q)[1],
	  ((unsigned char *)&q)[2],
	  ((unsigned char *)&q)[3]);

  return host;
}
#endif



void socket_load()
{

	struct hostent *h = NULL;

	// LISTEN ON SPECIFIC INTERFACE
	if (server_bindif) {

		strncpy(localhost_a, server_bindif, strlen(server_bindif));

		if ( (localhost = inet_addr(server_bindif)) == INADDR_NONE) {

			if (!localhost && !(h = gethostbyname(server_bindif))) {
				consolef("Can't determine bindif '%s' hostname: %s\n",
						server_bindif, strerror(errno));
				exit(1);
			}

			bcopy(h->h_addr, &localhost, sizeof(localhost));

		}

	}


	if (!localhost) {

		if ((gethostname(localhost_a, 80))
			|| !(h = gethostbyname(localhost_a)))
		{
			consolef("Can't determine hostname: %s\n",
						strerror(errno));
			exit(1);
		}

		bcopy(h->h_addr, &localhost, sizeof(localhost));

	}

	localhost = ntohl(localhost);

	consolef("[socket] localhost is %s\n", lion_ntoa(localhost));

}





#if HAVE_STDARG_H
void socket_print(struct login_node *t, char const *fmt, ...)
#else
void socket_print(t, fmt, va_alist)
     struct login_node *t;
     char const *fmt;
     va_dcl
#endif
{
  va_list ap;
  // Use to be BUFSZ, but we have it dynamic now...
  //static char msg[DATA_BUFSZ];
  static char msg[DATA_BUFSZ_STATIC];

  if (!t->handle) return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);


  lion_output(t->handle, msg, strlen(msg));

}




