#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <time.h>
#include <sys/stat.h>
#ifdef FREEBSD
#include <libutil.h>
#endif



#include "global.h"
#include "socket.h"
#include "misc.h"
#include "login.h" /* Last */
#include "user.h"
#include "data.h"
#include "file.h"
#include "check.h"
#include "quota.h"
#include "exec.h"
#include "help.h"
#include "log.h"
#include "userflag.h"
#include "groups.h"
#include "ident.h"
#include "site.h"
#include "fnmatch.h"
#include "section.h"
#include "io.h"
#include "irc.h"
#include "sitecust.h"

#include "remote.h"

#include "lion.h"
#include "dirlist.h"
#include "../lion/src/misc.h"
//#include "lion/misc.h"

#include "version.h"


#ifdef WIN32
#include "win32.h"
#endif


__RCSID("$LundFTPD: login.c,v 1.28 2000/00/00 08:32:03 lundman Exp $");






/*
 * Commands yet to implement
 * SITE umask, SITE chmod
 * MDTM:
 */

int server_num_logins = 0;
int server_num_data = 0;


static int login_abort_loop = 0;

extern unsigned long main_passiveIP;



/* Used to be "static" but I need it in quota.c and didn't feel like doing a
   wrapper */
// no longer used
//struct login_node *login_node_head = NULL;

struct login_node *login_remove_list = NULL;







struct command_s commands[] = { /* Before ST_LOGIN */
	{ "HELP", login_help },
	{ "USER", login_user },
	{ "PASS", login_pass },
	{ "SYST", login_syst },
	{ "QUIT", login_quit },
	{ "NOOP", login_noop },
	{ "IDEN", login_iden },    /* EXTENDED COMMAND! Warning! Renamed from AUTH*/
	/* crypt commands, moved from crypt stuff */
	{ "AUTH", login_auth },
	{ "PBSZ", login_pbsz },
	{ "PROT", login_prot },
	{ (char *)0, (void *)0 }
};

struct command_s commands2[] = { /* After ST_LOGIN */
	{ "PORT", login_port },
	{ "SITE", login_site },  /* SITE commands are now in site.c!! */
	{ "LIST", login_list },
	{ "TYPE", login_type },
	{ "RETR", login_retr },
	{ "STOR", login_stor },
	{ "CWD",  login_cwd  },
	{ "PWD",  login_pwd  },
	{ "CDUP", login_cdup },
	{ "APPE", login_appe },
	{ "STAT", login_stat },
	{ "ABOR", login_abor },
	{ "\xf2STAT", login_stat }, /* Two warnings */
	{ "\xf2\x41\x42OR", login_abor }, /* -14, f2, 242 \762 */
	{ "\xff\xf4\xff\xf2\x41\x42OR", login_abor }, /* lftp ssl "OOB" */
	{ "NLST", login_nlst },
	{ "MKD" , login_mkd },
	{ "DELE", login_dele },
	{ "RMD",  login_rmd },
	{ "PASV", login_pasv },
	{ "P@SV", login_pasv },
	{ "EPSV", login_pasv },
	{ "RNFR", login_rnfr },
	{ "RNTO", login_rnto },
	{ "REST", login_rest },
	{ "SIZE", login_size },
	{ "MDTM", login_mdtm },
	{ "XPWD", login_pwd  },
	{ "CLNT", login_clnt },
	{ "FEAT", login_feat },
	{ "PRET", login_pret },
#ifdef WITH_SSL
	{ "CCSN", login_ccsn },
	{ "SSCN", login_sscn },
#endif
	{ (char *)0, (void *)0 }
};







//
// This is the lion handler for all login nodes.
//
int login_handler( lion_t *handle,
				   void *user_data, int status, int size, char *line)
{
	struct login_node *t = (struct login_node *) user_data;

	// Here, user_data can be NULL for CONNECTED, but otherwise it should
	// be our login node.
	switch( status ) {

	case LION_CONNECTION_LOST:
		consolef("[login] connection lost: %s [%p] (%d:%s)\n",
				 t ? t->user : "(null)",
				 handle, size, line);
		if (t) {
            if (handle) lion_set_userdata(handle, NULL);
			t->handle = NULL; // it's closed, don't call close below.
			login_exit( t );
		}
		break;

	case LION_CONNECTION_CLOSED:
		consolef("[login] connection closed: %s [%p]\n",
				 t ? t->user : "(null)",
				 handle);
		if (t) {
            if (handle) lion_set_userdata(handle, NULL);
			t->handle = NULL; // it's closed, don't call close below.
			login_exit( t );
		}
		break;

	case LION_CONNECTION_CONNECTED:
		consolef("[login] connected: %p\n", handle);


		if (server_forceglobalonly) {
			unsigned long rhost;
			int rport;

			// Where are they from?
			lion_getpeername(handle, &rhost, &rport);

			snprintf(genbuf, sizeof(genbuf),
					 "lbnc@%s", lion_ntoa(rhost));

			if (!misc_patt_match_gbl(PATT_GLOBAL, genbuf)) {
				consolef("[login] connection NOT in global and forceglobalonly is on\n");
				lion_disconnect(handle);
				return 0;
			}
		}



		// Create new login node, which has relay, and set relay to have login
		t = login_new( handle );    // it would be bad to fail.
		if (!t) {
			lion_disconnect( handle );
			return 0;
		}

		lion_set_userdata(handle, t);
		lion_getpeername(handle, &t->host, (int *) &t->port);
		consolef("[login] connection from %s:%d\n",
				 lion_ntoa(t->host), t->port);
		t->idle = lion_global_time;
		break;

#ifdef WITH_SSL
	case LION_CONNECTION_SECURE_ENABLED:
		consolef("[ssl] %p/%p (%s) successful.\n",
				 handle,
				 t,
				 t ? t->user : "(null)");
		//t->status |= ST_TLS;
		//t->status |= ST_DATA_SECURE;
		break;

	case LION_CONNECTION_SECURE_FAILED:
		consolef("[ssl] %p/%p (%s) failed.\n",
				 handle,
				 t,
				 t ? t->user : "(null)");
		// check if it's SSL only
		break;
#endif

		// If we are sending loads of data from another lion handle, for
		// example sitecust commands, we need to pause the reader here.
	case LION_BUFFER_USED:
		if (t && t->sitecust) {
			sitecust_disable_read( t->sitecust );
		}
		//consolef("[login] pause %p\n", t->althandle);
		break;

	case LION_BUFFER_EMPTY:
		if (t && t->sitecust) {
			sitecust_enable_read( t->sitecust );
		}
		//consolef("[login] resum %p\n", t->althandle);
		break;



	case LION_INPUT:
		if (t) {

			// Security stuff. If a string is > 256 chars, lets just
			// truncate it, clearly they are up to no good.
			if (size >= INPUT_MAXLINE)
				line[INPUT_MAXLINE] = 0;

			login_command(t, line);

		}
		break;

	} // switch

	return 0;
}



int login_statlist_handler( lion_t *handle,
				   void *user_data, int status, int size, char *line)
{
	struct login_node *t = (struct login_node *) user_data;

	// Here, user_data can be NULL for CONNECTED, but otherwise it should
	// be our login node.
    consolef("[login] statlist: received event %d\n", status);

	switch( status ) {

	case LION_INPUT:
		if (t && line) {
            consolef("[login] statlist: %p : %s\n",
                     t, line);

            if (*line == ':') {
                // reply from dirlist

                // Dirlist starting, skip line
                if (line[1] == '0') break;

                // Dirlist finished
                if (!strcmp(line, ":END"))
                    consolef("[login] statlist finished, resuming normal ops...\n");
                else
                    consolef("[login] dirlist said no\n");

                lion_printf(t->handle, "213 End of status\r\n");

                if (t->handle) {
                    lion_enable_read(t->handle);
                    lion_set_handler(t->handle, login_handler);
                    return 0;
                }

            } // is colon

            lion_printf(t->handle, " %s\r\n", line);
        } // t or line
        break;

	case LION_CONNECTION_LOST:
	case LION_CONNECTION_CLOSED:
		consolef("[data] %p/%p Got close/lost even while dirlisting!\n",
				 handle, user_data);

		// We remember that this handle failed.
		dirlist_cancel(handle);

        /* FALL-THROUGH */
    default:
        // Pass event to original handler
        if (t)
            return login_handler( handle,
                                  user_data, status, size, line);
        break;

    }

    return 0;
}










void login_command(struct login_node *t, char *line)
{
	char *command;
	int i;

	t->idle = lion_global_time;

	if (!(command = digtoken((char **)&line, " "))) {
		socket_number(t, 500, NULL);
		return;
	}

	for(i = 0; commands[i].name; i++)
		if(!mystrccmp( commands[i].name, command)) {
			t->last_command = i; /* set last command */
			commands[i].function(t, line);
			return;
		}

	for(i = 0; commands2[i].name; i++)
		if(!mystrccmp( commands2[i].name, command)) {
			if (t->status & ST_LOGIN) {
				t->last_command = i+1024; /* set last command */
				commands2[i].function(t, line);
			}
			else
				socket_number(t, 5301, NULL);
			return;
		}

	socket_number(t, 500, command);

	consolef("Command(%p) :%s:%s\n", t->handle, command,
			 (line&&*line?(char *)line:"N/A"));

}





struct login_node *login_new(lion_t *handle)
{
	struct login_node *nue;
	int i;

	/*
	 * Create a node for this user connection
	 */

	nue = (struct login_node *) malloc(sizeof(struct login_node));


	if (nue) memset(nue, 0, sizeof(struct login_node));

	// We store passwords temporarily in buffer, so we need one.
	nue->buffer = (unsigned char *) malloc(8192);


	if (!nue || !nue->buffer) {
		return NULL;
	}


	nue->status = ST_NONE;
	//nue->socket = -1;  // now cap_group
	nue->handle = handle;
	nue->sitecust = NULL;
	nue->inbuffer = 0;
	nue->outbuffer = 0;
	nue->level = UF_NONE;
	nue->options = 0;

	nue->cap_group = 0;
	nue->port_host = 0;
	nue->port_port = 0;
	nue->waiting_for = 0;
	nue->data_restart = 0;
	nue->user[0] = 0;
	nue->pass[0] = 0;
	nue->data_active = 0;
	nue->data = 0;
	nue->mode = MODE_TYPE_ASCII; /* ASCII */
	nue->offset = 0;
	nue->last_command = 0;
	nue->current_group = NULL;
	nue->quota = NULL;
	nue->auth = NULL;
	nue->cwd_devid = 0;
	nue->bytes_up_login = 0;
	nue->files_up_login = 0;
	nue->seconds_up_login = 0;
	nue->bytes_down_login = 0;
	nue->files_down_login = 0;
	nue->seconds_down_login = 0;

	nue->num_logins = 0;
	nue->num_downloads = 0;
	nue->num_uploads = 0;

	nue->remote_slave = NULL;

	/*
	  nue->daily = 0;
	  nue->bytes_up = nue->bytes_down = 0;
	  nue->ratio = 0;
	*/
	nue->cwd = mystrcpy("/");

	for (i=0; i<IPMATCHMAX; i++)
		bzero(nue->matchip[i], IPMATCHLEN);

	/* Send greeting */
	socket_number(nue, 220, NULL);

	/* Fire away a identd lookup */
	ident_request(nue, nue->ident);

	login_countstats();

	/* OK, it's in the whooga */
	return nue;
}



















void login_exit(struct login_node *t)
{

	/* BUG - quota_new called before ST_LOGIN set
	   if (t->status & ST_LOGIN) {
	*/

	consolef("login_exit() ...\n");



	/* Log it out if they were logged in */
	if (t->status & ST_LOGIN) {

		if (server_quotafile) {

			/* Here, close all data sessions */
			while(t->data) {
				((struct data_node *)t->data)->status = 0; // remove 226, or we try to send it.
				data_close((struct data_node *)t->data);
			}

			/* Heh, close the data sessions first, then close quota!! */
			if (t->quota) {
				quota_exit(t);
				t->quota = NULL; /* Just to be on the safe side */
			}

			time(&t->logoff_time);

			user_update(t); /* Write us to disk */

			log_user("%s logged off %s", t->user, ctime(&t->logoff_time));

		}
	}

	if (t->status & ST_NEWLOGIN) {

		t->status &= ~ST_NEWLOGIN & ~ST_LOGIN;
		return;
	}

#if 1
	if (!t->ident[0]) { /* is a possible identd check active? */
		ident_remove(t->ident);
	}
#endif

	// Technically, we should release these things before we update to disk
	// since it is silly to write dummy pointers to disk.

	if (t->sitecust) {
		sitecust_exit( t->sitecust );
		t->sitecust = NULL;
	}

	// Release the remote node if it exists.
	remote_release(t);


	if (t->cwd) {
		free(t->cwd);
		t->cwd = NULL;
	}

	if (t->buffer) {
		free(t->buffer);
		t->buffer = NULL;
	}


	// if our lion handle is still set, we disconnect it, then close it
	// we will get a even in handler, but since userdata is NULL we dont
	// end up here again.
	if (t->handle) {
        lion_set_handler(t->handle, lion_userinput);
		lion_set_userdata(t->handle, NULL);
		lion_close(t->handle);
		t->handle = NULL;
	}

	consolef("login_exit(%p)\n", t->handle);


	// If we are in user capping, we also need to release the lion_group
	// if we are the last logged in. We should use quota->num_logins but we
	// don't. Dumb!

	if (server_capping &&
		(server_cap_user_download || server_cap_user_upload)) {

		if (!login_isloggedin_notus(t->user, t)) {

			// no other logins.
			lion_group_free(t->cap_group);
			consolef("[login] releasing lion_group %d\n", t->cap_group);

		}
	}

	// Also release our actual node. Is this dangerous here?
	// Yeah, cos if we are in a bunch of prints, and we get connection
	// closed/lost from one, then rest will keep trying to print. So we can
	// not free it here. Only tag it to be released.

	memset(t, 0, sizeof(*t));  // safety?
	t->offset = -1; // safety. If we try to write it, the update function
	// should freak.

	// FIXME! FIXME! This only remembers ONE node to release each loop.
	// true, that it just leaks memory, but we should fix this anyway.
	login_remove_list = t;



	login_abort_loop = 1;

}


/*
 *
 * USER COMMANDS FUNCTIONS
 *
 */


void login_help(struct login_node *t, char *args)
{
	int i, j;
	char buf[80];

	consolef("login_help(%p): %s\n", t->handle, args);

	/*
	  214-The following commands are recognized (* =>'s unimplemented).
	  USER    PORT    STOR    MSAM*   RNTO    NLST    MKD     CDUP
	  PASS    PASV    APPE    MRSQ*   ABOR    SITE    XMKD    XCUP
	  ACCT*   TYPE    MLFL*   MRCP*   DELE    SYST    RMD     STOU
	  SMNT*   STRU    MAIL*   ALLO    CWD     STAT    XRMD    SIZE
	  REIN*   MODE    MSND*   REST    XCWD    HELP    PWD     MDTM
	  QUIT    RETR    MSOM*   RNFR    LIST    NOOP    XPWD
	  214 Direct comments to ftp-bugs@tao.sans.vuw.ac.nz.
	*/


	if (*args) {
		/* Non-zero argument, pass to real help */
		help_commands(t, args);
		return;
	}

	if (t->options & UO_COLOUR)
		lion_printf(t->handle, MSG_214C_HEADER);
	else
		lion_printf(t->handle, MSG_214_HEADER);

	j = 0;
	*buf = (char) 0;

	for(i = 0; commands[i].name; i++) {
		switch(j) {
		case 0:
			sprintf(buf, "   %-8s", commands[i].name);
			j++;
			break;
		case 6:
			sprintf(&buf[3+(j<<3)], "%-8s", commands[i].name);
			lion_printf(t->handle, "%s\r\n", buf);
			j = 0;
			break;

		default:
			sprintf(&buf[3+(j<<3)], "%-8s", commands[i].name);
			j++;
		}
	}

	if (t->status & ST_LOGIN)
		for(i = 0; commands2[i].name; i++) {
			switch(j) {
			case 0:
				sprintf(buf, "   %-8s", commands2[i].name);
				j++;
				break;
			case 6:
				sprintf(&buf[3+(j<<3)], "%-8s", commands2[i].name);
				lion_printf(t->handle, "%s\r\n", buf);
				j = 0;
				break;

			default:
				sprintf(&buf[3+(j<<3)], "%-8s", commands2[i].name);
				j++;
			}
		}

	if (j)
		lion_printf(t->handle, "%s\r\n", buf);

	if (t->options & UO_COLOUR)
		lion_printf(t->handle, MSG_214C_TAIL);
	else
		lion_printf(t->handle, MSG_214_TAIL);

}

void login_user(struct login_node *t, char *args)
{
	consolef("login_user(%p - '%s'): %s\n", t->handle, t->user, args);


	if (t->status & ST_LOGIN) {

		consolef("USER hook\n");

		/* Already logged in, log them out first */
		t->status |= ST_NEWLOGIN;
		login_exit(t);

		t->user[0] = 0;
	}

	if (!args || !*args) {
		socket_number(t, 530, NULL);
		return;
	}


	// Check if they specified ! or %
	if (*args == '!') {

		t->status |= ST_DUPELOGIN;
		args++;

	} else if (*args == '%') {

		t->status |= ST_DUPEDOWN;
		args++;

	}

	strncpy(t->user, args, MAXUSERNAME);
	t->user[MAXUSERNAME] = 0;

	// Check if it is anonymous login

	if (!(mystrccmp(args, "ftp")) || !(mystrccmp(args, "anonymous"))) {
		if (!server_anonymous) {
			socket_number(t, 530, args);
			return;
		}
		strcpy(t->user, "ftp");
		socket_number(t, 331, NULL);

		// Disable autokick features for anonymous

		t->status &= ~(ST_DUPELOGIN|ST_DUPEDOWN);

		return;
	}

	socket_number(t, 331, args);

}



int login_ident_reply_sub(lion_t *handle, void *arg1, void *arg2)
{
	struct login_node *t;

	// Not a login node
	if (lion_get_handler(handle) != login_handler) return 1;

	// No login node
	if (!(t = (struct login_node *)lion_get_userdata(handle))) return 1;

	if (t->waiting_for != IDENT_REPLY) return 1;

	if (t->ident == (char *) arg1) {

		consolef("Resuming login.. \n");

		login_pass(t, (char *)t->buffer);

		t->waiting_for = 0;

		return 0; // stop
	}

	return 1;

}



void login_ident_reply(char *reply)
{
	/* find the logged in node, who's ident field == reply above.
	 * check if it's pause, if so, unpause it and call _pass */
    consolef("[login] ident reply\n");
	lion_find(login_ident_reply_sub, (void *)reply, NULL);

}







int login_count_logins_sub(lion_t *node, void *arg1, void *arg2)
{
	int *totals = (int *)arg2;
	char *user = (char *)arg1;
	struct login_node *t;

	if (!totals || !user) return 0;

	if (lion_get_handler(node) != login_handler)
		return 1;

	t = (struct login_node *) lion_get_userdata(node);
	if (!t) return 1;

	totals[0]++;

	if ((t->user != user) && !strcmp(t->user, user))
		totals[1]++;

	return 1;

}







int login_dupe_kicksub(lion_t *handle, void *arg1, void *arg2)
{
	struct login_node *user = (struct login_node *) arg1;
	struct login_node *lookup;

	if (!user) return 0;

	if (lion_get_handler(handle) != login_handler)
		return 1;

	lookup = (struct login_node *) lion_get_userdata(handle);

	if (!lookup) return 1;

	if ((lookup != user) && !strcmp(lookup->user, user->user)) {

		if ((user->status & ST_DUPEDOWN)) {

			if (!user->data) return 1;

			if (!(((struct data_node *)user->data)->type & DATA_RETR))
				return 1;

		}

		log_user("%s auto kick %s\n", user->user,
				 user->status & ST_DUPELOGIN ?
				 "login" : "download");

		login_abort_loop = 1;

		socket_print( lookup, "221 Auto-kicked by %c%s login\r\n",
					  user->status & ST_DUPELOGIN ? '!' : '%',
					  lookup->user);

		login_exit( lookup ); /* Log them out */

	}

	return 1;
}








void login_pass(struct login_node *t, char *args)
{
	struct login_node *tmp;
	struct quota_node *quota;
	int num = 0, total_logins = 0;
	int ints[2] = {0, 0};  // total-, and user- num logins.

	consolef("login_pass(%p): %s\n", t->handle, args);

	if (!t->user[0]  || (t->status & ST_LOGIN)) {
		/* Haven't issued USER command first */

		socket_number(t, 503, NULL);
		return;
	}


	/* Check if we have ident replies */
	if (server_useident && !t->ident[0]) {

		consolef("Waiting for ident reply\n");
		t->waiting_for = IDENT_REPLY;

		/* temporarily save the args passed to us! */
		// t->buffer allocated as 8192, max string input 256.
		strcpy((char *)t->buffer, args);

		return;
	}

	// We allow password-less logins for anonymous only
	if (!strcmp(t->user, "ftp")) {
		if (!args || !*args)
			args = "<blankpass>";
	}

	if (!args || !*args || !user_pass(t, args)) { /* Right passwd? */

		consolef("FAILED LOGIN on %s from %s:%d ident %s\n",
				 t->user, lion_ntoa(t->host), t->port, t->ident);

#if 0
		log_user("FAILED LOGIN on %s from %s:%d ident %s\n",
				 t->user, ul2a(t->host), t->port, t->ident);
#endif

		socket_number(t, 530, NULL);
		t->status &= ~ST_LOGIN; /* Just incase */
		t->user[0] = 0;
		return;
	}


	// Check if this is secure, or localhost
#ifdef WITH_SSL
	if (server_allow_only_secure) {

		if (!lion_ssl_enabled(t->handle) &&
			( t->host != 0x7f000001)   ) {

			consolef("FAILED LOGIN on %s from %s:%d ident %s - not secure login\n",
					 t->user, lion_ntoa(t->host), t->port, t->ident);

#if 1
			log_user("FAILED LOGIN on %s from %s:%d ident %s - not secure login\n",
					 t->user, lion_ntoa(t->host), t->port, t->ident);
#endif

#if 1
			lion_printf(t->handle, "530 Only SECURE/ENCRYPTED logins permitted\r\n");
			t->status &= ~ST_LOGIN; /* Just incase */
			t->user[0] = 0;
			return;
#endif
		}
	}
#endif


	/* user_pass() fills in details if passwd is correct */
	/* NOTE: user_pass sends the first 230- line */

	time(&t->time); /* Fill in login time, used in user_pass() */

	/* Check if user is allowed to login more than once */

	/* No values set, set some. */
#if 0
	if (!t->num_logins)    t->num_logins = server_loginlimit;
	if (!t->num_uploads)   t->num_uploads = server_uploadlimit;
	if (!t->num_downloads) t->num_downloads = server_downloadlimit;
#endif



#ifdef ACC

	if (!t->num_download > 1) {

		t->num_logins = server_loginlimit;
		t->num_uploads = server_uploadlimit;
		t->num_downloads = server_downloadlimit;

	}

#endif

	//	consolef("Num l%d u%d d%d\n", t->num_logins, t->num_uploads, t->num_downloads);


	// We no longer allow anonymous to auto kick!
	// but that is tested elsewhere

	if (t->status & (ST_DUPELOGIN | ST_DUPEDOWN)) {
		lion_find(login_dupe_kicksub, t, NULL);

		// There is a bug here, lets clear them so they can login.
		t->data_active = 0;
		((struct quota_node *)t->quota)->num_active_retr = 0;
		((struct quota_node *)t->quota)->num_active_stor = 0;
		((struct quota_node *)t->quota)->dirty = 1;
	}



	// Quick check to see if the server is shutdown
	if (server_maximumlogins == 0
		&& !((t->level|server_defaultflags)&UF_ALL))
	{

		consolef("[loginpass] Server is shutdown!  Denied %s from "
				"%s:%d ident %s\n", t->user, lion_ntoa(t->host),
				t->port, t->ident);
		log_user("Server is shutdown!  Denied %s from "
				"%s:%d ident %s\n", t->user, lion_ntoa(t->host),
				t->port, t->ident);

		lion_printf(t->handle,"230 LOGIN DENIED, server is shutdown!\r\n");

		t->user[0] = 0;
		t->status &= ~ST_LOGIN; /* Just incase */
		return;

	}



	// Check that maximum_logins has not been reached

	lion_find(login_count_logins_sub, t->user, ints);

	//	consolef("[login] counter returns total %d, user %d\n",
	//	 ints[0], ints[1]);

	total_logins = ints[0];
	num = ints[1]; // -1 because it will count THIS login as well.


	/* num is additional logins to this one */

	/* if LOGINS is set, allow any number */
	if (t->num_logins <= num) {
		consolef("Concurrent logins denied on user %s from %s:%d ident %s\n",
				 t->user, lion_ntoa(t->host), t->port, t->ident);
		log_user("Concurrent logins denied on user %s from %s:%d ident %s\n",
				 t->user, lion_ntoa(t->host), t->port, t->ident);
		lion_printf(t->handle,"230 LOGIN DENIED, Only %d logins allowed.\r\n",
					t->num_logins);

		t->user[0] = 0;
		t->status &= ~ST_LOGIN; /* Just incase */
		return;
	}


	// SERVER HAS REACHED MAXIMUMLOGINS
	if (server_maximumlogins > 0
		&& (total_logins >= server_maximumlogins)
		&& !((t->level|server_defaultflags)&UF_ALL))
		{

			consolef("[loginpass] Maximum logins reached, denied user %s "
					 "from %s:%d ident %s\n", t->user, lion_ntoa(t->host),
					 t->port, t->ident);
			log_user("Maximum logins reached, denied user %s from %s:%d "
					 "ident %s\n", t->user, lion_ntoa(t->host), t->port,
					 t->ident);

			lion_printf(t->handle,"530 LOGIN DENIED, site is full!\r\n");

			t->user[0] = 0;
			t->status &= ~ST_LOGIN; /* Just incase */
			return;

		}


	t->status |= ST_LOGIN;


	/* Load up our quota's. */
	/*  quota_new(t);*/

	consolef("login_pass(%p): Looking for group\n", t->handle);

	t->current_group = (void *)group_findcurr_byname(t->user);
	if (!t->current_group)
		t->current_group = (void *)group_findany_byname(t->user);

#if 0
	lion_printf(t->handle, "230-You are a member of groups: %s\r\n",
				group_listgroups(t->user));

	file_checkmessage(t, 230);
	file_hasmessages(t);

#endif

	//	consolef("login_pass(%p): retreiving quota\n", t->handle);

	if ((quota = quota_getquota(t))) {
		quota->login_times++;
		quota->dirty = 1;
	}



	// user capping code.
	// if we are the first login for this user, we need to go fetch a new
	// "group" from lion. If we are not, we can just look it up in the
	// other login.

	// Only execute this if capping is actually requested.
	if (server_capping &&
		(server_cap_user_download || server_cap_user_upload)) {

		tmp = login_isloggedin_notus(t->user, t);

		if (tmp) {

			t->cap_group = tmp->cap_group;
			//consolef("[user_cap] reusing group %d\n", t->cap_group);

		} else {

			t->cap_group = lion_group_new();
			lion_group_rate_in (t->cap_group, server_cap_user_upload);
			lion_group_rate_out(t->cap_group, server_cap_user_download);
			//consolef("[user_cap] set new group %d\n", t->cap_group);

		}

	}






	socket_number(t, 230, t->user);

	consolef("login_pass(%p): User %s logged in %s", t->handle, t->user,
			 ctime(&t->time));

	log_user("%s logged in  %s", t->user,
			 ctime(&t->time));

}

void login_syst(struct login_node *t, char *args)
{
	consolef("login_syst(%p): %s %d\n", t->handle, args, t->status);

	socket_number(t, 215, NULL);

}

void login_quit(struct login_node *t, char *args)
{
	consolef("login_quit(%p): %s\n", t->handle, args);
#ifndef WIN33
	socket_number(t, 221, NULL);
#endif
	login_abort_loop = 1;
	login_exit(t);

}


/*
 *
 * Authorised functions
 *
 */

void login_port(struct login_node *t, char *args)
{
	int port;

	consolef("login_port(%p): %s\n", t->handle, args);

#ifdef WITH_SSL
	// If we only allowed encrypted data session, check if we reject it here.
	if (server_allow_only_secure_data && !(t->status & ST_DATA_SECURE) &&
		t->host != 0x7f000001) {

		consolef("login_port(%p) refusing %s port due to insecure\n",
				 t->handle, t->user);
		socket_number(t, 500, "only SECURE data channels permitted");
		return;
	}

#endif

	// Remote-slave code. The code should really not be here, but we
	// allow for defaults (does this break PRET RFC?). Generally, the
	// PRET command should have been issued that tells us where to send
	// the PASV, if anywhere.
	// FIXME: wrt to PRET command.
	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_PORT, args);
		return;
	}

	// If they've issued PORT or PASV before, we need to clear them out.
	// (more so with PASV).
	data_clearall(t);



	if (!lion_ftp_pasv( args, &t->port_host, &port )) {

		socket_number(t, 500, args); /* LAAAAME reply */
		return;
	}

	t->port_port = port;


	consolef("login_port(%p/%s): %s:%d\n", t->handle,
			 t->user,
			 lion_ntoa(t->port_host),
			 t->port_port);


	/* Don't reply authoritively yet*/


	socket_number(t, 200, "PORT");
}




/*
 * Note: a response of 425 is not mentioned as a possible response to
 *      the PASV command in RFC959. However, it has been blessed as
 *      a legitimate response by Jon Postel in a telephone conversation
 *      with Rick Adams on 25 Jan 89.
 */

void login_pasv(struct login_node *t, char *args)
{
	int pickone = 0, i;
	lion_t *data_handle = NULL;
	static int last_used = 0;

	consolef("login_pasv(%p): %s\n", t->handle, args);

#ifdef WITH_SSL
	// If we only allowed encrypted data session, check if we reject it here.
	if (server_allow_only_secure_data && !(t->status & ST_DATA_SECURE) &&
		t->host != ntohl(0x7f000001)) {

		consolef("login_pasv(%p) refusing %s pasv due to insecure\n",
				 t->handle, t->user);
		socket_number(t, 425, "only SECURE data channels permitted");
		return;
	}

#endif

	// Remote-slave code. The code should really not be here, but we
	// allow for defaults (does this break PRET RFC?). Generally, the
	// PRET command should have been issued that tells us where to send
	// the PASV, if anywhere.
	// FIXME: wrt to PRET command.
	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_PASV, NULL);
		return;
	}





	// If they've issued PORT or PASV before, we need to clear them out.
	// (more so with PASV).
	data_clearall(t);


	// Logic.
	// If server_data_pasv_start - the PASV raange is set, we will for loop
	// that range, starting from the last port used + 1, until we find one that
	// succeeds (technically, we could loop sockets and work out one that is
	// free? Is that more efficient? We should always have more ports than
	// connections, so perhaps it would be).
	// If we find one that works, great.
	//
	// If range isn't set, just use 0, and OS picks one.

	if (server_data_pasv_start) {

		if (!last_used)
			last_used = server_data_pasv_start - 1;

		for(i  = server_data_pasv_start;
			i <= server_data_pasv_end;
			i++) {
		  /* Think our '+1' issue is here
If our upper limit is 5, and we enter this loop with last_used = 5,
last_used++ is 6, and we test data_handle for 6, which may succeed
but would be outside our limit.
 so, alter check to be >= ...
		   */
			if (last_used >= server_data_pasv_end)
				last_used = server_data_pasv_start;
			else
				last_used++;
			pickone = last_used;

			// This sends an event, which is annoying.
			data_handle = lion_listen(&pickone,
		      	  (server_bindif) ? lion_addr(server_bindif) : 0,
			  0, NULL);

			if (data_handle)
				break;
		}
	} else {
		/* This is hard */
		data_handle = lion_listen(&pickone,
			  (server_bindif) ?  lion_addr(server_bindif) : 0,
			  0, NULL);

	}
	if (!data_handle) {
		/* Can't Passive */
		socket_number(t, 425, "can't build passive socket");
		/* LAAAAME reply */
		return;
	}

	// Lion stores everything host-order for you.
	//t->port_port = htons(pickone);
	t->port_port = pickone;

	/* Set up a new data node */

	consolef("login_pasv(%p/%s): Adding new data node %p port %d\n",
			 t->handle,
			 t->user,
			 data_handle,
			 pickone);

	if (!data_pasv(t, data_handle)) {
		socket_number(t, 425, "can't make passive connect"); /* LAAAAME reply */
		consolef("Passive failed!\n");
		return;
	}




	/* Make a passive string */
	/* 227 Entering Passive Mode (1,1,1,6,4,8) */



	/* Get our external IP - unless supplied */

	if (!main_passiveIP) {

		lion_getsockname( t->handle, &t->port_host, NULL);

	} else { /* external IP supplied */

		bcopy(&main_passiveIP, &(t->port_host), sizeof(t->port_host));

	}




	/* Assign this again, since data_init wipes it */
	//t->port_port = htons(pickone);
	t->port_port = pickone;



	if(!mystrccmp(commands2[t->last_command - 1024].name, "EPSV")) {

		lion_printf(t->handle, "229 Entering Extended Passive Mode (|||%u|)\r\n",
					(unsigned int)pickone);

		consolef("229 Entering Extended Passive Mode (|||%u|)\r\n",
				 (unsigned int)pickone);

	} else {
		char *str;

		str = lion_ftp_port( t->port_host, t->port_port );

		lion_printf(t->handle, "227 Entering Passive Mode (%s)\r\n", str);

		consolef("227 Entering Passive Mode (%s) => %d\r\n", str, pickone);
	}

}



//
// arg1 is "char *" to user name we are trying to find.
// arg2 is optional "login_struct *" of a node to ignore.
//
int login_isloggedin_sub(lion_t *handle, void *arg1, void *arg2)
{

	struct login_node *p;

	// NOT LOGIN NODE
	if (lion_get_handler(handle) != login_handler)
		return 1;

	// Grab login_node data for this handle
	p = (struct login_node *) lion_get_userdata( handle );

	if (arg2 && (p == arg2))
		return 1;  // this is us, skip it too...


	// The man we're looking for?
	if (p && !strcasecmp(p->user, (char *)arg1)) {
		return 0; // stop looping, return this handle.
	}

	// Keep searching...
	return 1;
}

struct login_node *login_isloggedin(char *user)
{
	struct login_node *t = NULL;
	lion_t *handle;

	handle = lion_find( login_isloggedin_sub, (void *)user, NULL);

	if (handle)
		t = lion_get_userdata( handle );

	return t;
}


//
// same as isloggedin, but ignores an "us" node passed along, so we can get
// another login that isn't us.
//
struct login_node *login_isloggedin_notus(char *user, struct login_node *notme)
{
	struct login_node *t = NULL;
	lion_t *handle;

	handle = lion_find( login_isloggedin_sub, (void *)user, (void *)notme );

	if (handle)
		t = lion_get_userdata( handle );

	return t;
}










void login_list(struct login_node *t, char *args)
{
	struct data_node *newdata;
	static char args2[1024], path[1024];  // BUFFER!!

	// Max input is 256, but we expand the path too.



	// If we are in a remote section, relay command instead -
	// this really is checked in PRET, and not here. We should
	// also check the path given to LIST since it can be outside.
	// FIXME, handle the situation where we "LIST -l /remote/"
	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_LIST, args);
		return;
	}


  	misc_getargs(args, args2, path);

	consolef("login_list(%p): %s -- %s\n", t->handle, args2, path);
	//consolef("Num l%d u%d d%d\n", t->num_logins, t->num_uploads, t->num_downloads);




	newdata = data_list(t, args2, path);
	if (!newdata) return;

	/* Do something here if we want */

	/*
	  150 Opening BINARY mode data connection for .message (111 bytes).
	  226 Transfer complete.
	*/

}







void login_type(struct login_node *t, char *args)
{
	consolef("login_type(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "TYPE");
		return;
	}

	// Technically you can set TYPE before you CWD to where you want
	// so this would break then. FIXME?
	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_TYPE, args);
		return;
	}


	if ((*args == 'i') || (*args == 'I')) {
		t->mode |= MODE_TYPE_BINARY;
	} else
		if ((*args == 'a') || (*args == 'A')) {
			t->mode &= ~MODE_TYPE_BINARY;
		} else {
			socket_number(t, 500, "TYPE");
			return;
		}

	lion_printf(t->handle, "200 Type set to %c.\r\n", (t->mode&MODE_TYPE_BINARY?'I':'A'));

}


void login_retr(struct login_node *t, char *args)
{
	consolef("login_retr(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "RETR");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, path2file(args));
		return ;
	}
	if (remote_ispret(t) || section_remote(args)) {
		log_xfer("RETR %s %s (remote)\n", t->user, args);
		remote_relay(t, REMOTE_CMD_RETR, args);
		return;
	}



	data_retr(t, args);

}


void login_rest(struct login_node *t, char *args)
{
	lion64u_t restart;
	consolef("login_rest(%p): %s\n", t->handle, args);

	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_REST, args);
		return;
	}

#ifndef WIN32
	restart = strtoull(args, NULL, 10);
#else
	restart = _atoi64(args);
#endif

	t->data_restart = restart;

	lion_printf(t->handle, "350 Restarting at %"PRIu64". Send STORE or RETRIEVE to initiate transfer.\r\n", restart);

}


void login_stor(struct login_node *t, char *args)
{

	consolef("login_stor(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "STOR");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, path2file(args));
		return ;
	}
	if (remote_ispret(t) || section_remote(args)) {
		log_xfer("STOR %s %s (remote)\n", t->user, args);
		remote_relay(t, REMOTE_CMD_STOR, args);
		return;
	}

	data_stor(t, args, 0);

}



void login_mdtm(struct login_node *t, char *args)
{
	struct stat st;
	struct tm *tt;
	consolef("login_mdtm(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "MDTM");
		return;
	}
	/*
	  ftp> quote MDTM src
	  550 src: not a plain file.
	  ftp> quote MDTM quake.txt
	  213 19970205115719
	  -rw-r--r--   1 root     admin    22533 Feb  5 11:57 quake.txt
	  550 blahblah: No such file or directory
	*/

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, path2file(args));
		return;
	}

	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_MDTM, args);
		return;
	}


	if (stat(args, &st)) {
		socket_number(t, 550, path2file(args));
		return;
	}

	if (!(st.st_mode & S_IFREG)) {
		lion_printf(t->handle, "550 %s: not a plain file.\r\n", path2file(args));
		return;
	}

	/* Formulate the reply */
	tt = gmtime(&st.st_mtime);
	lion_printf(t->handle, "213 "
				"%04d%02d%02d%02d%02d%02d\r\n",
				1900 + tt->tm_year,
				tt->tm_mon+1, tt->tm_mday,
				tt->tm_hour, tt->tm_min, tt->tm_sec);
}

void login_appe(struct login_node *t, char *args)
{
	struct data_node *nd;

	consolef("login_appe(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "APPE");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, path2file(args));
		return ;
	}
	if (remote_ispret(t) || section_remote(args)) {
		log_xfer("APPE %s %s (remote)\n", t->user, args);
		remote_relay(t, REMOTE_CMD_APPE, args);
		return;
	}


	nd = data_stor(t, args, DATA_APPE); /* FILE_APPE? */

	// Ok, so data_stor used DATA_APPE, so setting after was rather pointless.
	//if (nd) {
	//	nd->type |= DATA_APPE;
	//}

}


void login_size(struct login_node *t, char *args)
{
	consolef("login_size(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "SIZE");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, path2file(args));
		return ;
	}
	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_SIZE, args);
		return;
	}


	file_size(t, args);

}


void login_cwd(struct login_node *t, char *args)
{
	char path[1024], *dir, path2[1024];


	consolef("login_cwd(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		socket_number(t, 550, NULL);
		return;
	}

	if (*args == '/')
		snprintf(path, sizeof(path), "%s/", args);
	else
		snprintf(path, sizeof(path), "%s/%s/", t->cwd, args);

	file_undot(path);

	//consolef("[CWD] Straight to CD...\n");


	// remote-slave section, if so, relay whatever we eventually will try
	// to CWD to, and relay off.
	// However, if the CWD worked, we don't update t->cwd!
	// FIXME: wrt to CWD on remote succeeding.
	if (section_remote(path)) {
		remote_relay(t, REMOTE_CMD_CWD, path);
		return;
	}


	/* try straight CD */
	if (file_isdir(t, path)) {


		// Permission?
		if (section_grouped(path) &&
			!section_group_ismember(path,t->user)) {
			socket_number(t, 581, "required for this section");
			return;
		}


		free(t->cwd);
		t->cwd = mystrcpy(path);

		// Check for section_expand
		if ((dir = section_expand(path))) {

			lion_printf(t->handle, "250-Path expanded to '%s'\r\n", dir);
			login_cwd(t, dir);
			return;
			//SAFE_COPY(t->cwd, dir);

		}


		file_checkmessage(t, 250);

		file_free(t, t->cwd, 250);

		socket_number(t, 250, NULL);
		return ;
	}

	//consolef("[CWD] Wildcard to CD...\n");
	/* Try wildcard CD */
	if ((dir = file_parsedir(t, args, 0, FILE_FINDFIRST, FNM_CASEFOLD))) {

		snprintf(path, sizeof(path), "/%s/%s/", t->cwd, dir);

		if (file_isdir(t, path)) {

			snprintf(path2, sizeof(path2), "/%s/", path);

			file_undot(path2);

			// We would expose the path here, if they have no permission to
			// enter this directory. We need to check for permission here too
			// which is a shame.

			// Permission?
			if (section_grouped(path2) &&
				!section_group_ismember(path2,t->user)) {
				socket_number(t, 581, "required for this section");
				return;
			}



			lion_printf(t->handle, "250-Path expanded to '%s'\r\n", dir);

			login_cwd(t, path2);
			return;

		}

	}


	//consolef("[CWD] Smartpath to CD...\n");
	// Now, try smart paths
	if ((dir = section_find_smartpath(path))) {
		//consolef("[CWD] Found %s\n", dir);

		// call file undor??

		login_cwd(t, dir);  // hey, this is neat!
		return;

	}


	/* Oh well, finally fail */

	lion_printf(t->handle, "550 %s: Not a directory.\r\n", path2file(args));


}

void login_pwd(struct login_node *t, char *args)
{
	consolef("login_pwd(%p): %s\n", t->handle, args);

	socket_number(t, 257, t->cwd);

}

void login_cdup(struct login_node *t, char *args)
{
	consolef("login_cdup(%p): %s\n", t->handle, args);

	login_cwd(t, "..");

}

void login_noop(struct login_node *t, char *args)
{
	consolef("login_noop(%p): %s\n", t->handle, args);

	socket_number(t, 200, "NOOP");

	return;

	/* NOT REACHED */
#if 1
	lion_printf(t->handle,
				"200-The code of Hubba.\r\n"
				"    Far off, in a history shrouded by the mists of time, lived an\r\n"
				"    ancient warrior race.  Their code embodied the proud traditions\r\n"
				"    of a noble people, and was known and respected by all.  This code,\r\n"
				"    which was strictly enforced, held the warriors to always live for\r\n"
				"    the moment (which moment, it never said), to never flee a battle\r\n"
				"    (slower than your pursuers) and, above all, to always drink of the\r\n"
				"    magic potion, the liquid of life, the aqua vitae they called \"Cohk\".\r\n"
				"     \r\n"
				"    These people became extinct almost overnight, for reasons which\r\n"
				"    baffle anthropologists to this day.  Their code however, lives on;\r\n"
				"    practised in secrecy by trained initiates and passed on by word\r\n"
				"    of mouth to the new disciples.  It is: The Way of the Hubba.\r\n");
#endif


}


void login_stat(struct login_node *t, char *args)
{
	time_t tt;
	int an;
    struct data_node *d;
	static char args2[1024], path[1024];  // BUFFER!!

	consolef("login_stat(%p): %s\n", t->handle, args);



    // No arguments mean we just send STAT.
	if (!args || !*args) {

        time(&tt);

        /*
         *
         211-tao.sans.vuw.ac.nz FTP server status:
         Version wu-2.4(1) Sat Jan 14 09:47:54 NZDT 1995
         Connected to tao.sans.vuw.ac.nz (202.20.76.11)
         Logged in anonymously
         TYPE: ASCII, FORM: Nonprint; STRUcture: File; transfer MODE: Stream
         No data connection
         211 End of status
         *
         */

        an = strcmp(t->user, "ftp");

        // Secret command
        lion_printf(t->handle,
                    "211-%s FTP server status:\r\n"
                    "    Version %s build %u %s"  /* ctime provides \r\n */
                    "    Connected to %s (%s)\r\n"
                    "    Logged in %s%s\r\n"
                    "    TYPE: %s, FORM: notimp; STRUcture: File; transfer MODE: Stream\r\n"
                    "    %d active data connection%s\r\n",
                    server_greeting ? server_greeting : localhost_a,
                    VERSION_STR, VERSION_BUILD, ctime(&tt),
                    server_greeting ? server_greeting : localhost_a,
                    server_greeting ? server_greeting : ul2a(localhost),
                    (an ? "as " : ""), (an ? t->user : "anonymously"),
                    (t->mode&MODE_TYPE_BINARY ? "BINARY" : "ASCII"),
                    t->data_active, (t->data_active == 1 ? "" : "s"));

        quota_status(t);

        lion_printf(t->handle, "211 End of status\r\n");
        return;
    }


    // Else, STAT with directory listing.


	// If we are in a remote section, relay command instead -
	// this really is checked in PRET, and not here. We should
	// also check the path given to LIST since it can be outside.
	// FIXME, handle the situation where we "LIST -l /remote/"
	if (remote_ispret(t) || section_remote(t->cwd)) {
		//remote_relay(t, REMOTE_CMD_STATLIST, args);
		return;
	}


  	misc_getargs(args, args2, path);

	consolef("login_statlist(%p): %s -- %s\n", t->handle, args2, path);

    // Mark us as being in stat list
    t->status |= ST_STATLIST;


    // We re-use the data_list() for its many sanity checks. But ditch the
    // data node fairly early on.
    // We need to set these to fool data_list:
    t->port_host = 0x7f000001;
    t->port_port = 20;

	d = data_list(t, args2, path);

    t->port_host = 0;
    t->port_port = 0;

	if (!d) {

        t->status &= ~ST_STATLIST;
        lion_printf(t->handle, "213 No status of %s possible.\r\n",
                    path);
        return;
    }

    lion_printf(t->handle, "213-Status of %s:\r\n", path);

    lion_set_handler(t->handle, login_statlist_handler);
    dirlist_list(t->handle, d->name, "" /*d->list_precat*/,
                 d->sort_by | DIRLIST_PIPE | DIRLIST_USE_CRNL,
                 t);


    data_close(d);


}


void login_nlst(struct login_node *t, char *args)
{
	struct data_node *newdata;
	char args2[1024], path[1024];

	consolef("login_nlst(%p): %s\n", t->handle, args);

	/*
	 * here, I chose to open a dir myself using opendir() and then
	 * parsing each entry through fnmatch().
	 */
#if 0
	if (*args == '-') { /* CRAPPY NCFTP uses this instead of LIST */
		/* Appearently, they no longer do, NOW they mean NLST */
		login_list(t,args);
		return;
	}
#endif

	/* Bah, changed my mind, using /bin/ls :) */

	// If we are in a remote section, relay command instead -
	// this really is checked in PRET, and not here. We should
	// also check the path given to LNST since it can be outside.
	// FIXME, handle the situation where we "LNST -l /remote/"
	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_NLST, args);
		return;
	}

	/* and now, I've doing what I said above */

	misc_getargs(args, args2, path);

	newdata = data_nlst(t, args2, path);
	if (!newdata) return;

	/* Do something here if we want */

}


void login_mkd(struct login_node *t, char *args)
{
	char path[1024], *slash;

	consolef("login_mkd(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "MKD");
		return;
	}


	strcpy(path, args);

	if (section_uppercasedir(path)) {

		if ((slash = path2file(path))) {  // There is a slash

			if (isalpha(*slash))
				*slash = toupper(*slash);

		}

	}


	if (!file_pathchecklogin(t, path)) {
		socket_number(t, 550, NULL);
		return;
	}

	if (remote_ispret(t) || section_remote(path)) {
		remote_relay(t, REMOTE_CMD_MKD, path);
		return;
	}


	if (section_denydir(path)) {
		socket_print(t, "550 MKD '%s' is denied from being "
					 "uploaded.\r\n", path2file(path));
		consolef("Denied the creation of dir '%s' by %s\n",
				 path2file(path), t->user);
		return;

	}

	file_mkdir(t, path);

}



void login_rmd(struct login_node *t, char *args)
{
	//	char path[1024];

	consolef("login_rmd(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "RMD");
		return;
	}

	//	strcpy(path, args); // FIXME!!

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, NULL);
		return;
	}

	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_RMD, args);
		return;
	}

	file_rmdir(t, args);

}




void login_dele(struct login_node *t, char *args)
{
	//	char path[1024];


	consolef("login_dele(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "DELE");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, NULL);
		return;
	}

	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_DELE, args);
		return;
	}

#if 0
	if (*args == '/')
		sprintf(path, "%s", args);
	else
		sprintf(path, "%s/%s", t->cwd, args);

	file_undot(path);
#endif

	file_delete(t, args);


}




void login_abor(struct login_node *t, char *args)
{
	struct data_node *p;

	consolef("login_abor(%p): %s\n", t->handle, args);

	if (t->data_active <= 0) {
		lion_printf(t->handle, "225 ABOR command successful. Ofcourse there was nothing to abort.\r\n");
		return;
	}

	if (t->data_active == 1 && t->data) {
		data_close((struct data_node *)t->data);
		lion_printf(t->handle, "225 ABOR command successful.\r\n");
		return;
	}

	/* More than one data session! */

	if (!args || !*args) {
		lion_printf(t->handle, "500 Ambiguous ABORT command, please specify file to abort with ABOR <file>\r\n");
		return;
	}

	/* It's lame to use strstr() !! :) heheheh... */

	for (p = (struct data_node *)t->data; p; p=p->next) {

		if (!cmptail(p->name, args)) {   /* Found the file */

			lion_printf(t->handle, "225 ABOR command successful. Aborting file %s at %ld bytes\r\n", p->name, p->bytes);
			data_close(p);
			return;
		}
	}

	lion_printf(t->handle, "500 ABOR command failed, could not find file %s\r\n",
				args);
}



void login_rnfr(struct login_node *t, char *args)
{
	consolef("login_rnfr(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "RNFR");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, NULL);
		return;
	}

	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_RNFR, args);
		return;
	}

	data_rnfr(t, args);

}



void login_rnto(struct login_node *t, char *args)
{
	consolef("login_rnto(%p): %s\n", t->handle, args);

	if (!args || !*args) {
		help_commands(t, "RNTO");
		return;
	}

	if (!file_pathchecklogin(t, args)) {
		socket_number(t, 550, NULL);
		return;
	}

	if (remote_ispret(t) || section_remote(args)) {
		remote_relay(t, REMOTE_CMD_RNTO, args);
		return;
	}

	data_rnto(t, args);
}



void login_countstats()
{
#ifdef HASSETPROCTITLEXX  // FIXME LION!

	int l=0, d=0;
	struct login_node *ll;
	struct data_node *dd;

	for(ll=login_node_head; ll; ll=ll->next) {
		for (dd=ll->data; dd; dd=dd->next)
			d++;
		l++;
	}

	/* not portable this... */
	setproctitle("%d user %d data", l, d);

#endif
}



/*
 * NO REPLY FROM THIS COMMAND!
 *
 * This is an protocol-illegal command :)
 *
 * Used only in conjunction with ShadowFTPD/ssl-lbnc
 *
 */
void login_iden(struct login_node *t, char *args)
{
	char rempat[IPMATCHLEN];

	consolef("login_iden(%p): '%s'\n", t->handle, args);

	if (!args || !*args) return;


	snprintf(rempat, sizeof(rempat),
			 "iden-cmd@%s", lion_ntoa(t->host));


	// Sets our real auth line, but it isn't actually checked until we
	// attempt to login. We should probably ensure it comes from a valid place
	// however, so scan the global list.
	/* Check global patterns */
	if (misc_patt_match_gbl(PATT_GLOBAL, rempat)) {

		consolef("user_iden(%d): valid GLOBAL pattern '%s'\n",
				 -1, rempat);


		t->auth = mystrcpy(args);
		t->status |= ST_AUTHOK;

		return;
	}

	consolef("[login] iden: un-authorised IDEN command. (Not from global ip match list)\n");

	return;

}


struct login_node *login_console_user(void)
{
	static struct login_node console;

	memset(&console, 0, sizeof(console));

	strcpy(console.user, "Console");
	console.offset = 1; /* Illegal offset, this is to ensure we NEVER get
						   written to disk, SHOULD something call login_exit()
						*/

	// FIXME!! lion!
	//console.socket = fileno(stdout);

	return &console;

}


void login_clnt(struct login_node *t, char *args)
{
	consolef("login_clnt(%p): %s\n", t->handle, args);

	log_user("%s uses client %s\n", t->user,
			 args);

	lion_printf(t->handle, "200 Noted.\r\n");

}


void login_tls_auth(struct login_node *t, char *args)
{

	//	consolef("[tls] attempting SSL\n");

#ifdef WITH_SSL
	if (lion_ssl_set(t->handle, LION_SSL_SERVER) == 1) {

		// Are we allowed to send something after starting the SSL?
		lion_printf(t->handle,"234 Attempting TLS connection\r\n");
		return;
	}
#endif

	socket_number(t, 500, "AUTH"); /* Yes it IS the wrong reply but can't be
									  bothered now */


}



void login_auth(struct login_node *t, char *args)
{
	char *ar = args, *type;
	int i;
	struct command_s auth_cmd[] = {
#ifdef WITH_SSL
		{ "TLS"  , login_tls_auth },
		{ "TLS-C", login_tls_auth },
		{ "SSL"  , login_tls_auth },
#endif
		{ (char *)0, (void *)0 }
	};

	if (!args || !*args) return;

	type = digtoken((char **)&ar, " ");

	for(i = 0; auth_cmd[i].name; i++)
		if(!mystrccmp(auth_cmd[i].name, type)) {
			auth_cmd[i].function(t, args);
			return;
		}
	socket_number(t, 500, "AUTH"); /* Yes it IS the wrong reply but can't be
									  bothered now */

}

void login_pbsz(struct login_node *t, char *args)
{
#ifdef WITH_SSL
	if (lion_ssl_enabled(t->handle)) {
		consolef("[login] PBSZ\n");
		lion_printf(t->handle, "200 OK\r\n");
		return;
	}
#endif

	socket_number(t, 500, "PBSZ"); /* Yes it IS the wrong reply but can't be */

}

void login_prot(struct login_node *t, char *args)
{
	char *ar = args, *type;

	consolef("login_prot(%p): %s\n", t, args);

	if (!args || !*args) {
		help_commands(t, "PROT");
		return;
	}

	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_PROT, args);
		return;
	}

#ifdef WITH_SSL
	if (lion_ssl_enabled(t->handle)) {


		if (!args || !*args) return;

		type = digtoken((char **)&ar, " ");

		if (mystrccmp(type, "C") && mystrccmp(type,"P")) {
			socket_print(t, "504 PROT must be P or C, not %s\r\n", type);
			return;
		}

		if (type[0] == 'P')
			t->status  |= ST_DATA_SECURE;
		else if (type[0] == 'C')
			t->status &= ~ST_DATA_SECURE;

		consolef("[login] PROT %c\n", type[0]);
		lion_printf(t->handle, "200 OK\r\n");
		return;
	}
#endif

	socket_number(t, 500, "PROT"); /* Yes it IS the wrong reply but can't be */

}


void login_ccsn(struct login_node *t, char *args)
{
	char *ar = args, *type;

	consolef("login_ccsn(%p): %s\n", t, args);

	if (!args || !*args) {
		lion_printf(t->handle, "200 %s Mode\r\n",
					t->mode&MODE_TYPE_CCSN?"Client":"Server");
		return;
	}

	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_CCSN, args);
		return;
	}

	type = digtoken((char **)&ar, " ");

	if (mystrccmp(type, "ON") && mystrccmp(type,"OFF")) {
		socket_print(t, "504 CCSN must be ON or OFF, not %s\r\n", type);
		return;
	}

	if (toupper(type[1]) == 'N')
		t->mode  |= MODE_TYPE_CCSN;
	else if (toupper(type[1]) == 'F')
		t->mode &= ~MODE_TYPE_CCSN;

	consolef("[login] CCSN %s\n", t->mode&MODE_TYPE_CCSN?"ON":"OFF");
	lion_printf(t->handle, "200 %s Mode\r\n",
				t->mode&MODE_TYPE_CCSN?"Client":"Server");

}


void login_sscn(struct login_node *t, char *args)
{
	char *ar = args, *type;

	consolef("login_sscn(%p): %s\n", t, args);

	if (!args || !*args) {
		lion_printf(t->handle, "200 %s Mode\r\n",
					t->mode&MODE_TYPE_SSCN?"Client":"Server");
		return;
	}

	if (remote_ispret(t) || section_remote(t->cwd)) {
		remote_relay(t, REMOTE_CMD_SSCN, args);
		return;
	}

	type = digtoken((char **)&ar, " ");

	if (mystrccmp(type, "ON") && mystrccmp(type,"OFF")) {
		socket_print(t, "504 SSCN must be ON or OFF, not %s\r\n", type);
		return;
	}

	if (toupper(type[1]) == 'N')
		t->mode  |= MODE_TYPE_SSCN;
	else if (toupper(type[1]) == 'F')
		t->mode &= ~MODE_TYPE_SSCN;

	consolef("[login] SSCN %s\n", t->mode&MODE_TYPE_SSCN?"ON":"OFF");
	lion_printf(t->handle, "200 %s Mode\r\n",
				t->mode&MODE_TYPE_SSCN?"Client":"Server");

}


void login_feat(struct login_node *t, char *args)
{
	consolef("login_feat(%p): %s\n", t->handle, args);

	/*
	  211-Features supported
	  MDTM
	  MLST Type*;Size*;Modify*;Perm*;Unique*;
	  REST STREAM
	  SIZE
	  TVFS
	  211 End
	*/

	lion_printf(t->handle, "211-Features supported\r\n");
	lion_printf(t->handle, " MDTM\r\n");
	lion_printf(t->handle, " REST STREAM\r\n");
	lion_printf(t->handle, " SIZE\r\n");
	lion_printf(t->handle, " PRET\r\n");
#ifdef WITH_SSL
	lion_printf(t->handle, " AUTH TLS\r\n");
	lion_printf(t->handle, " CCSN\r\n");
	lion_printf(t->handle, " SSCN\r\n");
#endif
	lion_printf(t->handle, " XDUPE %s\r\n",
				(t->options&(UO_XDUPE_2|UO_XDUPE_3)) ?
				"(Enabled)" : "(Disabled)");
	lion_printf(t->handle, "211 End\r\n");

}


//
// PRET - PreTransfer command.
//
void login_pret(struct login_node *t, char *args)
{
	char *ar, *cmd;
	char com[50];

	consolef("login_pret(%p): %s\n", t->handle, args);
	// From the DrFTPd documentation pages:
	// PRET LIST [ignored arguments]
	// PRET NLST [ignored arguments]
	// PRET RETR <file> [ignored arguments]
	// PRET APPE <file> [ignored arguments]
	// PRET STOR <file> [ignored arguments]
	// PRET STOU [ignored arguments]
	//
	// However, the LIST and NLST support is poor. Since you can
	// go "LIST /some/other/directory/"
	//
	// So we are forced to extend their RFC. Sorry.

	ar = args;

	cmd = misc_digtoken(&ar, " \r\n");

	if (!cmd) {
		socket_number(t, 1503, "Bad sequence of commands");
		return;
	}

	// optional path in ar.
	consolef("[login_pret] command '%s' and path '%s'\n",
			 cmd, ar);


	help_get4letter(cmd, com); // Make sure its 4 characters, and uppercase

	switch(TOINT(com[0], com[1], com[2], com[3])) {

		// These take <optional> path argument.
		// BROKEN. We need to remember that the PRE command was LIST
		// so that we can SKIP that CCSN is set if it is dirlisting, as per
		// RFC.
	case TOINT('L','I','S','T'):
	case TOINT('N','L','S','T'):
		if (ar && *ar && file_pathchecklogin(t, ar)) {
			if (section_remote(ar)) {
				remote_relay(t, REMOTE_CMD_PRET, ar);
				return;
			}
		} else {
			// Assume CWD if not supplied.
			if (section_remote(t->cwd)) {
				remote_relay(t, REMOTE_CMD_PRET, NULL);
				return;
			}
		}
		// Not actually for a remote area, so we just say ok.
		remote_clearpret(t);
		socket_number(t, 200, "PRET (local)");
		return;
		break;

		// These require path argument
	case TOINT('R','E','T','R'):
	case TOINT('A','P','P','E'):
	case TOINT('S','T','O','R'):
		if (!ar || !file_pathchecklogin(t, ar)) {
			socket_number(t, 1503, "Missing path argument, or invalid path");
			return;
		}

		if (section_remote(ar)) {
			remote_relay(t, REMOTE_CMD_PRET, ar);
			return;
		}

		remote_clearpret(t);
		socket_number(t, 200, "PRET (local)");
		return;
		break;

		// These are not implemented in LundFTPD to date.
	case TOINT('S','T','O','U'):
		socket_number(t, 1503, "PRET failed as command not implemented");
		return;
		break;

	}


	socket_number(t, 200, NULL);
}




//
// Open the listening port
//

THREAD_SAFE static lion_t *login_listen_handle = NULL;


//
// The handler for the listening socket.
//
int login_listen_handler( lion_t *handle,
						  void *user_data, int status, int size, char *line)
{
	lion_t *new_handle = NULL;

	// Sanity
	//	assert(handle == login_listen_handle);

	switch( status ) {

	case LION_CONNECTION_LOST:
		consolef("[listen] lost listening socket? %d:%s\n", size, line);

		/* Fall-through */
	case LION_CONNECTION_CLOSED:
		printf("[listen] connection closed\n");
		login_listen_handle = NULL;

  log_user("Server shutdown %s\n", ctime(&lion_global_time));

#ifdef IRCBOT
	printf("STOP\n");
	irc_announce("STOP|msg=Shutting down\n");
	irc_free();
#endif
		io_exit = 1;
		break;

	case LION_CONNECTION_NEW:
		consolef("[listen] New connection\n");
		new_handle = lion_accept(handle, 0, LION_FLAG_FULFILL,
								 NULL, NULL, NULL);

		lion_set_handler( new_handle, login_handler );

		break;

	} // switch

	return 0;
}



void login_listen(void)
{

	consolef("[login] Binding to %s\n", ul2a(ntohl(localhost)));

	login_listen_handle = lion_listen(&server_port,
									  (server_bindif) ?
								      lion_addr(server_bindif) : 0,
									  LION_FLAG_NONE, NULL);

	if (!login_listen_handle) {
		consolef("Failed to open listening socket\n");
		io_exit = 1;
		return ;
	}

	// fulfill always gives us a node, so we can safely set a handler.
	lion_set_handler( login_listen_handle, login_listen_handler );


	log_user("\nServer started  %s", ctime(&lion_global_time));

	consolef("[login] listen socket open - Server Started\n");

#ifdef IRCBOT
	irc_announce("START|time=%lu\n", lion_global_time);
#endif
}



void login_listen_free(void)
{

	if (login_listen_handle)
		lion_close(login_listen_handle);

}



int login_loop_sub(lion_t *handle, void *arg1, void *arg2)
{
	struct login_node *p;
    struct data_node *d;

    if (login_abort_loop) return 0;

	// NOT LOGIN NODE
	if (lion_get_handler(handle) != login_handler)
		return 1;

	// Grab login_node data for this handle
	p = (struct login_node *) lion_get_userdata( handle );

    // NULL?
    if (!p) return 1;

    // Has a data node?
    if (!p->data) return 1;


    d = (struct data_node *)p->data;
    if (d->status & DATA_WANTCONNECTED) {
        d->status &= ~DATA_WANTCONNECTED;
        consolef("[login] calling data_connected\n");
        data_connected(d);
        return 0;
    }

	// Keep searching...
	return 1;
}



void login_loop(void)
{

    // Iterate all login nodes
    login_abort_loop = 0;
	lion_find( login_loop_sub, NULL, NULL);
    login_abort_loop = 0;

}







//
// ***********************************************************************
//

// WARNING!! These are CALLBACK function from libdirlist, and are executed
// as ANOTHER FORKED PROCESS. This means you should take care in the
// implementation here.
char *login_gid_lookup( int gid )
{
	struct group *g;

	// Use system group names
	if (server_lookupuid)
		return NULL;

	if ((g = group_find_bygid(gid - server_useridstart)))
		return g->name;

	return server_nobody;

}


