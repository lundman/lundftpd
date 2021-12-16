#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lion.h"
#include "misc.h"

#include "irc.h"
#include "irc_state.h"
#include "udp.h"
#include "arguments.h"









#if 1


//
// The main handler for the irc connections.
//
int irc_state_handler( connection_t *handle,
					   void *user_data, int status, int size, char *line)
{
	irc_server_t *irc = (irc_server_t *) user_data;

	if (!irc) {
		// Its set to NULL when irc_free() has been called.
		return 0;
	}



	if (irc->handle)
		irc->idle = lion_global_time;



	switch(status) {


	case LION_CONNECTION_LOST:
		if (arguments_verbose)
			printf("[irc] handler: irc connection '%s' %s:%d lost; %d:%s\n",
				   irc->name,
				   irc->host,
				   irc->port,
				   size, line);
		irc->state = IRC_NONE;
		irc->handle = NULL;
		break;


	case LION_CONNECTION_CLOSED:
		if (arguments_verbose)
			printf("[irc] handler: irc connection '%s' %s:%d closed;\n",
				   irc->name,
				   irc->host, irc->port);
		irc->state = IRC_NONE;
		irc->handle = NULL;
		break;


	case LION_CONNECTION_CONNECTED:
		if (arguments_verbose)
			printf("[irc] handler: irc connection '%s' %s:%d connected.\n",
				   irc->name,
				   irc->host, irc->port);

		// Turn on SSL if required for this server:
		if (irc->flags & IRC_FLAG_TLS)
			lion_ssl_set(irc->handle, LION_SSL_CLIENT);
		else
			irc->state = IRC_INITIALISE;

		break;

#ifdef WITH_SSL
	case LION_CONNECTION_SECURE_ENABLED:
		irc->state = IRC_INITIALISE;
		if (arguments_verbose)
			printf("[irc] handler: SSL enabled.\n");
		break;

	case LION_CONNECTION_SECURE_FAILED:
		if (arguments_verbose)
			printf("[irc] handler: SSL failed.\n");
		break;
#endif

	case LION_BUFFER_USED:
	case LION_BUFFER_EMPTY:
		if (arguments_verbose)
			printf("[irc] handler: buffering events ignored.\n");
		break;


	case LION_INPUT:

		if (arguments_verbose)
			printf("[%s:%d] %s\n", irc->host, irc->port, line);

		irc_state(irc, line);

		break;
	}

	return 0;

}






int decrypt_string(char *key, char *str, char *dest, int len);


int irc_state_parse(irc_server_t *irc, char *line)
{
	char *ar, *token, *from, *r, *addressee;
	int cmd;

	if (!line) return 0;

	// If it starts with ":" try to parse out the status code
	from = NULL;
	addressee = NULL;

	ar = line;

	// :lundman.interq.or.jp 433 * lundman :Nickname is already in use.
	if (*line == ':') {

		// Fetch irc server name
		from = misc_digtoken(&ar, ": ");
		if (!from) return 0;

		// Fetch protocol number
		token = misc_digtoken(&ar, ": ");
		if (!token) return 0;

#ifdef DEBUG_VERBOSE
		printf("[irc_state] reply %d\n", atoi(token));
#endif

		// If it's "0" here, try to parse against other commands too...
		if (atoi(token)) return atoi(token);


		// If it has a "!" truncate there.
		if ((r = strchr(from, '!')))
			*r = 0;


	} else {

		// Doesn't start with ":" try to parse commands...

		// Fetch command
		token = misc_digtoken(&ar, ": ");
		if (!token) return 0;

	}


	if (arguments_verbose)
		printf("[irc_handler] parse: '%s' (from = %s)\n", token,
			   from ? from : "(null)");


	// Handle commands.
	if (!mystrccmp("PING", token)) {
		lion_printf(irc->handle, "PONG %s\r\n", ar);
		if (arguments_verbose)
			printf("PONG %s\n", ar);
	}

#if 1
	if (from && !mystrccmp("PRIVMSG", token)) {

		//:lundman!~lundman@lundman.interq.or.jp PRIVMSG #bots :hi
		//:lundman!~lundman@lundman.interq.or.jp PRIVMSG po :hi

		// To whom was it addressed?
		addressee = misc_digtoken(&ar, ": ");
		if (!addressee) return 0;

		// Set up things so that we know how to reply, this will be
		//
		irc->reply = from;

		// Clear active channel.
		irc_setactive(irc, NULL);

		// If it was said on a channel, also set the active channel

		// If it was to us directly, use "from" as is.
		// if it was to the channel, use "channel" to reply.
		if (addressee[0] == '#') {

			// Find channel and assign it.
			irc_setactive(irc, addressee);

			irc->reply = addressee;
		}
		// irc->reply has been set to what we should reply to.

		// And what was said?
		token = misc_digtoken(&ar, ": ");
		if (!token) return 0;

		// Encrypted?
		if (irc->active_channel && irc->active_channel->fish &&
			!strcmp("+OK", token)) {

			if (!decrypt_string(irc->active_channel->fish,
								ar, token, strlen(ar)))
				return 0;

			if (arguments_verbose)
				printf("[irc_handler] decrypted '%s'\n", token);

			ar = token;

			token = misc_digtoken(&ar, " \r\n");
			if (!token) return 0;

		}


		//
		// Check which IRC command
		//
		// Use universal "!" char to indicate command, or,
		// if the first word is our name.
		cmd = 0;

		if (arguments_verbose)
			printf("[irc_state] to us? '%s':'%s'\n",
				   token,irc->nick);

		if (!strncasecmp(token, irc->nick, strlen(irc->nick))) {

			cmd = 1;
			// First word was our name, so go get the actual command.
			token = misc_digtoken(&ar, " \r\n");
			if (!token) return 0;

		}

		// Either way, command may start with "!"
		if (token[0] == '!') {
			token++;
			cmd = 1;
		}


		// Parse the command
		if (cmd) {

			if (!mystrccmp("bw", token)) {
				irc_bw(irc, ar);
			} else if (!mystrccmp("speed", token)) {
				irc_speed(irc, ar);
			} else if (!mystrccmp("uploaders", token)) {
				irc_uploaders(irc, ar);
			} else if (!mystrccmp("downloaders", token)) {
				irc_downloaders(irc, ar);
			}
		}
		//
		// End Commands
		//



	} // PRIVMSG

	irc->reply = NULL; // clear it, so we speak to the channel in future.

#endif

	return 0; // no state

}












//
// Main irc state engine. Connect, authenticate, join etc.
//
void irc_state( irc_server_t *irc, char *line )
{

	int state;

	if (!irc->handle) {

		if (irc->state != IRC_NONE) {
			irc->state = IRC_NONE;
			if (arguments_verbose)
				printf("[irc_state] irc->handle was NULL, but state not IRC_NONE\n");
		}
	}

	switch (irc->state) {


		// No state, if we have no handle set, and time
		// has passed sufficiently for a retry to connect, do so here.
	case IRC_NONE:
		// Find the current server
		// Connect to it
		if (arguments_verbose)
			printf("[irc_state] considering '%s' time %lu\n",
				   irc->name,
				   irc->idle + IRC_RECONNECT_DELAY - lion_global_time);

		if (!irc->handle &&
			(irc->idle + IRC_RECONNECT_DELAY) < lion_global_time) {

			// Connect us
			irc->idle = lion_global_time;

			sleep(1);
			if (arguments_verbose)
				printf("Connecting...\n");

			irc->handle = lion_connect(irc->host, irc->port, 0,
									   0, LION_FLAG_FULFILL, (void *) irc);

			lion_set_handler(irc->handle, irc_state_handler);


		}
		break;




		// Connection has JUST been established.
	case IRC_INITIALISE:
		irc->state = IRC_GREETING;

		lion_printf(irc->handle, "USER %s %s %s :%s\r\n",
					irc->nick, irc->nick, irc->nick,
					irc->name);

		if (irc->passwd)
			lion_printf(irc->handle, "NICK %s %s\r\n", irc->nick, irc->passwd);
		else
			lion_printf(irc->handle, "NICK %s\r\n", irc->nick);

		break;


		// Sent USER and NICK. Get error, or the usual login crap.
	case IRC_GREETING:

		state = irc_state_parse(irc, line);

		switch(state) {

		case 433:  // Nick name already in use
			printf("[irc_handler] nick name already in use\n");
			break;

		case 1: // I am guessing this means good?
			if (arguments_verbose)
				printf("[irc_handler] registered.\n");
			irc->state = IRC_JOIN;
			break;

			// state we don't care about...
		default:
			break;

		}


		break;





		// We have registered as a user, attempt to join
	case IRC_JOIN:
		// Set the usermode if there is one:
		if (irc->mode) {
			if (arguments_verbose)
				printf("[irc_state] setting usermode %s on %s:%d",
					   irc->mode, irc->host, irc->port);
			lion_printf(irc->handle, "MODE %s :%s\r\n", irc->nick,
						irc->mode);
		}

		{
			struct irc_channel *runner;

			for (runner = irc->channels; runner;
				 runner = runner->next) {

				lion_printf(irc->handle, "JOIN %s %s\r\n", runner->name,
					runner->key ? runner->key : "" );

			}

			// We should check if we could join... or keep trying..
			irc->state = IRC_ACTIVE;

		}

		break;




		// All is done. joined channel etc. Here, deal with users and commands
	case IRC_ACTIVE:

		state = irc_state_parse(irc, line);


		// Have we not heard anything for a really long time? reconnect then
		if ((irc->idle + IRC_RECONNECT_STALLED) < lion_global_time) {
			if (arguments_verbose)
				printf("[irc_state] stale, reconnecting...\n");
			lion_close(irc->handle);
		}

		break;





	default:
		printf("[irc_state] help, unknown state %d for %s:%d\n",
			   irc->state, (irc) ? irc->host : "NULL",
				(irc) ? irc->port : 0);
		break;
	}


}





















#endif
