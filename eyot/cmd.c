#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include "lion.h"
#include "misc.h"

#include "cmd.h"
#include "irc.h"
#include "udp.h"
#include "parser.h"
#include "announce.h"
#include "arguments.h"





// Default function is somewhat special case. Items will generally
// always be one of:
//   1 - keys[0] contains the entire un-parsed string
//   2 - values[0] contain the receiptient username, keys[1] the string. ????
void cmd_default(char **keys, char **values,
				  int items,void *optarg)
{

	char *string, *ircserv, *chan;
	udp_t *ftp = (udp_t *)optarg;
	irc_server_t *is;
	irc_channel_t *ic;
	int i;


	if (!ftp) return;


	if (arguments_verbose)
		printf("[cmd] default\n");


	// Did we receive the IRCSERV= CHAN= keypairs? If so we only speak there.
	ircserv = parser_findkey(keys, values, items, "IRCSERV");
	chan    = parser_findkey(keys, values, items, "CHAN");

	// If given, we find ircserver, and channel/user and msg only there.
	// Otherwise, it is broadcast.
	if (ircserv && chan) {

		if ((is = irc_find_by_name(ircserv))) {

			ic = ircchan_find_by_name(is, chan);

			if (arguments_verbose)
				printf("[cmd] sending input to %s:'%s' ic '%s'\n",
					   is->name, chan,
					   ic ? ic->name : "(null)");

			string = an_format(keys, values, items,
							   ic ? ic : is->channels);

			is->reply = chan;

			if (string) {
				if (ic)
					irc_say_channel(ic, string);
				else
					irc_reply(is, string);

			} else {

				if (arguments_verbose)
					printf("[cmd] WARNING - no matching event for %s:%s\n",
						   ftp->name,
						   ic ? ic->name : is->channels);
			}

			return;
		}
	}



	// Basically, for all "channels" this server talks to, call the formating
	// function.

	for (i = 0;
		 (i < ftp->num_channels) && (ic = ftp->channels[ i ]);
		 i++) {


		if (arguments_verbose)
			printf("[cmd] sending input to channel '%s'\n", ic->name);

		string = an_format(keys, values, items, ic);

		if (string) {

			irc_say_channel(ic, string);

		} else {

			if (arguments_verbose)
				printf("[cmd] WARNING - no matching event for %s:%s\n",
					   ftp->name,
					   ic->name);

		}

	}

}






void cmd_cps(char **keys, char **values,
			 int items,void *optarg)
{
	udp_t *reply = (udp_t *) optarg;
	char *up, *down;
	float fup, fdown;

	if (!reply) return;


	// These are the _obligatory_ fields required.
	//   user - user causing action
	//   name - text of what type refers to.
	up   = parser_findkey(keys, values, items, "UP");
	down = parser_findkey(keys, values, items, "DOWN");

	// Missing one of these is a failure
	if (!up || !down) return;

	fup   = atof(up);
	fdown = atof(down);


	reply->cps_up   = fup;
	reply->cps_down = fdown;

	if (arguments_verbose)
		printf("[cmd] cps: %7.2f %7.2f %7.2f\n", fup, fdown, fup+fdown);

	// We don't really want to print it.
#if 0
	irc_say("CPS Heartbeat:  UP: %7.2f   Down: %7.2f   Total: %7.2f",
			fup, fdown, fup + fdown);
#endif
}

