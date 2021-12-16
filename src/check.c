#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef WIN32
#include <../include/io.h>   // MS picks local io.h first, this is WRONG.
#include "win32.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include "global.h"
#include "login.h"
#include "data.h"
#include "check.h"
#include "lfnmatch.h"
#include "misc.h"
#include "socket.h"
#include "exec.h"
#include "quota.h"
#include "log.h"
#include "file.h"
#include "quota.h"
#include "dupe.h"
#include "groups.h"
#include "user.h"
#include "sfv.h"
#include "section.h"
#ifdef RACESTATS
#include "race.h"
#endif
#ifdef IRCBOT
#include "irc.h"
#endif

#include "checkchild.h"

#include "mp3genre.h"

#include "lion.h"


#include "../lion/src/misc.h"
//#include "misc.h"



__RCSID("$LundFTPD: check.c,v 1.28 2003/04/23 08:32:03 lundman Exp $");



/* List of loaded ext */
struct check_node *check_node_head = NULL;

/* List of loaded smartpaths */
struct cd_node *cd_node_head = NULL;


void check_add(char *ext, char *keyword, int ret)
{
	struct check_node *c;

	server_userfilecheck = 1;

	if (!(c = (struct check_node *) malloc(sizeof(struct check_node)))) {
		perror("Failed to get memory for new entry");
		exit(-1);
	}
	memset(c, 0, sizeof(struct check_node));

	c->ext = ext;

	if (!strcasecmp(keyword, "FREE")) {

		SAFE_FREE(keyword);
		c->exe = (char *)CHECK_FREE;

	} else if (!strcasecmp(keyword, "NOCHECK")) {

		SAFE_FREE(keyword);
		c->exe = (char *)CHECK_NOCHECK;

	} else if (!strcasecmp(keyword, "INTERNAL")) {

		SAFE_FREE(keyword);
		c->exe = (char *)CHECK_INTERNAL;

	} else {

		c->exe = keyword;

	}

	c->ret = ret;
	c->next = check_node_head;
	check_node_head = c;

}



//
// This function is wrong.
//
void check_free(void)
{
  struct check_node *runner, *next;
  struct cd_node *run, *nex;

  consolef("[check] releasing check_node's\n");
  for (runner = check_node_head; runner; runner = next) {

    next = runner->next;

	SAFE_FREE(runner->ext);

    if (runner->exe &&
		(runner->exe != (char *)CHECK_FREE) &&
		(runner->exe != (char *)CHECK_NOCHECK) &&
		(runner->exe != (char *)CHECK_INTERNAL)) {
		SAFE_FREE(runner->exe);
	}

    SAFE_FREE(runner);

  }

  check_node_head = NULL;


  consolef("[check] releasing cd_node's\n");
  for (run = cd_node_head; run; run = nex) {

    nex = run->next;

    if (run->abbr)
      free(run->abbr);

    if (run->expa)
      free(run->expa);

    free(run);

  }

  cd_node_head = NULL;


  consolef("[check] releasing free_helper\n");
  check_free_helper();

}









/*
 *
 *****************************************************************************
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *****************************************************************************
 *
 * LION Checker Code!!
 *
 *****************************************************************************
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *****************************************************************************
 *
 */

static THREAD_SAFE lion_t *check_helper_handle = NULL;


//
// Technically we probably don't need to build a list really, but
// since lion don't have timeout events yet, we will want to loop
// through them to do manual timeouts.
//
static THREAD_SAFE checker_t *checker_head = NULL;





void check_manual_scan(char *path)
{
	struct data_node d;
	int ret;

	memset(&d, 0, sizeof(d));

	consolef("Attempting to pass '%s' to checker logic\n", path);
	SAFE_COPY(d.name, path);

	ret = check_filebad(&d);

	consolef("Completed with %d\n", ret);
}



//
// This function takes a data_node (as starting point), creates a new
// checker_t node, which is used for all checking, copies over all the
// information that is needed, from data, and the login. It also registers
// for a quota node for the user, so all the information you need should be
// in this. If you need to access more information, you should put in copy
// to copy it across here.
//
checker_t *check_makenode(struct data_node *d)
{
	checker_t *result;
	struct login_node fakelogin;  // node so we can call quota_new.
	static THREAD_SAFE unsigned int id_count = 0;
	struct timeval tv;

	result = (checker_t *) malloc( sizeof( *result ));

	if (!result) return NULL;

	memset(result, 0, sizeof (*result) );


	consolef("[check] makenode '%s' : %u\n",
			 d && d->name ? d->name : "(null)",
			 id_count);


	if (d) {

		if (d->login) {

			strncpy(result->user, d->login->user, MAXUSERNAME);
			result->user[MAXUSERNAME] = 0;

			result->current_group = d->login->current_group;

			fakelogin.offset = d->login->offset;
			result->offset   = fakelogin.offset;

			quota_new( &fakelogin );

			result->quota = fakelogin.quota;
			// quota shouldn't be NULL now.
			// if ( !fakelogin.quota ) ;  // do something?


		} else { // no login {

			strcpy(result->user, "<rescan>");

		} // login


		if (d->name) {
			char path[1024], *r;
			int n;

			result->name = mystrcpy( d->name );

			// Does the name given to us from client have a "/" in it?
			// if not, add on server root, and possibly cwd.
			// if so, uhmm.

			// just the filename.
			result->file_name = mystrcpy( path2file(d->name) );

			// just the directory
			r = (char *)strrchr(d->name, '/');

			if (!r)
				snprintf(path, sizeof(path), "%s/%s/",
						 server_usechroot, d->login->cwd);
			else {

				n = strlen( d->name ) - strlen( r );
				snprintf(path, sizeof(path), "%*.*s/", n, n, d->name);

			}

			result->dir_name = mystrcpy( path );

			// and both together.
			snprintf( path, sizeof(path), "%s%s",
					  result->dir_name, result->file_name);

			result->disk_name = mystrcpy( path );


#if 0
			consolef("[check] new check '%s'\n"
					 "        disk_name '%s'\n"
					 "         dir_name '%s'\n"
					 "        file_name '%s'\n",
					 result->name,
					 result->disk_name,
					 result->dir_name,
					 result->file_name);

#endif



		}
		result->bytes = d->bytes;
		result->time  = d->time;

	}

	// Assign the correct ratio based on section or default user ratio.
	result->ratio = (section_ratio(result->dir_name))
				? section_ratio(result->dir_name)
				: ((struct quota_node *)result->quota)->ratio ;

	result->id = id_count++;
#ifndef WIN32
	gettimeofday(&tv, NULL);
#endif
	result->timesec = tv.tv_sec;
	result->timeusec = tv.tv_usec;


	result->next = checker_head;
	checker_head = result;


	return result;

}


//
// This finally releases a check node, and frees all memory allocated.
//
void check_releasenode( checker_t *node)
{
	struct login_node fakelogin;  // node so we can call quota_new.
	checker_t *curr, *last;

	// release quota node, if any.
	if (node->quota) {

		fakelogin.quota  = node->quota;
		fakelogin.offset = node->offset;

		quota_exit( &fakelogin );

	}

	// Release name
	if (node->name) {
		free(node->name);
		node->name = NULL;
	}
	if (node->disk_name) {
		free(node->disk_name);
		node->disk_name = NULL;
	}
	if (node->dir_name) {
		free(node->dir_name);
		node->dir_name = NULL;
	}
	if (node->file_name) {
		free(node->file_name);
		node->file_name = NULL;
	}
	if (node->expected_value) {
		free(node->expected_value);
		node->expected_value = NULL;
	}
	if (node->computed_value) {
		free(node->computed_value);
		node->computed_value = NULL;
	}
	if (node->sfv_file_name) {
		free(node->sfv_file_name);
		node->sfv_file_name = NULL;
	}



	// Remove from linked list.
	for (curr = checker_head, last = NULL;
		 curr; last = curr, curr=curr->next) {

		if (curr == node) {
			if (!last)
				checker_head = curr->next;
			else
				last->next = curr->next;

			consolef("[check] free on %p id %u\n", node, node->id);
			// finally, release us.
			free( node );

			break;
		}
	}
}



checker_t *check_find( unsigned int id)
{
	checker_t *result;

	for (result = checker_head; result; result = result->next) {
		if (id == result->id)
			return result;
	}

	consolef("[check] find - no id %u\n", id);
	return NULL;
}








int check_handler( lion_t *handle,
				   void *user_data, int status, int size, char *line)
{
	char *token, *ar;
	unsigned int id;
	checker_t *check_node;

	consolef("[check] %p (%d)\n", handle, status);


	switch( status ) {

	case LION_PIPE_FAILED:
		consolef("[check] child lost: %s\n", line);
		check_helper_handle = NULL;

		// Maybe we want to restart it here, and re-issue the nodes?
		break;

	case LION_PIPE_EXIT:
		consolef("[check] child has exited.\n");

		check_helper_handle = NULL;

		// Maybe we want to restart it here, and re-issue the nodes?
		break;

	case LION_PIPE_RUNNING:
		consolef("[check] helper is alive.\n");

		// Check if we have some files to check from last time.
		check_resume();

		break;

	case LION_INPUT:
		consolef("[check] %p input '%s'\n", handle, line);

		// We have a reply from the helper. We expect back
		// "id:reply string" ie, "56:4f23e512" or similar.
		ar = line;

		token = misc_digtoken(&ar, ":!\r\n");
		if (!token) {
			consolef("[check] parse error from child '%s'.\n",
					 line);
			return 0;
		}

		id = atoi( token );

		check_node = check_find( id );

		// Check if it failed.
		if (!check_node) {
			consolef("[check] warning, failed to match %d:%s\n",
					 id, ar);
			return 0;
		}

		if (misc_digtoken_optchar == '!') {

			consolef("[check] tester failed. Admin, check path and permissions => %d:%s\n",
					 size, ar);
			check_node->computed_status = CHECK_UNKNOWN;
			check_action( check_node );

		} else {
			check_return( check_node, ar); // ar has everything after ":"
		}


#if 1
		// Check how many are outstanding..
		for (id = 0, check_node = checker_head;
			 check_node;
			 check_node = check_node->next, id++) /* empty */ ;

		consolef("[check] processing complete: %d item%s remaining.\n",
				 id, id == 1 ? "" : "s");
#endif

		break;

	default:
		consolef("[check] unhandled event %d for %p\n", status, handle);
		break;

	}

	return 0;

}




int check_init(void)
{

	if (!check_helper_handle) {

		check_helper_handle = lion_fork( checkchild_init,
										 LION_FLAG_FULFILL,
										 NULL, NULL );

		lion_set_handler( check_helper_handle, check_handler);
		// If we already have some checker nodes, we should probably
		// re-issue them again since we restarted it here? Although,
		// we would know that it died in the handler, so that should be
		// dealt with there.

	}

	return 1;
}




void check_free_helper(void)
{
	FILE *ffd = NULL;

	if (check_helper_handle) {
		lion_disconnect(check_helper_handle);
		check_helper_handle = NULL;
	}

	// Only create the file if there is a need.
	if (checker_head)
		ffd = fopen(CHECK_SAVE_FILENAME, "w");

	// Release nodes.
	while (checker_head) {

		consolef("[check] file left untested '%s'\n",
				 checker_head->disk_name);

		if (ffd)
			fprintf(ffd, "%s,%.2f,%d,%s\n",  // save group? friar?
					checker_head->user,
					(float) checker_head->bytes / 1048576.0,
					checker_head->ratio,
					checker_head->disk_name);


		// Maybe add code to call check_action() with UNKNOWN?

		check_releasenode( checker_head );
	}

	if (ffd)
		fclose(ffd);

	checker_head = NULL;

}





//
// Setup a new file checking node, based on a data_node. Creates a new
// checker_t node that contains all information we could want.
//
int check_filebad(struct data_node *d)
{
	int type;
	struct check_node *c;
	checker_t *check_node, *runner;


	// Handle new SFV files.
	if (!lfnmatch("*.sfv",  d->name, LFNM_CASEFOLD)
#ifdef WITH_SSL
		|| !lfnmatch("*.md5",  d->name, LFNM_CASEFOLD)
#endif
		) {
		struct sfv_state *ss = NULL;

		if (!sfv_state_load(d->name))
			ss = sfv_file_load(d->name);

		// Backcheck
		if (ss) {

			char tmp[MAX_PATHLEN];
			struct sfv_state_entry *sse;

#ifdef IRCBOT
			if (d->login && section_racestats(ss->path) &&
				section_announce(ss->path)) {

				irc_announce("SFV|section=%s|dir=%s|fname=%s|user=%s|count=%d\n",
							 section_name(ss->path), section_rlsfind(ss->path),
							 ss->filename, d->login->user, ss->count);
			}
#endif

			for (sse = ss->entry; sse; sse = sse->next) {

				snprintf(tmp, MAX_PATHLEN, "%s/%s", ss->path, sse->filename);

				// File exists and hasn't been checked, backcheck!
				if (!(sse->status & SFV_OK) && !access(tmp, F_OK)) {

					struct data_node dummy;

					bzero(&dummy, sizeof(struct data_node));
					dummy.name = strdup(tmp);
					dummy.login = d->login;
					// Don't award credits
					dummy.bytes = 0;
					consolef("[check] Backchecking %s...\n", path2file(dummy.name));
					check_filebad(&dummy);
                }

            }

		} // Backcheck

		return 1;
	}

#if 0
  if (!lfnmatch( "*.diz", check_node->name, LFNM_CASEFOLD)) {
	  check_loaddiz( d );
	  return 1;
  }
#endif



  check_node = check_makenode( d );

  if (!check_node)
	  return 0;


  // Lets loop through the list of pending tests and see if this file is
  // already in there, since there is no point in adding it again.

  for (runner = checker_head;
	   runner;
	   runner = runner->next) {

	  if ((runner != check_node) &&
		  !strcmp(check_node->disk_name,
				  runner->disk_name)) {

		  consolef("File '%s':%d already pending, dropping this test.\n",
				   runner->disk_name,
				   runner->id);

		  check_releasenode(check_node);
		  return 0;
	  }

  }









#if defined ( IRIX ) || ( HPUX ) || ( WIN32 )

  consolef("check_filebad(%s): checking file '%s' %llu bytes\n",
		   check_node->user,
		   check_node->name, check_node->bytes);

#elif defined ( DEC )

  consolef("check_filebad(%s): checking file '%s' %lu bytes\n",
		   check_node->user,
		   check_node->name, check_node->bytes);


#else

  consolef("check_filebad(%s): checking file '%s' %qu bytes\n",
		   check_node->user,
		   check_node->name, check_node->bytes);

#endif


   if (server_maxsizecheck && (check_node->bytes >=
							   ((lion64u_t)server_maxsizecheck
								* 1048576))) {

	   if (d->login && d->login->handle)
	   socket_print(d->login, "226-Archive greater than %dMB, skipping tests\r\n", server_maxsizecheck);

	   check_return(check_node, NULL);

	   return 0;

  }

#if 0
   // This appears to cause trouble.
   if (!d->bytes) {

	   consolef("check_filebad(%d/%d): 0 bytes appear to have been transfered -- skipping tests\n",
				d->login->socket,
				d->socket);

	   return 0;

   }
#endif


   // Fetch out imdb?
   // consolef("extra imdb says %d\n", section_extractimdb(check_node->dir_name));

   if (section_extractimdb(check_node->dir_name) &&
	   (!lfnmatch( "*.nfo", check_node->name, LFNM_CASEFOLD) ||
		!lfnmatch( "*.url", check_node->name, LFNM_CASEFOLD))) {

	   check_extractimdb(check_node);

   }





	for (c = check_node_head; c; c=c->next) {

		if (!lfnmatch(c->ext, d->name, LFNM_CASEFOLD)) {

			if ((c->exe == (char *)CHECK_NOCHECK)) {

				check_node->computed_status = CHECK_GOOD;


				check_action(check_node);
				return 0;

			} else if ((c->exe == (char *)CHECK_FREE)) {

				// Nothing we need to do, just release the node
				check_releasenode(check_node);
				return 0;

			} else if ((c->exe == (char *)CHECK_INTERNAL)) {

				check_node->expected_value =
					sfv_state_crc(check_node->file_name,
								  check_node->dir_name);

				if (check_node->expected_value) {
					check_node->expected_value =
						strdup( check_node->expected_value );
				} else {

					// Nothing to check, not CRC found for this file,
					// set the status to UNKNOWN.
					check_node->computed_status = CHECK_UNKNOWN;
					check_action(check_node);

					return 0;

				}

			} else {

				// EXTERNAL TESTING. Assign expected value
				check_node->expected_value = strdup( misc_itoa( c->ret ));

			}


			// The file is now in the queue to be tested, so lets make it
			// read-only so nobody else can over write it.
			//Itischmodedbackto permfile is testing fails and we are asked not
			// to delete the bad files.
			if (server_goodfile) {
				file_goroot();
				chmod(check_node->disk_name, (mode_t) server_goodfile);
				file_gononroot();
			}




			// We have a request here, so, we send it off to the
			// helper child. The we twiddle our thumbs until we get a reply
			// of I guess we should have a timeout?

			// Is the child around?
			if (!check_helper_handle) {

				check_helper_handle = lion_fork( checkchild_init,
												 LION_FLAG_FULFILL,
												 NULL, NULL );

				lion_set_handler( check_helper_handle, check_handler);
				// If we already have some checker nodes, we should probably
				// re-issue them again since we restarted it here? Although,
				// we would know that it died in the handler, so that should be
				// dealt with there.

			}



			//
			// Send request, child expects to receive
			// "id:type:/path/to/file name.txt", or
			// "56:0:/disk06/requests/rubbish/rubbish file.txt"

			// If we have a sfv filename, and it ends in md5
			//consolef("sfv: '%s' and expected '%s'\n",
			//		 check_node->sfv_file_name ? check_node->sfv_file_name : "(null)",
			//check_node->expected_value?check_node->expected_value:"(null)"
			//		 );

			// SFV or MD5 type?
			type = 0;

			if (check_node->sfv_file_name &&
				!lfnmatch("*.md5",  check_node->sfv_file_name,
						  LFNM_CASEFOLD)) {
				type = 1;
			} else { // No sfv filename, does the CRC expected length be 32?
				if (check_node->expected_value &&
					strlen(check_node->expected_value) == 32)
					type = 1;
			}

			consolef("[check] sending request to %p '%u:%u:%s'\n",
					 check_helper_handle,
					 check_node->id, type,
					 check_node->disk_name);


			lion_printf(check_helper_handle, "%u:%u:%s\n",
						check_node->id,
						type,
						check_node->disk_name);



			// We only do background testing now?
			if (d->login && d->login->handle) {

				if (server_backgroundtest)
					socket_print(d->login,
								 "226-Background archive testing started.\r\n");
				else
					socket_print(d->login,
								 "226-Testing archive, please wait...\r\n");

			}


			return 0; /* 0 is good */

		} // if fnmatch

	} // for check nodes


	// extention was not found.

	if (d->login && d->login->handle)
		socket_print(d->login,
					 "226-Unkown archive extension, skipping tests.\r\n");


	check_releasenode(check_node);

	return 2; /* UNKNOWN EXTENSION! */

}



int check_isfree(char *name)
{
  struct check_node *c;

  consolef("check_isfree(): checking file '%s'\n",
	 name);

  for (c = check_node_head; c; c=c->next) {
    if (!lfnmatch(c->ext, name, LFNM_CASEFOLD)) {

      if (c->exe == CHECK_FREE)
	return 1; /* It is free! */
      else
	return 0; /* Not free */

    }
  }

  return 0;
}





//
// FIXME! FIXME! friar
// This function is called once the return value (code, or checksum) is
// known, and it it sets the computed_status to good, bad or unknown.
// The value is a string, and it should set "->computed_value" and compare it
// against "->expected_value". Based on that comparison, it should set
// "->computed_status". Finally, it should call check_action().
//
void check_return(checker_t *node, char *value)
{

	consolef("[check_return]: '%s' value '%s'\n",
			 node->name, value);


	node->computed_value = mystrcpy( value );

	if (!mystrccmp(node->expected_value, node->computed_value))
		node->computed_status = CHECK_GOOD;
	else
		node->computed_status = CHECK_BAD;

	check_action( node );

}



//
// FIXME! FIXME! friar
// This is the last function in the chain of testing. It receives a check node
// which has the status set (good, bad, unknown) and should action on it. This
// means creating any files (-missing, -bad etc), updating any status files
// or work logs, race notes etc etc etc etc.
// Generally called from check_return() above, but can come from the
// input handler (especially if status is unknown).
// We should put in any external hooks here to lion_execv().
// Finally, it releases the node. (any access to node after is a core)
//
void check_action(checker_t *node)
{

	consolef("[check_action] '%s' status %d.\n",
			 node->name, node->computed_status);

	// This code is only here to show what to do.
	switch( node->computed_status ) {

	case CHECK_UNKNOWN: // don't know file integrity, something went wrong
		consolef("[check] UNKNOWN => %s:'%s'\n", node->user, node->disk_name);
		if (section_filecheck(node->dir_name)) {
			sfv_state_create(node->dir_name, node->file_name,
					"-unknown", node->user);
		}

		// Its bad, make it readable again - is this desirable?
		if (server_goodfile) {
			file_goroot();
			chmod(node->disk_name, (mode_t) server_permfile);
			file_gononroot();
		}

		quota_stor_award(node);

		log_xfer("TEST %s unknown %s\n", node->user,node->file_name);
		break;

	case CHECK_GOOD:    // know the file is good.
		consolef("[check] GOOD    => %s:'%s'\n", node->user, node->disk_name);
		sfv_state_update(node->file_name, node->dir_name, SFV_OK);

#ifdef RACESTATS
		if (section_racestats(node->dir_name)) {
			race_file_complete(node->dir_name, node->file_name, node->user,
					 ((struct group *)node->current_group) ?
					 ((struct group *)node->current_group)->name :
							   server_nogroupname,
					  node->bytes, node->timesec, node->timeusec);
		}
#endif

		// Award the credits, if wanted.
		quota_stor_award(node);

		if (section_extractmp3genre(node->dir_name)) {
			if (!lfnmatch("*.mp3", node->file_name, LFNM_CASEFOLD)) {
				create_mp3_info(node->dir_name, node->disk_name);
			}
		}

		// Dupe db
#ifdef DUPECHECK
		if (server_dupecheck && section_dupecheck(node->dir_name)) {
			dupe_add(node->file_name, node->user, node->bytes,global_time_now);
		}
#endif

		log_xfer("TEST %s GOOD %s\n", node->user, node->file_name);

		break;

	case CHECK_BAD:     // know (for sure) the file is bad.
		consolef("[check] BAD     => %s:'%s'\n", node->user, node->disk_name);


		// Delete failed files if requested by config.

        // The issue here is if it once WAS good, and now is bad, can we
        // decrease the counters and mark release incomplete?

		if (section_deletebad(node->dir_name)) {


			log_xfer("TEST %s debug_deletebad %s\n",
					 node->user, node->disk_name);
			consolef("[check] deletebad => %s\n",
					 node->disk_name);


			remove(node->disk_name);

#ifdef RACESTATS
			if (section_racestats(node->dir_name))
				race_file_deleted(node->dir_name, node->file_name);
#endif

#ifdef DUPECHECK
			if (server_dupecheck && section_dupecheck(node->dir_name))
				dupe_del(node->file_name);
#endif

		} else {


			// If its not part of sfv engine, ie, like a .zip or similar, we
			// need to notify it here anyway.
			if (!sfv_state_update(node->file_name, node->dir_name, SFV_FAILED)) {

				sfv_state_create(node->dir_name,
								 node->file_name,
								 "-failed",
								 node->user);


			}

			// Its bad, make it readable again
			if (server_goodfile) {
				file_goroot();
				chmod(node->disk_name, (mode_t) server_permfile);
				file_gononroot();
			}
		}

		log_xfer("TEST %s BAD %s\n", node->user, node->file_name);

		break;
	}



	// HOOK location. Spawn any exeternal hooks for file testing here.
	// We could also make the file read-only.



	// But you should call this as the final line.
	check_releasenode( node );




}








void check_stats(struct login_node *t)
{
	checker_t *runner, *last = NULL;
	unsigned int count;

	for (runner = checker_head, count = 0;
		 runner;
		 runner = runner->next, count++) {
		last = runner;
	}

	lion_printf(t->handle, " There are %u item%s in the check queue\r\n",
				count, count == 1 ? "" : "s");

	if (last)
		lion_printf(t->handle," current test item is '%s' (%s) at %7.2f Mbytes\r\n",
					last->file_name,
					last->user,
					(float)last->bytes / 1048576.0);

}

//
// Check if there is a file with some items to test from when we last shutdown
// so we can resume, and remain coherent. It is a little bit messy since we
// have to look up the user node, assign quota etc etc. Here goes.
//
void check_resume(void)
{
	FILE *ffd;
	char *ar, *user, *size, *ratio, *name;
	struct data_node dummy;

	ffd = fopen(CHECK_SAVE_FILENAME, "r");

	if (!ffd)
		return;

	while(fgets(genbuf, sizeof(genbuf), ffd)) {

		// Parse the input it should be in the format:
		// "username,size,ratio,fulldirectoryname"

		ar = genbuf;

		user = NULL;
		if (*ar != ',') {
			user = misc_digtoken(&ar, ",\r\n");
			if (!user) continue; // parse error?
		}

		size = misc_digtoken(&ar, ", \r\n");
		if (!size) continue; // parse error?

		ratio = misc_digtoken(&ar, ", \r\n");
		if (!ratio) continue; // parse error?

		name = misc_digtoken(&ar, ", \r\n");
		if (!name) continue; // parse error?

		consolef("[check] re-queuing %s,%s,%s,%s\n",
				 user ? user : "",
				 size,
				 ratio,
				 name);

		// if user is set, find login node
		if (user && *user) {

			dummy.login = user_findbyname(user);
			if (dummy.login)
				dummy.login->current_group = group_findcurr_byname(user);

			dummy.login->handle = NULL;

		} else {

			dummy.login = NULL;

		}


		dummy.bytes = (lion64_t)atof(size) * 1048576.0;
		//dummy.ratio = atoi(ratio);
		dummy.name = name;

		//		consolef("[check] login %p, group %p\n", dummy.login,
		//	 dummy.login ? dummy.login->current_group : NULL);

		check_filebad(&dummy);


	}

	fclose(ffd);

	remove(CHECK_SAVE_FILENAME);

}








void create_mp3_info(const char *path, const char *file)
{
	int f, announce = 0;
	char buf[MP3_ID4_SIZE];

	f = open(file, O_RDONLY
#ifdef WIN32
			 |O_BINARY
#endif
			 );

	if (f < 0)
		return;

	lseek(f, -MP3_ID4_SIZE, SEEK_END);
	if (read(f, buf, MP3_ID4_SIZE) < MP3_ID4_SIZE) {
		close(f);
		return;
	}
	close(f);

	// Yuck, harcoded values and offsets
	if ((memcmp(buf, "TAG", 3)) || (unsigned char)buf[127] > (MP3GENRE_NUM-1))
		return;

	// Yuck, harcoded values and offsets
	buf[97] = 0;
	// Yuck, harcoded values and offsets
	snprintf(genbuf, sizeof(genbuf), "%s/__MP3_Year_%s_-_Genre_%s_MP3__",
			 path, buf+93, genre[(unsigned char)buf[127]]);

	// Check if it already exists, if not, we can irc announce it.
	announce = access(genbuf, F_OK);

	// Make it fail if the file already exists, so we don't send
	// multiple irc announcements.
	f = open(genbuf, O_CREAT|O_EXCL|O_RDWR, (mode_t) server_permfile);
	if (f > -1) {
		close(f);
#ifdef IRCBOT
		if (section_announce((char *)path)) {
			irc_announce("MP3GENRE|section=%s|year=%s|genre=%s|dir=%s\n",
						 section_name((char *)path),
						 buf+93, genre[(unsigned char)buf[127]],
						 section_rlsfind((char *)path));
		}
#endif

#if 0
		// We also attempt to write a ".id4" file now.
		snprintf(genbuf, sizeof(genbuf), "%s/.id4",
				 path);

		f = open(genbuf, O_CREAT|O_EXCL|O_RDWR, (mode_t) server_permfile);

		if (f > -1) {
			write(f, buf, sizeof(buf));
			close(f);
		}
#endif

		// We also attempt to write a ".genre" file now.
		snprintf(genbuf, sizeof(genbuf), "%s/.genre",
				 path);

		f = open(genbuf, O_CREAT|O_EXCL|O_RDWR, (mode_t) server_permfile);

		if (f > -1) {
			write(f, genre[(unsigned char)buf[127]],
				  strlen(genre[(unsigned char)buf[127]]));
			close(f);
		}

	}
}








/*
 * New file command paths
 */
void check_newadd(FILE *config)
{
  struct cd_node *c;

  if (!(c = (struct cd_node *) malloc(sizeof(struct cd_node)))) {
    perror("Failed to get memory for new entry");
    exit(-1);
  }

  memset(c, 0, sizeof(*c));

  c->type = CD_NEWPATH;

  c->abbr = NULL;

  c->expa = get_string(config);

  c->next = cd_node_head;
  cd_node_head = c;

}



/* New file scna */

void check_newscan(struct login_node *t, time_t date)
{
  struct cd_node *cd;

  for (cd = cd_node_head; cd; cd = cd->next)

    if (cd->type == CD_NEWPATH) {

      socket_print(t,
"200-+----------+--------------+-----+-------+---------------------------------+\r\n"
"200-| UPLOADER | DATE/TIME    | FLS | SIZE  | %-31s |\r\n"
"200-+----------+--------------+-----+-------+---------------------------------+\r\n", cd->expa);


      file_parsedir(t, cd->expa, date, FILE_NEWSCAN, 0);

      socket_print(t,
"200-+----------+--------------+-----+-------+---------------------------------+\r\n");


    }


}





/* Shadow FTPD indet@IP-pattern matches */




int check_nocapmatch(char *ip)
{

  consolef("nocapmatch(%s)\n", ip);


  if (misc_patt_match_gbl(PATT_NOCAP, ip)) {

	  consolef("No capping IP, matched %s, excluding..\n", ip);
	  return 1;

  }

  return 0;

}

void *check_find_exe(char *pathfile)
{

	struct check_node *c;

    for (c = check_node_head; c; c=c->next) {
        if (!lfnmatch(c->ext, pathfile, LFNM_CASEFOLD))
			return c->exe;
	}

	return NULL;

}







void check_extractimdb(checker_t *c)
{
	char buffer[8192];
	char *ar, *token;
	FILE *fd;
	int lines = 0;

	consolef("check_extractimdb: %s\n", c->disk_name);

	fd = fopen(c->disk_name, "r");

	if (!fd) return;

	while(fgets(buffer, sizeof(buffer), fd)) {

		if (lines++ > 200) break;  // incase they up HUGE nfos

		// Parse out bits.
		for (ar = buffer; (token = misc_digtoken(&ar, " \r\n")); ) {

			if (!lfnmatch("http://*.imdb.com/*", token, LFNM_CASEFOLD)) {

				consolef("matched '%s'\n", token);

				// Announce it.

				// We could also create a file here?
#ifdef IRCBOT
				irc_announce("IMDB|section=%s|dir=%s|URL=%s\n",
							 section_name(c->disk_name),
							 section_rlsfind(c->dir_name),
							 token);
#endif





				lines = 9999;  // break both loops.
				break;

			} // lfnmatch


		} //for digtoken

	} // while

	fclose(fd);

}

