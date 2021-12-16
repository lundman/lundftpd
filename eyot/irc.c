#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <string.h>

#include "lion.h"
#include "misc.h"

#include "irc.h"
#include "irc_state.h"

#include "udp.h"
#include "conf.h"
#include "announce.h"
#include "arguments.h"


static irc_server_t *irc_server_head = NULL;









int irc_init(irc_server_t *servers)
{
	irc_server_t *is;
	irc_channel_t *ic;

	irc_server_head = servers;

	// Ok, loop through all irc servers, and all channels
	// make sure that the "announce" exists, and that
	// ftpserver name exists too.
	for (is = irc_server_head; is; is=is->next) {

		//printf("\n\n**********\nIRC: server '%s'\n", is->name);

		for (ic = is->channels; ic; ic=ic->next) {

			//printf("    channel '%s'\n", ic->name);

			// Make sure ftpserver exists
			ic->ftpserver = udp_assign_channel(ic);

			if (!ic->ftpserver) {
				fprintf(stderr, "irc: channel's ftpserver name '%s' does not exist.\n",
						ic->ftpname);

			} // if null


			// Also find all announce sections with the name wanted and
			// assign to the channel.
			an_assigntochannel(ic);

			if (ic->ircserver != is)
				printf("[irc] warning, server mismatch\n");


		}

	}



	return 0;

}



void irc_free(void)
{


}



//
// This function just makes sure that we call the irc_state function
// for all irc connections periodically. Like reconnecting etc.
//
void irc_poll(void)
{
	irc_server_t *is;

	for (is = irc_server_head; is; is=is->next) {
		irc_state( is, NULL);
	}


}


#if 0
//
// This function sends a string to ALL channels defined.
//
#if __STDC__ || WIN32
int irc_say_all(char const *fmt, ...)
#else
	 int irc_say_all(fmt, va_alist)
     char const *fmt;
     va_dcl
#endif
{
	va_list ap;
	static char msg[8192];
	static char send[8192];
	int result;
	irc_server_t *runner;
	struct irc_channel *chan;



#if __STDC__ || WIN32
	va_start(ap, fmt);
#else
	va_start(ap);
#endif


	// Buffer overflow! No vsnprintf on many systems :(
	result = vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	if (result <= 0) return result;

	if (runner->handle) {

		for (chan = irc->channels; chan; chan = chan->next) {

			result = snprintf(send, sizeof(send),
						  "PRIVMSG %s :%s\r\n",
						  runner->reply ? runner->reply : chan->name,
						  msg);

			lion_output(runner->handle, send, result);

			if (arguments_verbose)
				printf("[irc] say: %s", send);

		}
	}

	return result;

}
#endif




int encrypt_string(char *key, char *str, char *dest, int len);



//
// This function sends a string to a SPECIFIC channel.
//
#if __STDC__ || WIN32
int irc_say_channel(irc_channel_t *ic, char const *fmt, ...)
#else
	 int irc_say_channel(ic, fmt, va_alist)
	 irc_channel_t *ic;
     char const *fmt;
     va_dcl
#endif
{
	va_list ap;
	static char msg[8192];
	static char send[8192];
	char *in, *out;
	int result;
	irc_server_t *is;

	if (!ic) return -1;

#if __STDC__ || WIN32
	va_start(ap, fmt);
#else
	va_start(ap);
#endif

	// Assign our irc server
	is = (irc_server_t *)ic->ircserver;

	// Buffer overflow! No vsnprintf on many systems :(
	result = vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	if (result <= 0) return result;

	if (is->handle) {

		in = msg;
		out = send;

		// If fish is on, we need to hash it now.
		if (ic->fish) {
			strcpy(out, "+OK ");
			encrypt_string(ic->fish, in, &out[4], result);
			in = send;
			out = msg;
		}




		result = snprintf(out, sizeof(send),
						  "PRIVMSG %s :%s\r\n",
						  ic->name,
						  in);

		lion_output(is->handle, out, result);

		if (arguments_verbose)
			printf("[irc] %s say: %s (%s)\n", ic->name, out, in);

		// If we send something we arent considered idle
		is->idle = lion_global_time;


		return result;

	}

	return 0;

}



//
// This function replies to whatever "reply" is.
//
#if __STDC__ || WIN32
int irc_reply(irc_server_t *is, char const *fmt, ...)
#else
	 int irc_reply(is, fmt, va_alist)
	 irc_server_t *is;
     char const *fmt;
     va_dcl
#endif
{
	va_list ap;
	static char msg[8192];
	static char send[8192];
	int result;

#if __STDC__ || WIN32
	va_start(ap, fmt);
#else
	va_start(ap);
#endif

	// Buffer overflow! No vsnprintf on many systems :(
	result = vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	if (result <= 0) return result;

	if (is->handle) {

		result = snprintf(send, sizeof(send),
						  "PRIVMSG %s :%s\r\n",
						  is->reply,
						  msg);

		lion_output(is->handle, send, result);

		if (arguments_verbose)
			printf("[irc] %s reply: %s\n", is->reply, send);

		return result;

	}

	return 0;

}




void irc_setactive(irc_server_t *irc, char *name)
{
	irc_channel_t *chan;

	if (!irc) return;

	if (!name) {
		irc->active_channel = NULL;
		return;
	}

	for (chan = irc->channels; chan; chan = chan->next) {

		if (!strcasecmp(name, chan->name)) {

			irc->active_channel = chan;
			return;

		}

	}

	irc->active_channel = NULL;

}








irc_server_t *irc_find_by_name(char *name)
{
	irc_server_t *is;

	for (is = irc_server_head; is; is=is->next) {
		if (!mystrccmp(is->name, name))
			return is;
	}

	return NULL;
}




irc_channel_t *ircchan_find_by_name(irc_server_t *irc, char *name)
{
	irc_channel_t *chan;

	for (chan = irc->channels; chan; chan = chan->next) {

		if (!mystrccmp(name, chan->name))
			return chan;
	}

	return NULL;
}















void irc_bw(irc_server_t *irc, char *line)
{
	udp_t *ftp;

	if (arguments_verbose)
		printf("[irc] cps\n");

	if (!irc || !irc->channels || !irc->channels->ftpserver) return;

	// Find the ftp server we are attached to
	ftp = irc->channels->ftpserver;

	if (!ftp) return;

	udp_sendf(ftp, "IRC_BW %s %s",
			  irc->name,
			  irc->reply ? irc->reply : "");
			//				  line && *line ? line : "");
}


void irc_speed(irc_server_t *irc, char *line)
{
	udp_t *ftp;

	if (arguments_verbose)
		printf("[irc] speed\n");

	if (!irc || !irc->channels || !irc->channels->ftpserver) return;

	// Find the ftp server we are attached to
	ftp = irc->channels->ftpserver;

	if (!ftp) return;

	udp_sendf(ftp, "IRC_SPEED %s %s %s",
			  irc->name,
			  irc->reply ? irc->reply : "",
			  line && *line ? line : "");
}


void irc_uploaders(irc_server_t *irc, char *line)
{
	udp_t *ftp;

	if (arguments_verbose)
		printf("[irc] uploaders\n");

	if (!irc || !irc->channels || !irc->channels->ftpserver) return;

	// Find the ftp server we are attached to
	ftp = irc->channels->ftpserver;

	if (!ftp) return;

	udp_sendf(ftp, "IRC_UPLOADERS %s %s %s",
			  irc->name,
			  irc->reply ? irc->reply : "",
			  line && *line ? line : "");
}


void irc_downloaders(irc_server_t *irc, char *line)
{
	udp_t *ftp;

	if (arguments_verbose)
		printf("[irc] downloaders\n");

	if (!irc || !irc->channels || !irc->channels->ftpserver) return;

	// Find the ftp server we are attached to
	ftp = irc->channels->ftpserver;

	if (!ftp) return;

	udp_sendf(ftp, "IRC_DOWNLOADERS %s %s %s",
			  irc->name,
			  irc->reply ? irc->reply : "",
			  line && *line ? line : "");
}



