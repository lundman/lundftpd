#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lion.h"
#include "../lion/src/misc.h"

#include "global.h"
#include "socket.h"
#include "login.h"
#include "log.h"
#include "section.h"
#include "misc.h"
#include "remote.h"


//#define DEBUG

static void remote_fail(struct login_node *node, remote_enum_t command, char *reason)
{
	remote_t *rem;
	
	if (!node) return;
	if (!node->remote_slave) return;
	if (!node->handle) return;       // if they have already logged out.

	rem = (remote_t *) node->remote_slave;
	
	switch(command) {
		
	case REMOTE_CMD_SIZE:
		lion_printf(node->handle, "500 SIZE failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_PASV:
		lion_printf(node->handle, "425 PASV failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_LIST:
		lion_printf(node->handle, "551 LIST failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_CWD:
		lion_printf(node->handle, "550 CWD failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_RETR:
		lion_printf(node->handle, "550 RETR failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_MDTM:
		lion_printf(node->handle, "550 MDTM failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_PRET: 
		lion_printf(node->handle, "500 PRET failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_STOR:
		lion_printf(node->handle, "550 STOR failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_APPE:
		lion_printf(node->handle, "550 APPE failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_REST:
		lion_printf(node->handle, "550 REST failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_TYPE:
		lion_printf(node->handle, "550 TYPE failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_PROT:
		lion_printf(node->handle, "550 PROT failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_NLST:
		lion_printf(node->handle, "550 NLST failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_MKD:
		lion_printf(node->handle, "550 MKD failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_DELE:
		lion_printf(node->handle, "550 DELE failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_RMD:
		lion_printf(node->handle, "550 RMD failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_RNFR:
		lion_printf(node->handle, "550 RNFR failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_RNTO:
		lion_printf(node->handle, "550 RNTO failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_PORT:
		lion_printf(node->handle, "500 PORT failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_CCSN:  
		// This is potentially incorrect. We don't know if
		// remote FTPd supports this, but a failure should be seen
		// by client and hopefully....
		lion_printf(node->handle, "500 CCSN failed '%s'\r\n", 
					reason ? reason : "");
		break;
	case REMOTE_CMD_SSCN:  
		// This is potentially incorrect. We don't know if
		// remote FTPd supports this, but a failure should be seen
		// by client and hopefully....
		lion_printf(node->handle, "500 SSCN failed '%s'\r\n", 
					reason ? reason : "");
		break;

	case REMOTE_CMD_NONE:
	default:
		consolef("[remote_relay_int] reached for %s with bad type %u\n",
				 node->user, rem->command);
		lion_printf(node->handle, "500 INTERNAL ERROR!\r\n");
	}

	if (rem) {
		SAFE_FREE(rem->args);
		rem->command = REMOTE_CMD_NONE;
	}

	// Disconnect then as well?
	lion_close(rem->handle);
	rem->handle = NULL;

}



static void remote_send(struct login_node *node)
{
	remote_t *rem;
	
	if (!node) return;
	if (!node->remote_slave) return;
	if (!node->handle) return;       // if they have already logged out.

	rem = (remote_t *) node->remote_slave;

	if (!rem) return;
	
	switch(rem->command) {
		
	case REMOTE_CMD_SIZE:
		lion_printf(rem->handle, "SIZE /%s\r\n", 
					hide_path(rem->args));
		break;

	case REMOTE_CMD_PASV:
		lion_printf(rem->handle, "PASV\r\n");
		break;

	case REMOTE_CMD_LIST:
		lion_printf(rem->handle, "LIST %s\r\n", 
					rem->args?rem->args:"");
		SAFE_FREE(rem->pret); // only valid for one turn
		break;

	case REMOTE_CMD_CWD:
		lion_printf(rem->handle, "CWD %s\r\n", 
					hide_path(rem->args));
		// Do NOT release rem->args below, but keep it until we know status.
		return;
		break;

	case REMOTE_CMD_RETR:
		// FIXME: We add "/" infront of like "remote/file.txt" but I guess
		// we could also skip past the ->cwd, if they didn't specify it
		// absolutely.
		lion_printf(rem->handle, "RETR /%s\r\n", 
					hide_path(rem->args));
		SAFE_FREE(rem->pret); // only valid for one turn
		break;

	case REMOTE_CMD_MDTM:
		lion_printf(rem->handle, "MDTM /%s\r\n", 
					hide_path(rem->args));
		break;

	case REMOTE_CMD_PRET:  // We don't send anything, but we need to say 200 ok
		rem->command = REMOTE_CMD_NONE;
		lion_printf(node->handle, "200 PRET Command OK.\r\n");
		break;

	case REMOTE_CMD_STOR:
		lion_printf(rem->handle, "STOR /%s\r\n", 
					hide_path(rem->args));
		SAFE_FREE(rem->pret); // only valid for one turn
		break;

	case REMOTE_CMD_APPE:
		lion_printf(rem->handle, "APPE /%s\r\n", 
					hide_path(rem->args));
		SAFE_FREE(rem->pret); // only valid for one turn
		break;

	case REMOTE_CMD_REST:
		lion_printf(rem->handle, "REST %s\r\n", rem->args);
		break;

	case REMOTE_CMD_TYPE:
		lion_printf(rem->handle, "TYPE %s\r\n", rem->args);
		break;
	case REMOTE_CMD_PROT:
		lion_printf(rem->handle, "PROT %s\r\n", rem->args);
		break;
	case REMOTE_CMD_NLST:
		lion_printf(rem->handle, "NLST %s\r\n", 
					rem->args ? rem->args : "");
		SAFE_FREE(rem->pret); // only valid for one turn
		break;
	case REMOTE_CMD_MKD:
		lion_printf(rem->handle, "MKD /%s\r\n", 
					hide_path(rem->args));
		break;
	case REMOTE_CMD_DELE:
		lion_printf(rem->handle, "DELE /%s\r\n", 
					hide_path(rem->args));
		break;
	case REMOTE_CMD_RMD:
		lion_printf(rem->handle, "RMD /%s\r\n", 
					hide_path(rem->args));
		break;
	case REMOTE_CMD_RNFR:
		lion_printf(rem->handle, "RNFR /%s\r\n", 
					hide_path(rem->args));
		break;
	case REMOTE_CMD_RNTO:
		lion_printf(rem->handle, "RNTO /%s\r\n", 
					hide_path(rem->args));
		break;
	case REMOTE_CMD_PORT:
		lion_printf(rem->handle, "PORT %s\r\n", 
					rem->args?rem->args:"");
		break;
	case REMOTE_CMD_CCSN:
		lion_printf(rem->handle, "CCSN %s\r\n", 
					rem->args?rem->args:"");
		break;
	case REMOTE_CMD_SSCN:
		lion_printf(rem->handle, "SSCN %s\r\n", 
					rem->args?rem->args:"");
		break;

	case REMOTE_CMD_NONE:
	default:
		consolef("[remote_send] reached for %s with bad type %u\n",
				 node->user, rem->command);
		lion_printf(node->handle, "500 INTERNAL ERROR!\r\n");
	}


	// Command sent, we can release the args.
	SAFE_FREE(rem->args);

}


//
// Check if we had a prior PRET command, and if so return true.
// Clears out the PRET command so it is only good for one test.
//
int remote_ispret(struct login_node *node)
{
	remote_t *rem;

	if (!node || !node->remote_slave) return 0;

	rem = (remote_t *) node->remote_slave;

	if (!rem->pret) return 0;

	switch (rem->command) {
	case REMOTE_CMD_LIST:
	case REMOTE_CMD_NLST:
		return 2;
	default:
		return 1;
	}

	return 1;
}


void remote_clearpret(struct login_node *node)
{
	remote_t *rem;

	if (!node || !node->remote_slave) return;

	rem = (remote_t *) node->remote_slave;

	SAFE_FREE(rem->pret); // only valid for one turn
}



//
// WARNING: This function ASSUMES that "args" has already been through the
// file path checking functions (file_pathcheck*)!! Since we call 
// section_remote test to determine if we are in a remote section, and that
// function requires a reduced path before called.
//
void remote_relay(struct login_node *node, remote_enum_t command, char *args)
{
	remote_t *rem;
	section_t *section = NULL;
	
	if (!node) return;
	
	rem = (remote_t *) node->remote_slave;

	//#ifdef DEBUG
	consolef("[remote] relay %s cmd %u with args '%s'\n",
			 node->user, command, args);
	//#endif


	// If we don't have a node allocated, do so now.
	if (!rem) {

		rem = (remote_t *) malloc(sizeof(*rem));
		if (!rem) {
			remote_fail(node, command, "Out of memory");
			return;
		}

		memset(rem, 0, sizeof(*rem));

		// Assign us over!
		node->remote_slave = (void *) rem;

	}

	// Find the desired section we want.
	// If a PRET command was given, we use that first, and foremost.
	// Then we check if they passed us an argument, then use that.
	// Finally if all those failed, we will attempt using cwd as a last measure
	if (rem->pret) 
		section = section_find(rem->pret);
	
	if (!section && args) 
		section = section_find(args);
	
	if (!section || !section->remote_user) 
		section = section_find(node->cwd);


	// Fail then.
	if (!section || !section->remote_user) {
		remote_fail(node, command, "Could not find section - or it is not a remote section");
		return;
	}



	// Now we need to check if we are connected, but its to the wrong
	// section/remote host, we need to disconnect first.
	// This code assumes remote_* variables are not NULL, but how
	// can they be NULL if it is remote. Only remote_user being NULL
	// would make it core, but section_remote() should not return true then.
	if (rem->handle &&               // we have a connected handle.
		(rem->section != section) && // the sections don't match
		((rem->section->remote_host != section->remote_host) ||
		 (rem->section->remote_port != section->remote_port) ||
		 strcmp(rem->section->remote_user, section->remote_user))) {
		// and the sites defined are different!

		consolef("[remote_relay] different remote detected, re-connecting...\n");

		rem->section = NULL;
		lion_set_userdata(rem->handle, NULL); 
		lion_close(rem->handle);
		rem->handle = NULL;

	}

	// Assign over the section we are supposed to be, or still are.
	rem->section = section;

	// We should always have replied to a command by the time we are here.
	if (rem->command != REMOTE_CMD_NONE) {
		consolef("[remote] warning %s remote command %u is not null!\n",
				 node->user, rem->command);
	}


	rem->command = command;

	// If we were pret, save command for later.
	if (command == REMOTE_CMD_PRET) {

		SAFE_COPY(rem->pret, args);

	} else {

		SAFE_COPY(rem->args, args);

	}


	// If we are not connected, we need to trigger that right now
	// All replying to original client is cared for delayed now.
	if (!rem->handle) {

		consolef("[remote_relay] Connecting to remote FTPD...\n");

		rem->handle = lion_connect(
								   lion_ntoa(section->remote_host),
								   section->remote_port,
								   0,
								   0,
								   LION_FLAG_FULFILL,
								   (void *)node);

		lion_set_handler(rem->handle, remote_handler);

		return;
		
	}


	// we just need to relay the command now.
	remote_send(node);
	
}






void remote_release(struct login_node *node)
{
	remote_t *rem;

	if (!node) return;

	rem = (remote_t *) node->remote_slave;

	if (rem) {

		// Remove our node, since closing the handle will generate an
		// event.
		node->remote_slave = NULL;

		if (rem->handle) {

			// We can't have remote handle's userdata refer to the login node
			// at all from now on.
			lion_set_userdata(rem->handle, NULL); 

			// close whatever connection is up.
			lion_close(rem->handle);
			rem->handle = NULL; // not needed, NULLed above.

		}

		SAFE_FREE(rem->args);

		rem->section = NULL;

		SAFE_FREE(rem);

	}

}






//
// remote handler
//
// user_data is the login_node pointer. If it is NULL the user has already
// logged out, and we've issued lion_close() so it should be safe to ignore
// all events.
//
int remote_handler( lion_t *handle,
					void *user_data, int status, int size, char *line)
{
	struct login_node *node = (struct login_node *) user_data;
	remote_t *rem;

#ifdef DEBUG
	consolef("[remote] %p/%p handler event %d: '%s'\n", handle, user_data, status,
			 line ? line : "(null)");
#endif

	// No login node, ignore events.
	if (!node) return 0;

	rem = (remote_t *) node->remote_slave;

	// If rem is NULL, we have disassociated ourselves with login node already
	// and are closing down the connection.
	
	switch( status ) {

	case LION_CONNECTION_LOST:
		consolef("[remote] %p/%p connection failed %d:%s\n", 
				 handle, user_data, size, line);

		/* Fall-through */

	case LION_CONNECTION_CLOSED:
		//consolef("[ident] %p/%p connection closed\n", handle, user_data);

		// Are we in the middle of processing a command? If so
		// we need to report this problem

		if (rem) {

			// clear handle, it is now invalid.
			rem->handle = NULL;

			if (rem->command != REMOTE_CMD_NONE) 
				remote_fail(node, rem->command, 
							line ? line : "connection closed");
			
			// Clear up everything for next connection.
			rem->command = REMOTE_CMD_NONE;
			SAFE_FREE(rem->args);
			rem->section = NULL;
		}

		break;


	case LION_CONNECTION_CONNECTED:
		// start login procedure
		if (rem) {
			rem->state = REMOTE_STATE_CONNECTED;
			while(remote_state(node, NULL));
		}
		break;

#ifdef WITH_SSL
	case LION_CONNECTION_SECURE_ENABLED: // SSL is gooood
		while(remote_state(node, "997 SSL was goood"));
		break;

	case LION_CONNECTION_SECURE_FAILED:
		if (rem) {
			rem->state = REMOTE_STATE_SENT_AUTH_SSL;
			while(remote_state(node, "500 AUTH SSL failed to negotiate."));
		}
		break;
#endif

	case LION_INPUT:
		if (rem) {
			while(remote_state(node, line)) line = NULL;
		}

	}

	return 0;
}


//
// Deal with the state. While we return TRUE, we keep calling this function.
// 
int remote_state(struct login_node *node, char *line)
{
	remote_t *rem;
	int code = -1;

	if (!node) return 0; // shouldn't happen

	rem = (remote_t *) node->remote_slave;

	if (!rem) return 0;  // also shouldn't happen

	if (!rem->section) return 0; // ditto
	

	if (line && line[0] && line[1] && line[2] && (line[3] == ' ') &&
		isdigit(line[0]) && isdigit(line[1]) && isdigit(line[2])) 
		code = atoi(line);

	if ((code <= 0) ||
		(code > 1000))
		code = -1;


#ifdef DEBUG
	consolef("[remote_state] %d line '%s'\n",
			 code, line ? line : "(null)");
#endif


	switch(rem->state) {

	case REMOTE_STATE_CONNECTED:
		// Just go into waiting for 220.
		rem->state = REMOTE_STATE_AWAIT220;
		break;


	case REMOTE_STATE_AWAIT220: 
		if (code == 220) {

			rem->state = REMOTE_STATE_SENT_AUTH_TLS;
			lion_printf(rem->handle, "AUTH TLS\r\n");
			break;
		}
		break;

	case REMOTE_STATE_SENT_AUTH_TLS:
#ifdef WITH_SSL
		if (code == 234) {

			rem->state = REMOTE_STATE_AWAIT_SSL;
			lion_ssl_set(rem->handle, LION_SSL_CLIENT);
			break;
		}
#endif

		if (code >= 500) {
			rem->state = REMOTE_STATE_SENT_AUTH_SSL;
			lion_printf(rem->handle, "AUTH SSL\r\n");
			break;
		}
		break;

	case REMOTE_STATE_SENT_AUTH_SSL:
		if (code == 234) {

			rem->state = REMOTE_STATE_AWAIT_SSL;
#ifdef WITH_SSL
			lion_ssl_set(rem->handle, LION_SSL_CLIENT);
#endif
			break;
		}

		if (code >= 500) {

			if (rem->section->remote_ssl) {  // SSL enforced, and we just failed.
				remote_fail(node, rem->command, line);
				break;
			}

			// Failing TLS/SSL is ok, carry on.
			rem->state = REMOTE_STATE_SENT_USER;
			// Send user.
			lion_printf(rem->handle, "USER %s\r\n", 
						rem->section->remote_user);
			break;
		}


	case REMOTE_STATE_AWAIT_SSL:
		if (code == 997) { // internal secret code, success

			// Do we want to send PBSZ, or PROT here?
			// RFC say PROT C is the default though.
			rem->state = REMOTE_STATE_SENT_USER;
			// Send user.
			lion_printf(rem->handle, "USER %s\r\n", 
						rem->section->remote_user);
			break;
		}
		break;

	case REMOTE_STATE_SENT_USER: 
		if (code == 331) {

			rem->state = REMOTE_STATE_SENT_PASS;

			lion_printf(rem->handle, "PASS %s\r\n", 
						rem->section->remote_pass);
			break;
		}

		if (code > 0) {
			remote_fail(node, rem->command, line);
		}

		break;

	case REMOTE_STATE_SENT_PASS: 
		if (code == 230) {
			
			rem->state = REMOTE_STATE_CHECK_PROT;
			return 1; // Call us again
		}
		
		if (code > 0) {
			remote_fail(node, rem->command, line);
		}

		break;

	case REMOTE_STATE_CHECK_PROT:
		if (node->status & ST_DATA_SECURE) {
			rem->state = REMOTE_STATE_SENT_PROT;
			lion_printf(rem->handle, "PROT P\r\n");
			consolef("[remote] sending PROT\n");
			break;
		} 
		
		rem->state = REMOTE_STATE_CHECK_CCSN;
		return 1;


	case REMOTE_STATE_SENT_PROT:
		if (code >= 300) {
			remote_fail(node, rem->command, line);
			break;
		}

		// Ok we set PROT level.
		if (code > 0) {
			rem->state = REMOTE_STATE_CHECK_CCSN;
			return 1;
		}
		break;

	case REMOTE_STATE_CHECK_CCSN:
		if (node->mode & MODE_TYPE_CCSN) {
			rem->state = REMOTE_STATE_SENT_CCSN;
			lion_printf(rem->handle, "CCSN ON\r\n");
			consolef("[remote] sending CCSN\n");
			break;
		}

		rem->state = REMOTE_STATE_CHECK_SSCN;
		return 1;

	case REMOTE_STATE_SENT_CCSN:
		if (code >= 300) {
			remote_fail(node, rem->command, line);
			break;
		}

		// Ok we set CCSN level.
		if (code > 0) {
			rem->state = REMOTE_STATE_CHECK_SSCN;
			return 1;
		}
		break;


	case REMOTE_STATE_CHECK_SSCN:
		if (node->mode & MODE_TYPE_SSCN) {
			rem->state = REMOTE_STATE_SENT_SSCN;
			lion_printf(rem->handle, "SSCN ON\r\n");
			consolef("[remote] sending SSCN\n");
			break;
		}

		rem->state = REMOTE_STATE_CHECK_TYPE;
		return 1;

	case REMOTE_STATE_SENT_SSCN:
		if (code >= 300) {
			remote_fail(node, rem->command, line);
			break;
		}

		// Ok we set SSCN level.
		if (code > 0) {
			rem->state = REMOTE_STATE_CHECK_TYPE;
			return 1;
		}
		break;


	case REMOTE_STATE_CHECK_TYPE:
		if (node->mode & MODE_TYPE_BINARY) {
			rem->state = REMOTE_STATE_SENT_TYPE;
			lion_printf(rem->handle, "TYPE I\r\n");
			consolef("[remote] sending TYPE\n");
			break;
		}

		rem->state = REMOTE_STATE_CHECK_DONE;
		return 1;

	case REMOTE_STATE_SENT_TYPE:
		if (code >= 300) {
			remote_fail(node, rem->command, line);
			break;
		}

		// Ok we set TYPE level.
		if (code > 0) {
			rem->state = REMOTE_STATE_CHECK_DONE;
			return 1;
		}
		break;

	case REMOTE_STATE_CHECK_DONE:
		// Send command
		rem->state = REMOTE_STATE_RELAY;
		consolef("[remote] actually sending the original command to remote\n");
		remote_send(node);
		break;
		

	case REMOTE_STATE_RELAY:
		lion_printf(node->handle, "%s\r\n", line);

		switch(rem->command) {

		case REMOTE_CMD_CWD:
			if (code == 250) {  // cwd was successful, update cwd
				SAFE_FREE(node->cwd);
				node->cwd = rem->args;
				rem->args = NULL;
			}
			break;
		default:
			;
		}


		// Command probably finished?
		if (code > 0) {
			rem->command = REMOTE_CMD_NONE;
		}
		break;

	case REMOTE_STATE_NONE:
	default:
		;
	}

	return 0;

}
