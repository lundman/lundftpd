#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <time.h>
//#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "global.h"
#include "socket.h"
#include "misc.h"
#include "login.h" /* Last */
#include "user.h"
#include "data.h"
#include "file.h"
#include "check.h"
#include "quota.h"
#include "log.h"
#include "groups.h"
#include "global.h"
#include "dbutil.h"
#include "dupe.h"
#include "section.h"
#include "userflag.h"

#include "lion.h"

#ifdef WIN32
#include "win32.h"
#endif



__RCSID("$LundFTPD: quota.c,v 1.28 2003/04/23 08:32:03 lundman Exp $");



#undef DEBUG

/* First we have the OLD routines... */
/* See bottom for new stuff */

static struct quota_node *quota_node_head = NULL; /* In-Memory cache */
static int quota_db_fd = -1;



void quota_logincheck(struct login_node *now, struct login_node *then)
{
	/* First, determin if we've changed day since last login */
	/* Either get old quota's, and the neew daily default */
	struct tm *localtime_now, *localtime_then;

	struct tm tnow, tthen;
	struct quota_node *runn;

	runn = (struct quota_node *) now->quota;

	if (!runn) return;


	localtime_now = NULL;
	localtime_then = NULL;


	/* below are additional check added to check for NULL values */
	localtime_now = localtime(&now->time);
	localtime_then = localtime(&then->time);

	if (localtime_now != NULL)
		bcopy(localtime_now, &tnow, sizeof(tnow));
	else
		consolef("localtime(now) returned NULL ptr!\n");
	if (localtime_then != NULL)
		bcopy(localtime_then, &tthen, sizeof(tthen));
	else
		consolef("localtime(then) returned NULL ptr!\n");
	//  bcopy(localtime(&now->time), &tnow, sizeof(tnow));
	//  bcopy(localtime(&then->time), &tthen, sizeof(tthen));



	if (tnow.tm_yday != tthen.tm_yday) {
		/* We have a new day */
#if 0
		consolef("quota_logincheck(%p): New day, resetting quotas %d:%d:%d\n",
				 now->handle, global_resync_day, global_resync_week,
				 global_resync_month);
#endif

		if (server_userquotadaily >= 0)
			runn->daily = server_userquotadaily*1024;
		if (server_userquotatime >= 0)
			runn->time_limit = server_userquotatime;

		runn->dirty = 1;

	} else {
		/*
		  now->daily = then->daily;
		  now->time_limit = then->time_limit;
		*/
	}
}


void quota_status(struct login_node *t)
{
	struct quota_node *q;

	//	consolef("quota_status(): \n");

	if (!t->quota) return;

	q = (struct quota_node *) t->quota;


	socket_print(t, "230-This is your %d%s login.\r\n",
				 q->login_times, postnumber(q->login_times));


	if (server_userquotadaily < 0)
		socket_print(t, "230-%s has unlimited daily quota, ", t->user);
	else
		socket_print(t, "230-%s has %dKB of daily quota, ", t->user,
					 (int) q->daily/1024);

	if (server_userquotatime < 0)
		socket_print(t, "unlimited time and ");
	else
		socket_print(t, "%d minute%s time left and ",
					 q->time_limit, (q->time_limit == 1 ? "" : "s"));

	if ((server_userquotaratio < 0) || (q->ratio < 0))
		socket_print(t, "unlimited d/l ratio.\r\n");
	else
		socket_print(t, "%uMb download ratio.\r\n",
					 (unsigned int)q->credits / 1024 / 1024);

	socket_print(t,
				 "230-%s has uploaded %2.2f Mbytes and downloaded %2.2f Mbytes.\r\n",

				 t->user,
				 q->bytes_up ?   ((float)q->bytes_up)   / 1024.0 / 1024.0 : 0,
				 q->bytes_down ? ((float)q->bytes_down) / 1024.0 / 1024.0 : 0 );

}




//
// User has attempted to download a new file, check they have enough quota.
//
int quota_retr(struct data_node *d, struct stat *st)
	 /* return 0 to fail open file
	  * and !0 to succeed
	  */
{
	struct quota_node *q;

	if (!server_quotafile ||
		((server_userquotaratio < 0) && (server_userquotadaily < 0)))
		return 1;          /* quota is off */

	if (check_isfree(d->name)) {
		d->type |= DATA_FREE;
		return 5; /* Free extension */
	}

	q = (struct quota_node *) d->login->quota;


	if (!q) return 0; /* No quota node, fail it */


	if ((server_userquotadaily >= 0) &&
		(q->daily >= (unsigned long)st->st_size))
		/* We have enough free download */
		return 2;


	/* Is user's ratio -1 (disabled) ? */
	if (q->ratio < 0)
		return 4;

	/* Does the user have enough bytes to get this file ? */


	if (q->credits >= (lion64_t) st->st_size)
		return 3;/*Do we have enough ratio? */


	/* We don't have enough quota?! */
	socket_print(d->login, "550 You do not have enough quota for file %s size %ld bytes\r\n", path2file(d->name), (unsigned long)st->st_size);

	return 0;
}





//
// User has downloaded a file, or partial, update the number of
// bytes transfered.
//
int quota_retr_close(struct data_node *d)
	 /* return 0 when quota has run out */
{
	struct quota_node *q;
	time_t diff;
	bytes_t bytes = 0;
	float in, out;

	q = (struct quota_node *) d->login->quota;

	// Shouldn't happen but...
	if (!q) return 0;



	// Update the informative-only stats for downloading.

	// If quota is off in .conf, we still need to decrement this counter.
	q->num_active_retr--;

	if (!server_quotafile ||
		((server_userquotaratio < 0) && (server_userquotadaily < 0))) {

		return 1;          /* quota is off */
	}

	diff = lion_get_duration( d->handle );
	lion_get_bytes(d->handle, NULL, &bytes);
	lion_get_cps(d->handle, &in, &out);


	q->dirty = 1;

	//	consolef("[quota] decreased active_retr %d\n", q->num_active_retr);



	if (!bytes) return 0; // No bytes, do no work.

	consolef("[quota] retr_close bytes set to %7.2f megs, duration %d\n",
			 (float) bytes / 1048576.0, diff);



	//	log_xfer("RETR Accident \"/vcd/Shrek2/CD1/clh-shrek2.part02.rar\",\"ISO Games\",\"CLH\",1500000,376.26\n");

	log_xfer("RETR %s %s %s %"PRIu64" %.2f %s\n",
			 d->login->user,
			 section_name(d->name),
			 d->login->current_group ?
			 ((struct group *)d->login->current_group)->name : "(none)",
			 bytes,
			 out,
			 hide_path(d->name));


	// Update all bytes counters, user ..
	q->bytes_down += bytes;
	q->bytes_down_day += bytes;
	q->bytes_down_week += bytes;
	q->bytes_down_month += bytes;


	// .. groups
	if (d->login->current_group) {
		struct group *g = (struct group *)d->login->current_group;
		g->bytes_downloaded += bytes;
		g->bytes_downloaded_day += bytes;
		g->bytes_downloaded_week += bytes;
		g->bytes_downloaded_month += bytes;
	}

	// Update duration
	q->seconds_down_day   += diff;
	q->seconds_down_week  += diff;
	q->seconds_down_month += diff;
	q->seconds_down       += diff;

	// Num files
	q->files_down++;
	q->files_down_day++;
	q->files_down_week++;
	q->files_down_month++;

	// Per this login
	d->login->bytes_down_login += bytes;
	d->login->seconds_down_login += diff;
	d->login->files_down_login++;

	// Global day
	global_dbd += bytes;
	global_dsd += diff;
	global_dfd++;


	q->dirty = 1;


	// If they are disabled, then no credits change.
	if (q->ratio <= 0)
		return 2;




	// File is free, don't do any updating.. we probably should add
	// directory listings here.. could be mean otherwise.
	if (d->type & DATA_FREE)
		return 5; /* Free extension */




	// *** Basically, we have decided now that this file cost the user from
	// credits.


	// If we have daily left, take that off first.

	if (q->daily > 0) {  /* We have some daily quota left */

		if (q->daily >= bytes) { /* we have enough daily for this read */

			q->daily -= bytes;
			return 1;

		} else { /* Not quite enough daily left for this read */

			bytes -= q->daily;
			q->daily = 0;

		}


	}



	// Right. we have "q" and we have "bytes".

	q->credits -= (lion64_t) bytes;


	consolef("[quota] user %s credits is now %7.2f megs.\n",
			 d->login->user,
			 (float)q->credits / 1048576.0);

	return 1;

}




//
// Maybe we should just pass it the checker node?
void quota_stor_award(struct check_filetest_struct *node)
{
	struct quota_node *q;

	//	consolef("[quota] stor_award: %s\n", node->user);

	q = (struct quota_node *)node->quota;

	if (!q)
		return;


	/* DATA_STOR completed award bytes */
	// credits should only be awarded once we know the file is good.
	if (node->ratio > 0) {

		q->credits += (lion64_t) (node->bytes * node->ratio);
		q->dirty = 1;


		consolef("[quota] close_award: Awarding '%s' %7.2f Mbytes\n",
				 node->user,
				 (float) node->bytes / 105876.0 * (float)node->ratio);

	}

}



//
// This updates the upload statistics. BUT NOT THE CREDITS!
// credits is only set once we know that a file tested 100%
//
void quota_stor_close(struct data_node *d)
{
	struct quota_node *q;
	time_t diff;
	bytes_t bytes;
	float in, out;

	//consolef("[quota] stor_close\n");

	/*
	  off = d->login->offset/sizeof(struct login_node) * sizeof(struct quota_node);
	*/

	q = (struct quota_node *)d->login->quota;

	/*
	  for (q = quota_node_head; q; q = q->next)
	  if (q->offset == off) break;
	*/

	if (!q)
		return;

	q->num_active_stor--;
	//	consolef("[quota] decreased active_stor %d\n", q->num_active_stor);


	/* DATA_STOR completed award bytes */

	diff = lion_get_duration( d->handle );
	lion_get_bytes(d->handle, &bytes, NULL);
	lion_get_cps(d->handle, &in, &out);

	//consolef("[quota] stor_close: %7.2f %d\n",
	//	 (float)bytes, diff);


	if (!bytes) return;

	// We probably need to remember this for the file checking, so we can
	// update credits later.
	d->bytes = bytes;


	// Update all bytes counters, user ..
	q->bytes_up        += bytes;
	q->bytes_up_day    += bytes;
	q->bytes_up_week   += bytes;
	q->bytes_up_month  += bytes;


	// .. groups
	if (d->login->current_group) {
		struct group *g = (struct group *)d->login->current_group;
		g->bytes_uploaded += bytes;
		g->bytes_uploaded_day += bytes;
		g->bytes_uploaded_week += bytes;
		g->bytes_uploaded_month += bytes;
	}


	// Update duration
	q->seconds_up       += diff;
	q->seconds_up_day   += diff;
	q->seconds_up_week  += diff;
	q->seconds_up_month += diff;

	// Num files
	q->files_up++;
	q->files_up_day++;
	q->files_up_week++;
	q->files_up_month++;

	// Per this login
	d->login->bytes_up_login   += bytes;
	d->login->seconds_up_login += diff;
	d->login->files_up_login++;

	// Global day
	global_ubd += bytes;
	global_ufd++;
	global_usd += diff;


	q->dirty = 1;

	consolef("quota_close(%p): Updating %s %7.2f Mbytes for file %s "
			 "(%d seconds)\n", d->login, d->login->user,
			 (float) bytes / 105876.0, d->name, diff);

	log_xfer("STOR %s %s %s %"PRIu64" %.2f %s\n",
			 d->login->user,
			 section_name(d->name),
			 d->login->current_group ?
			 ((struct group *)d->login->current_group)->name : "(none)",
			 bytes,
			 in,
			 hide_path(d->name));

	// FIXME!! LION!!
	// We used to spawn the external program that unpacks .nfo and .diz from
	// the .zip files. This needs to be re-coded and indeed, be done both
	// as file type sensitive (always used to spawn unzip ragardless of type)
	// and scriptable. I assume friar's hooks code will tie in nicely.


	/* extract diz is requested. */
#if 0
	if (server_autoextract_diz)
		{
			static char path[256], *r, *file;

			strcpy(path, d->name);
			r = (char *)strrchr(path, '/');

			if (!r)
				sprintf(path, "%s/%s/", server_rootdir, d->login->cwd);
			else
				*r = 0;

			file = path2file(d->name);

#ifndef NOPUTENV
			setenv("DIZ_DIR", path, 1);

			setenv("DIZ_FILE", file, 1);
#else
			sprintf(genbuf, "DIZ_DIR=%s", path);
			putenv(genbuf);

			/* We temp use the path buffer here */
			sprintf(path, "DIZ_FILE=%s", file);
			putenv(path);
#endif

			if (!server_backgroundtest)
				socket_print(d->login, "226-Extracting diz and nfo files...\r\n");

			consolef("Executing '%s'\n", server_autoextract_diz);

			file_gouser(d->login);
			system(server_autoextract_diz);
			/*exec_system(NULL, server_autoextract_diz);*/
			file_gononuser();

		}

#endif // autodiz FIXME LION
}




int quota_timecheck_sub(lion_t *node, void *arg1, void *arg2)
{
	time_t now = (time_t) arg1;
	struct login_node *t;
	//struct quota_node *q;

	if (lion_get_handler(node) != login_handler) return 1; // keep going.

	t = lion_get_userdata( node );
	if (!t) return 1; // keep going.

	// If a remote transfer is going.
	if (t->remote_slave) return 1;


	// Ideally, I'd like to get rid of this test. If we for some reason have
	// dead login nodes, then this would mop it up as they are idle.
	//	if (!(t->status & ST_LOGIN))
	//	return 1; // keep going.


	// Do we still want time quota? does it even work still? From the old
	// BBS days
#if 0
	time_on = (int) ((now-t->time) / 60);

	if (server_userquotatime >= 0) /* Quota is enabled */
		{

			q = (struct quota_node *)t->quota;

			if (!q) continue;

			consolef("quota_timecheck(%d); %s has %d minutes left\n",
					 t, t->user, q->time_limit-time_on);

			if (((int)(q->time_limit-time_on) < 0)) {

				socket_print(t, "421-Your time limit has expired.\r\n");
				socket_number(t, 421, NULL);
				login_exit(t);
				break;
			}
		}
#endif


	// sitecust commands can also take a very long time...
	// do we allow for that?
	if (t->sitecust) return 1;



	// Check if they are idle.
	// No idle set? (race between creating login node, and idle being updated
	// after command sent.
	//	if (!t->idle) return 1; // keep going.

	//	q = (struct quota_node *) t->quota;

	//	if (!q) return 1; // keep going.


	//	consolef("[quota] %s idle %d\n", t->user, now - t->idle);

    // If it doesn't have a socket connection....
    if (!t->handle) return 1;

	if ( (server_useridle > 0)
		 && ((now - t->idle) > server_useridle)
#if 0    // do we kick site admins or not?
		 && !((t->level|server_defaultflags)&UF_ALL)
#endif
		 ) {

		consolef("[timecheck] considering %p/%s as idle, booting.\n",
				 t, t->user ? t->user : "(null)");

		t->logoff_time = lion_global_time;

		log_user("%s idle kicked off %s", t->user, ctime(&t->logoff_time));

		lion_printf(t->handle, "421-Your idle count is reached.\r\n");
		socket_number(t, 421, NULL);
		//lion_close(t->handle); // flushes the handle, and gives us the event.
		login_exit(t);

		return 0; // Step looping (say 'found it' and lion_find returns !NULL)
	}

	return 1; // keep going.
}




void quota_timecheck()
{
	time_t now;

	/* Damnit, this was obviously more work than I thought,
	   I'm splitting it up in two sections, first check
	   if quotalimit is in, check if it has expired. Then
	   check if useridle is on, and see if he has idled out.
	*/

	if (server_useridle <= 0) {

		// If we don't do time quota anymore, we can just exit here.
		return;

	}

	//	consolef("[quota] idle check\n");


	//time(&now);
	now = lion_global_time;

	// This will only idle out one node at a time
	//	lion_find(quota_timecheck_sub, (void *) now, NULL);

	// This will idle out all node at a time
	while(lion_find(quota_timecheck_sub, (void *) now, NULL)) /* empty */ ;

}











/*
 *
 * QUOTA
 *
 */



void quota_update_low(struct quota_node *q)
{
#ifdef DEBUG
	consolef("quota_update_low(): on %ld %d\n", (long int)q->offset, q->dirty);
#endif

	/* Some fail safes*/
	if ((q->offset % sizeof(*q))) {
#ifdef DEBUG
		consolef("INT(F)ERNAL BUG: Rogue offset = %qu\n", q->offset);
#endif
		return;
	}

#if 0
	if (q->dirty != 1)
		return;

	q->dirty = 0;
#endif

	if (lseek(quota_db_fd, q->offset, SEEK_SET) != q->offset) {
		perror("quota_update_low: couldn't lseek()");
		return;
	}

	if (write(quota_db_fd, q, sizeof(struct quota_node)) !=
		sizeof(struct quota_node)) {
		perror("quota_update_low: failed to write?!?? lost info");
		return;
	}

#ifdef PARANOIA1
	lseek(quota_db_fd, 0, SEEK_END); /* Safety */
#endif

}


void quota_addnew(struct login_node *t, int ratio)
{
	struct quota_node runn;
	off_t off;

	off = t->offset / sizeof(struct login_node) * sizeof(struct quota_node);
#ifdef DEBUG
	consolef("Creating at %08lX\n", off);
#endif

	bzero(&runn, sizeof(runn));

	runn.offset = off;
	runn.ratio = ratio;

	quota_update_low(&runn);
}





void quota_update(struct login_node *t)
{
	struct quota_node *runn;

	runn = (struct quota_node *)t->quota;

	if (!runn) return;

	if (runn->dirty) {
		quota_update_low(runn);
		runn->dirty = 0;
	}

	/* Node doesn't exit */

}



void quota_load()
{
	struct stat sb;

	if (!server_quotafile) return; /* No quotas */

	if (access(server_quotafile, F_OK)) {
		consolef("server_quotafile \"%s\" doesn't exist, creating.\n",
				 server_quotafile);

		quota_db_fd = open(server_quotafile, O_RDWR|O_CREAT
#ifdef WIN32
						   |O_BINARY
#endif
						   , 0600);
	} else {
		/*consolef("Opening server_quotafile \"%s\"\n", server_quotafile);*/

		quota_db_fd = open(server_quotafile, O_RDWR
#ifdef WIN32
						   |O_BINARY
#endif
						   );
	}

	if (quota_db_fd < 0) {
		perror("Failed to open server_quotafile");
		exit(1);
	}




	if (!stat(server_quotafile, &sb)) {

		if ((sb.st_size % sizeof(struct quota_node)) == 0) {

			/*	consolef("Correct quotadb version detected.\n");*/

		} else {

			consolef("Incorrect quotadb version!\n");

			db_check_quota((unsigned long)sb.st_size, quota_db_fd);

		}
	}


#ifdef PARANOIA1
	lseek(quota_db_fd, 0, SEEK_END); /* Safety */
#endif

}


void quota_quit()
{

	if (!server_quotafile) return; /* No quotas */

	quota_sync();
	consolef("Closing server_quotafile\n");
	close(quota_db_fd);
	quota_db_fd = -1;
}


void quota_new(struct login_node *t)
{
	struct quota_node *nue, *runn;
	off_t off;

	/* Quota disabled? Return... */
	if (!server_quotafile) return; /* No quotas */

	off = t->offset / sizeof(struct login_node) * sizeof(struct quota_node);


	for (runn = quota_node_head; runn; runn = runn->next)
		if (runn->offset == off) {   /* Already logged in */

			runn->ref_count++;
#ifdef DEBUG
			consolef("Already on, increasing ref to %d\n", runn->ref_count);
#endif

			t->quota = (void *)runn;

			return;
		}

	/* Get a new node, add to linked list, and read for disk */
	if (!(nue = (struct quota_node *) malloc(sizeof(struct quota_node)))) {
		perror("Failed to allocate memory");
		return; /* Wont have a node in memory so all call for quota will fail */
	}


	memset(nue, 0, sizeof(struct quota_node));


	if (lseek(quota_db_fd, off, SEEK_SET) != off) { /* Something went wrong */
		perror("quota_new: failed to lseek() ");
		free(nue);
		return;
	}

	if (read(quota_db_fd, nue, sizeof(struct quota_node)) !=
		sizeof(struct quota_node)) { /* Something went wrong again */
		/*    perror("quota_new: failed to read() ");*/
		fprintf(stderr, "tried to read at off %08lX %ld - returning zero'd struct\n", (long)off, (long)off);

		/*
		  free(nue);
		  return;
		*/
		memset(nue, 0, sizeof(struct quota_node));

		nue->offset = off;

	}

#ifdef PARANOIA1
	lseek(quota_db_fd, 0, SEEK_END); /* Safety */
#endif

#ifdef DEBUG
	consolef("Read old quota data, %ld %ld\n", nue->bytes_up, nue->bytes_down);
#endif

	nue->next = quota_node_head;
	quota_node_head = nue;
	nue->dirty = 0;

	nue->ref_count = 1;

	t->quota = (void *) nue;

	// Zero the active counters, incase we have a bug
	nue->num_active_retr = 0;
	nue->num_active_stor = 0;

}


void quota_sync()
{
	/* Write all node that are dirty to disk */
	struct quota_node *runn;

#if 0
	consolef("quota_sync(): \n");
#endif

	for (runn = quota_node_head; runn; runn = runn->next) {

#ifdef DEBUG
		consolef("      Checking dirty %d, ref %d off %ld\n",
				 runn->dirty, runn->ref_count, (long int)runn->offset);
#endif

		if (runn->dirty) {
			quota_update_low(runn);
			runn->dirty = 0;
		}
	}

#ifdef DUPECHECK
	if (server_dupecheck)
		dupe_sync();
#endif

}


struct quota_node *quota_getquota(struct login_node *t)
{
	struct quota_node *runn;
	off_t off;

	off = t->offset / sizeof(struct login_node) * sizeof(struct quota_node);

	for (runn = quota_node_head; runn; runn = runn->next) {
		if (runn->offset == off) return runn;
	}

	return NULL;
}



void quota_exit(struct login_node *t)
{
	time_t now;
	int time_on;
	off_t off;
	struct quota_node *runn, *prev;

#ifdef DEBUG
	consolef("quota_exit(): \n");
#endif

	/* Right, t is loggin out, remove us */
	if (!server_quotafile) return; /* No quotas */

	/* first we need to find us... */
	off = t->offset / sizeof(struct login_node) * sizeof(struct quota_node);

	for (prev = NULL, runn = quota_node_head;
		 runn;
		 prev = runn, runn = runn->next) {

		if (runn->offset != off) continue;   /* Until we find us */


		if (server_userquotatime >= 0) {
			time(&now);

			time_on = (int) ((now-t->time) / 60);
			runn->time_limit =
				(time_on > runn->time_limit ? 0 : runn->time_limit - time_on);

			consolef("quota_exit(%d): timeon %d, time left %d\n", t,
					 time_on, runn->time_limit);
		}



		if (runn->dirty) {
			quota_update_low(runn);
#ifdef DEBUG
			consolef("Clearing dirty\n");
#endif
			runn->dirty = 0;
		}

		/* Delete node */
#ifdef DEBUG
		consolef("Decreasing ref count from %d\n", runn->ref_count);
#endif
		runn->ref_count--;

		if ((runn->ref_count) > 0) break;  /* More logged on */

#ifdef DEBUG
		consolef("Last ref #, removing from list \n");
#endif

		if (!prev) { /* First in list */
			quota_node_head = runn->next;
			free(runn);
		} else {     /* Middle */
			prev->next = runn->next;
			free(runn);
		}

		break;
	}
}




void quota_top_clear(void)
{
	struct quota_node *runn, q;
	/* Ok, we need to zero out some of the bytes/second/files stats for
	   day (and month) from the quota stuff. */

#if 0
	consolef("quota_top_clear(): Reseting all quotas %d %d %d (%d)\n",
			 global_resync_day, global_resync_week, global_resync_month,
			 global_resync_total);
#endif

	if (global_resync_total) {
		global_total_bytes_up = 0;
		global_total_bytes_down = 0;
	}


	/* First run through in-core nodes and zero them */
	for (runn = quota_node_head; runn; runn = runn->next) {

		runn->dirty = 1;

		if (global_resync_total) {
			global_total_bytes_up += runn->bytes_up;
			global_total_bytes_down += runn->bytes_down;
		}
		if (global_resync_day) {
			runn->bytes_up_day = 0;
			runn->bytes_down_day = 0;
			runn->files_up_day = 0;
			runn->files_down_day = 0;
			runn->seconds_up_day = 0;
			runn->seconds_down_day = 0;
			runn->dirty = 1;
		}
		if (global_resync_week) {
			runn->bytes_up_week = 0;
			runn->bytes_down_week = 0;
			runn->files_up_week = 0;
			runn->files_down_week = 0;
			runn->seconds_up_week = 0;
			runn->seconds_down_week = 0;
			runn->dirty = 1;
		}
		if (global_resync_month) {
			runn->bytes_up_month = 0;
			runn->bytes_down_month = 0;
			runn->files_up_month = 0;
			runn->files_down_month = 0;
			runn->seconds_up_month = 0;
			runn->seconds_down_month = 0;
			runn->dirty = 1;
		}

	}

	/* Now, run through all on-disk nodes */
	if (lseek(quota_db_fd, 0 , SEEK_SET) != 0) {
		perror("quota_top_clear: couldn't lseek()");
		return;
	}

	while (read(quota_db_fd, &q, sizeof(struct quota_node)) ==
		   sizeof(struct quota_node)) {

		if (global_resync_total) {
			global_total_bytes_up += q.bytes_up;
			global_total_bytes_down += q.bytes_down;
		}
		if (global_resync_day) {
			q.bytes_up_day = 0;
			q.bytes_down_day = 0;
			q.files_up_day = 0;
			q.files_down_day = 0;
			q.seconds_up_day = 0;
			q.seconds_down_day = 0;
			q.dirty = 1;
		}
		if (global_resync_week) {
			q.bytes_up_week = 0;
			q.bytes_down_week = 0;
			q.files_up_week = 0;
			q.files_down_week = 0;
			q.seconds_up_week = 0;
			q.seconds_down_week = 0;
			q.dirty = 1;
		}
		if (global_resync_month) {
			q.bytes_up_month = 0;
			q.bytes_down_month = 0;
			q.files_up_month = 0;
			q.files_down_month = 0;
			q.seconds_up_month = 0;
			q.seconds_down_month = 0;
			q.dirty = 1;
		}

		/* This changes the seek, so we have to read-seek */
		quota_update_low(&q);

		if (lseek(quota_db_fd, q.offset + sizeof(q), SEEK_SET) !=
			q.offset + sizeof(q)) {
			perror("quota_top_clear: couldn't lseek() 2");
			break;
		}

	} /* while */

#ifdef PARANOIA1
	lseek(quota_db_fd, 0, SEEK_END); /* Safety */
#endif


	/* If a new day, copy #1 TOP stats into yesterdays variables */
	if (global_resync_day) {

		strcpy((char *)global_yuud  , (char *)global_tuud[0]) ; /* Username */
		strcpy((char *)global_yugd  , (char *)global_tugd[0]) ; /* Group */
		global_yubd         = global_tubd[0]  ; /* Bytes */
		global_yufd         = global_tufd[0]  ; /* Files */
		global_yusd         = global_tusd[0]  ; /* Seconds */
		strcpy((char *)global_yutd  , (char *)global_tutd[0]) ; /* Tagline */

		strcpy((char *)global_ydud  , (char *)global_tdud[0]) ; /* Username */
		strcpy((char *)global_ydgd  , (char *)global_tdgd[0]) ; /* Group */
		global_ydbd         = global_tdbd[0]  ; /* Bytes */
		global_ydfd         = global_tdfd[0]  ; /* Files */
		global_ydsd         = global_tdsd[0]  ; /* Seconds */
		strcpy((char *)global_ydtd  , (char *)global_tdtd[0]) ; /* Tagline */

		// Reset NEWDAY stats for irc
		global_ubd = global_dbd = 0;
		global_ufd = global_dfd = 0;
		global_usd = global_dsd = 0;

	}

	/* If a new week, copy #1 TOP stats into last-weeks variables */
	if (global_resync_week) {


		strcpy((char *)global_yuuw  , (char *)global_tuuw[0]) ; /* Username */
		strcpy((char *)global_yugw  , (char *)global_tugw[0]) ; /* Group */
		global_yubw         = global_tubw[0]  ; /* Bytes */
		global_yufw         = global_tufw[0]  ; /* Files */
		global_yusw         = global_tusw[0]  ; /* Seconds */
		strcpy((char *)global_yutw  , (char *)global_tutw[0]) ; /* Tagline */

		strcpy((char *)global_yduw  , (char *)global_tduw[0]) ; /* Username */
		strcpy((char *)global_ydgw  , (char *)global_tdgw[0]) ; /* Group */
		global_ydbw         = global_tdbw[0]  ; /* Bytes */
		global_ydfw         = global_tdfw[0]  ; /* Files */
		global_ydsw         = global_tdsw[0]  ; /* Seconds */
		strcpy((char *)global_ydtw  , (char *)global_tdtw[0]) ; /* Tagline */

	}

	/* If a new month, copy #1 TOP stats into last-months variables */
	if (global_resync_month) {

		strcpy((char *)global_yuum  , (char *)global_tuum[0]) ; /* Username */
		strcpy((char *)global_yugm  , (char *)global_tugm[0]) ; /* Group */
		global_yubm         = global_tubm[0]  ; /* Bytes */
		global_yufm         = global_tufm[0]  ; /* Files */
		global_yusm         = global_tusm[0]  ; /* Seconds */
		strcpy((char *)global_yutm  , (char *)global_tutm[0]) ; /* Tagline */

		strcpy((char *)global_ydum  , (char *)global_tdum[0]) ; /* Username */
		strcpy((char *)global_ydgm  , (char *)global_tdgm[0]) ; /* Group */
		global_ydbm         = global_tdbm[0]  ; /* Bytes */
		global_ydfm         = global_tdfm[0]  ; /* Files */
		global_ydsm         = global_tdsm[0]  ; /* Seconds */
		strcpy((char *)global_ydtm  , (char *)global_tdtm[0]) ; /* Tagline */

	}

}





