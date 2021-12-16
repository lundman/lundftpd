#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef WITH_SSL
#include <openssl/blowfish.h>
#endif
#include <ctype.h>
#include <string.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#include "lion.h"
#include "misc.h"

#include "udp.h"
#include "parser.h"
#include "cmd.h"
#include "irc.h"

#include "conf.h"

#include "arguments.h"


static udp_t *udp_head = NULL;



//
// These are the command and functions to call. Note the last one will also
// catch any unknowns IFF the function is defined. If we don't want to deal
// we unknowns, just set it to NULL.
//
// To add new command, add line in here, add prototype in cmd.h, and the
// actual code in cmd.c.
parser_command_list_t udp_command_list[] = {
	//	{ COMMAND( "CPS"      )   , cmd_cps                },
	{ NULL,                  0, cmd_default            }
};






//
// The main handler for any incoming udp packets.
//
int udp_handler( connection_t *handle,
				 void *user_data, int status, int size, char *line)
{
	udp_t *node = (udp_t *) user_data;
	char ivec[8] = { 0,0,0,0,0,0,0,0 };
	int num = 0;

	switch(status) {


	case LION_CONNECTION_LOST:
		if (arguments_verbose)
			printf("[udp] connection failed: %d:%s\n", size, line);

		/* fall-through */
	case LION_CONNECTION_CLOSED:
		if (arguments_verbose)
			printf("[udp] connection closed.\n");

		if (node)
			node->handle = NULL;

		//exit_interrupt();

		break;

	case LION_CONNECTION_CONNECTED:
		if (arguments_verbose)
			printf("[udp] ready\n");
		break;

	case LION_BUFFER_USED:
	case LION_BUFFER_EMPTY:
		if (arguments_verbose)
			printf("[udp] ignoring buffering events\n");
		break;


	case LION_BINARY:

		if (arguments_verbose)
			printf("[udp] binary received %d (from %s)\n", size,
				   node && node->name ? node->name : "unknown");

		// Decrypt input data.
		if (!node) break;

		//void BF_cfb64_encrypt(const unsigned char *in, unsigned char *out,
		//      long length, BF_KEY *schedule, unsigned char *ivec, int *num,
		//      int enc);

		// Do we want to check size is sane here?
#ifdef WITH_SSL
		BF_cfb64_encrypt(line, line, size, &node->key, ivec, &num,
						 BF_DECRYPT);
#endif

		// Quick test, if the password is VERY wrong and it just decrypted
		// to garbage, discard this sooner.. Not really required since the
		// parsers wont match either...
		if (!isalnum(line[0]) || !isalnum(line[1])) {
			break;
		}

		// Let's cheat. We assume this is only ONE line sent, so just
		// null terminate it, and pass it to ourselves.
		line[size] = 0;
		udp_handler(handle, user_data, LION_INPUT, size, line);

		break;



	case LION_INPUT:

		if (!node) return 0;

		lion_getpeername(handle, &node->host, &node->port);

		if (arguments_verbose > 1)
			printf("[udp] from %s:%d -> %s\n",
				   lion_ntoa(node->host), node->port, line);

		// We don't have to pass udp_listen along, but if we ever want
		// to support multiple udp inputs, perhaps special ones from different
		// servers (with different passwords) we need to reply to the right
		// one, so let's make it simpler for us in future.
		parser_command(udp_command_list, line, (void *)node);

		break;

	default:
		break;
	}

	return 0;

}








udp_t *udp_init(char *name, unsigned long ip, int lport, char *pass,
				int rport)
{
	int port = lport;
	udp_t *newd;
	lion_t *tmp;


	newd = (udp_t *) malloc(sizeof(*newd));

	if (!newd) return NULL;

	memset(newd, 0, sizeof(*newd));



	// Ready up listening port. do we want multiple listening sockets?
	// there is no real reason...

	newd->name = strdup(name);

	newd->handle = lion_udp_new(&port, 0, LION_FLAG_FULFILL, newd);

	lion_set_handler( newd->handle, udp_handler);

	newd->remote_port = rport;
	newd->host = ip;

#if 0
	if (ip && rport) {

		// Bind specifically to this IP if specified.
		if (arguments_verbose)
			printf("binding %s:%d\n", lion_ntoa(ip), rport);
		tmp = lion_udp_bind(newd->handle, ip, rport, newd);
		lion_set_userdata(newd->handle, NULL);
		lion_close(newd->handle);
		newd->handle = tmp;
	}
#endif


	// Encrypted.
#ifdef WITH_SSL
	BF_set_key(&newd->key,
			   strlen(pass), pass);
#endif

	lion_setbinary(newd->handle);


	newd->next = udp_head;
	udp_head = newd;


	return newd;

}



void udp_free(void)
{
	udp_t *tmp;

	while(udp_head) {

		// Close the socket
		if (udp_head->handle) {
			// Detatch userdata, so we dont get the event
			lion_set_userdata(udp_head->handle, NULL);
			lion_disconnect(udp_head->handle);
		}

		// Free the node data.
		udp_head->handle = NULL;  // safety
		SAFE_FREE(udp_head->name);

		tmp = udp_head->next;

		SAFE_FREE(udp_head);

		udp_head = tmp;

	}


}




udp_t *udp_assign_channel(struct irc_channel *ic)
{
	udp_t *ftp;

	//printf("[udp] assigning channel '%s' to '%s'\n",
	//   ic->name,
	//   ic->ftpname);

	for (ftp = udp_head; ftp; ftp=ftp->next) {

		if (!strcasecmp(ic->ftpname, ftp->name)) {

			// Add another to the list.
			ftp->channels = realloc(ftp->channels,
									sizeof(ic) * (ftp->num_channels + 1));

			if (!ftp->channels) {
				perror("malloc: ");
				exit(1);
			}

			ftp->channels[ ftp->num_channels ] = ic;
			ftp->num_channels++;

			return ftp;

		}

	}

	return NULL;

}



udp_t *udp_find(char *name)
{
	udp_t *ftp;

	for (ftp = udp_head; ftp; ftp=ftp->next) {

		if (!strcasecmp(name, ftp->name)) {

			if (!ftp->handle) return NULL;

			return ftp;

		}

	}

	return NULL;
}


void udp_send_all(char *line)
{
	char encoded[1500];
	char ivec[8] = { 0,0,0,0,0,0,0,0 };
	int num = 0, len;
	udp_t *ftp;

	len = strlen(line);

	for (ftp = udp_head; ftp; ftp=ftp->next) {

		if (!ftp->handle) continue;

#ifdef WITH_SSL
		BF_cfb64_encrypt(line, encoded, len,
						 &ftp->key,
						 ivec, &num,
						 BF_ENCRYPT);
#endif

		lion_output(ftp->handle, encoded, len);

	}

}


#if __STDC__ || WIN32
int udp_sendf(udp_t *ftp, char const *fmt, ...)
#else
	 int udp_sendf(ftp, fmt, va_alist)
	 udp_t *ftp;
     char const *fmt;
     va_dcl
#endif
{
	va_list ap;
	static char msg[1500]; // Max UDP packet anyway
	static char encoded[1500];
	int result;
	char ivec[8] = { 0,0,0,0,0,0,0,0 };
	int num = 0, len;


#if __STDC__ || WIN32
	va_start(ap, fmt);
#else
	va_start(ap);
#endif

	// Buffer overflow! No vsnprintf on many systems :(
	len = vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	if (len <= 0) return len;
	if (!ftp->handle) return -1;

	if (arguments_verbose)
		printf("[udp] sending to '%s':'%s'\n", ftp->name, msg);

#ifdef WITH_SSL
	BF_cfb64_encrypt(msg, encoded, len,
					 &ftp->key,
					 ivec, &num,
					 BF_ENCRYPT);
#endif

	if (ftp->host && ftp->remote_port) {
			if (arguments_verbose)
				printf("[udp] Remote FTP bound: %s:%d\n",
                       lion_ntoa(ftp->host), ftp->remote_port);
			lion_udp_bind(ftp->handle, ftp->host, ftp->remote_port, NULL);
	}

	lion_output(ftp->handle, encoded, len);

	return len;
}
