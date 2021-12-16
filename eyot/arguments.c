#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __FreeBSD__
#include <getopt.h>
#endif

#include "misc.h"


int     arguments_verbose        = 0;
char   *arguments_configfile     = NULL;





static void options(char *prog)
{

	printf("\n");
	printf("%s - eyot irc bot\n", prog);
	printf("%s [-hv] [-f file] [...]\n\n", prog);
	printf("  options:\n");

	printf("  -h          : display usage help (this output)\n");
	printf("  -v          : verbose irc server output\n");
	printf("  -f file     : specify config file to read (default: eyot.conf)\n");

	printf("\n\n(c) Jorgen Lundman <lundman@lundman.net>\n\n");

	exit(0);

}


void arguments(int argc, char **argv)
{
	int opt;

	while ((opt=getopt(argc, argv,
					   "hvf:")) != -1) {

		switch(opt) {

		case 'h':
			options(argv[0]);
			break;

		case 'v':
			arguments_verbose += 1;
			break;

		case 'f':
			SAFE_COPY(arguments_configfile, optarg);
			break;

		default:
			printf("Unknown option.\n");
			break;
		}
	}

	argc -= optind;
	argv += optind;

	// argc and argv adjusted here.

}





