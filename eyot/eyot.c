#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#include "lion.h"

#include "eyot.h"
#include "irc.h"
#include "udp.h"
#include "conf.h"
#include "arguments.h"


__RCSID("$eyot: eyot.c,v 1.28 2003/04/23 08:32:03 lundman Exp $");

#define EYOT_VERSION  "0.3"

static int master_switch = 0;







void exit_interrupt(void)
{

	master_switch = 1;

}








int lion_userinput( connection_t *handle,
					void *user_data, int status, int size, char *line)
{

	printf("[eyot] userinput called %p/%p %d:%d - ignored.\n",
		   handle, user_data, status, size);

	return 0;

}




int main(int argc, char **argv)
{

	signal(SIGINT, exit_interrupt);
#ifndef WIN32
	signal(SIGHUP, exit_interrupt);
#endif
	signal(SIGPIPE, SIG_IGN);

	arguments(argc, argv);


	conf_init(arguments_configfile ? arguments_configfile : CONF_DEFAULT);


	printf("-++------ EYoT v%s ------++-\n", EYOT_VERSION);


	printf("Initialising Network...\n");

	lion_init();
	lion_compress_level( 0 );

	printf("Network Initialised.\n");


	while( !master_switch ) {

		lion_poll(0, 5);     // This blocks. (by choice, FYI).

		irc_poll();

	}

	//	irc_free();

	udp_free();

	lion_free();

	printf("Network Released.\n");

	printf("Done\n");

	return 0; // Avoid warning

}


