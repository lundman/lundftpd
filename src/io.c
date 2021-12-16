// Increase the size of the normal fd_set

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif

#define FD_SETSIZE 4096

#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif

#define FD_SETSIZE 4096

#include "lion.h"

#include "global.h"
#include "socket.h"
#include "login.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "io.h"
#include "log.h"
#include "groups.h"
#include "ident.h"
#include "global.h"

int io_exit = 0;

#if defined (SUNOS) || defined (IRIX)
extern int errno;
#endif



time_t io_server_uptime = 0;

extern struct login_node *login_remove_list;
void main_rehash(void);








void loop()
{
  int nsel;
  static time_t lastcheck = 0;
  static int errors = 0;


  time(&io_server_uptime);

  if (!lastcheck) time(&lastcheck);


  while(1) {


	  // If we need to rehash, we do so now.
	  if (server_rehash) {

		  server_rehash = 0;

		  main_rehash();

	  }



    time(&global_time_now);

    if ((global_time_now - lastcheck) >= 60) {
      lastcheck = global_time_now;

      quota_timecheck();
      login_countstats();
      quota_sync();
      //group_weekcheck();

      ident_timeout();

      global_update_minute();

    }


	if (io_exit) return;



	nsel = lion_poll( 0, PERIOD); //PERIOD + 100 );


    switch(nsel) {
    case -1:
      /* Error */

		if (errno == EINTR)   /* If SUNOS? See #ifdef at top of file */
			continue;

#ifdef WIN32
		// We get this error sometimes under cygwin - seems to work if we ignore it.
		printf("Win32 error %d\n", errno);
		if (errno == ECHILD)
			continue;
#endif


		perror("select");
		consolef("Quitting, because select returned error: %d\n", errno);
		exit(-1);

		return;
		break;

	case 0:
		/* Timeout */
		/* Check for time-out logins */
		ident_timeout();
		// check_input(&rset, &wset);
		break;

	default:
		/* Input */

		ident_timeout();

		break;
	}

	errors = 0;


    // Cyclic checks on login/data nodes. In particular, data_connected();
    login_loop();

	// Find any nodes to be deleted.
	  if (login_remove_list) {

		  //consolef("[io] actually releasing %p\n", login_remove_list);
		  memset(login_remove_list, -1, sizeof(struct login_node));
		  free(login_remove_list);
		  login_remove_list = NULL;

	  }




  }

}
