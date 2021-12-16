
// If crypt() cores on Ubuntu, it is usually because _XOPEN_SOURCE needs to be
// set.
//#define _XOPEN_SOURCE
#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __USE_XOPEN
extern char *crypt (__const char *__key, __const char *__salt);
#endif

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#include <pwd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>


#ifdef DEC_ES
#include <sys/security.h>
#include <prot.h>
#endif


#include "global.h"
#include "misc.h"
#include "login.h"
#include "socket.h"
#include "user.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "fnmatch.h"
#include "file.h"
#include "userflag.h"
#include "log.h"
#include "local.h"
#include "groups.h"

#include "global.h"

#include "dbutil.h"

#ifdef WIN32
#include "win32.h"
#endif



__RCSID("$LundFTPD: user.c,v 1.28 2003/04/23 08:32:03 lundman Exp $");




static int user_db_fd = -1;

int max_open_files_safe = 0;


void user_load()
{
	struct stat sb;

	if (access(server_userfile, F_OK)) {
		consolef("server_userfile \"%s\" doesn't exist, creating.\n",
				 server_userfile);

		user_db_fd = open(server_userfile, O_RDWR|O_CREAT
#ifdef WIN32
						  |O_BINARY
#endif
						  , 0600);
	} else {
		/*consolef("Opening server_userfile \"%s\"\n", server_userfile);*/

		user_db_fd = open(server_userfile, O_RDWR
#ifdef WIN32
						  |O_BINARY
#endif
						  );

		if (!stat(server_userfile, &sb)) {

			if ((sb.st_size % sizeof(struct login_node)) == 0) {

				/*	consolef("Correct userdb version detected.\n");*/

			} else {

				consolef("Incorrect userdb version!\n");

				db_check_login((unsigned long)sb.st_size, user_db_fd);

			}
		}
	}

	if (user_db_fd < 0) {
		perror("Failed to open server_userfile");
		exit(1);
	}

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

}







void user_close()
{
	consolef("Closing server_userfile\n");
	close(user_db_fd);
	user_db_fd = -1;
}




int user_addnew(struct login_node *t)
{
	struct login_node tmp;

	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return 0;
	}

	while(read(user_db_fd, &tmp, sizeof(tmp)) == sizeof(tmp)) {
		if (!*tmp.user) {
			consolef("Found previously deleted user, adding %s\n", t->user);

			if ((t->offset = lseek(user_db_fd, -((off_t)sizeof(tmp))
								   , SEEK_CUR)) < 0) {
				perror("lseek(-record)");
				return 0;
			}

			if (write(user_db_fd, t, sizeof(tmp)) != sizeof(tmp)) {
				perror("write(new_user)");
				return 0;
			}

#ifdef PARANOIA1
			lseek(user_db_fd, 0, SEEK_END);
#endif

			return 1; /* Success */
		}
	}

	if ((t->offset = lseek(user_db_fd, 0L, SEEK_END)) < 0) {
		perror("lseek(end)");
		return 0;
	}

	if (write(user_db_fd, t, sizeof(tmp)) != sizeof(tmp)) {
		perror("write(append)");
		return 0;
	}

	consolef("User %s appended.\n", t->user);

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

	return 1;
}

struct login_node *user_findbyname(char *name)
{
	static struct login_node buf;

	if (lseek(user_db_fd, 0, SEEK_SET) < 0) {
		perror("user_findbyname(lseek(rewind))");
		return NULL;
	}

	while(read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf))
		if (!mystrccmp(buf.user, name)) {
#ifdef PARANOIA1
			lseek(user_db_fd, 0, SEEK_END);
#endif

			/* Need to clear some invalid fields here! Mostly paranoia but not all */
			buf.cwd = NULL;
			buf.data = NULL;
			buf.next = NULL;
			buf.buffer = NULL;

			buf.current_group = NULL;
			buf.auth = NULL;
			buf.quota = NULL;
			buf.tagline[MAXTAGLINE - 1] = 0;

			return &buf;
		}

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

	return NULL;
}





struct login_node *user_dumpall(int reset)
{
	static struct login_node buf;

	if (reset) {
		if (lseek(user_db_fd, 0, SEEK_SET) < 0) {
			perror("user_dumpall(lseek(rewind))");
			return NULL;
		}
	}

	if (read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf)) {

		/* Need to clear some invalid fields here! Mostly paranoia but not all */
		buf.cwd = NULL;
		buf.data = NULL;
		buf.next = NULL;
		buf.buffer = NULL;

		buf.current_group = NULL;
		buf.auth = NULL;
		buf.quota = NULL;
		buf.tagline[MAXTAGLINE - 1] = 0;

		return &buf;
	}

	return NULL;
}





struct login_node *user_findbyuid(int uid)
{
	static struct login_node buf;

	if (uid < server_useridstart) return NULL;

	/* If your nobody etc is 65535... */
	if (uid >= 65535) return NULL;


	if (lseek(user_db_fd, (uid - server_useridstart) * sizeof(buf), SEEK_SET) < 0) {
		perror("user_findbyuid(lseek(rewind))");
		return NULL;
	}

	if ((read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf)) &&
		buf.user[0]) {
#ifdef PARANOIA1
		lseek(user_db_fd, 0, SEEK_END);
#endif
		return &buf;
	}

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

	return NULL;
}

void user_update(struct login_node *t)
{
	consolef("User_update at %ld\n", (unsigned long) t->offset);

	if ((t->offset % sizeof(struct login_node)) != 0) {
		consolef("INCORRECT OFFSET for user %s!!!\n", t->user);
		return;
	}

	if (((off_t)lseek(user_db_fd,(off_t) t->offset, SEEK_SET)) == -1) {
		perror("user_update(lseek(offset))");
		return;
	}

	if (write(user_db_fd, t, sizeof(struct login_node)) !=
		sizeof(struct login_node)) {
		perror("user_update(write())");
		return;
	}

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

}

int user_pass(struct login_node *t, char *pass)
{
	struct login_node *lookup;
	int i;

	/* Search for t->user in user_db */



	if (!(lookup = user_findbyname(t->user))) {
		/* We couldn't actually find such an user */
		log_user("FAILED LOGIN on %s from %s:%d ident %s: no such user.\n",
				 t->user, lion_ntoa(t->host), t->port, t->ident);
		return 0;
	}


	/* Now check first if this is meant to be anonymous, if so, don't crypt */

	if (!strcmp(t->user, "ftp")) {
		if (server_anonymous) {

			// Copy over the supplied password
			strncpy(lookup->pass, pass, MAXUSERNAME);
			lookup->pass[MAXUSERNAME-1] = 0;

			// If its anonymous let it drop down, so we load the node from disk
			// set up quota node etc.
			//return 1;

		} else return 0;
	}





	if (!server_userfile) {

#ifndef WIN32
#ifndef DEC_ES
		/* Fetch password from /etc/passwd (needs root if shadow) */
		struct passwd *pb;

		pb = getpwnam(t->user);
		if (!pb)  /* No such user, fail */
			return 0;

		strcpy(lookup->pass, pb->pw_passwd);

#else
		/* DEC's enhanced security */
		struct es_passwd *pb;

		file_goroot();

		pb = getespwnam(t->user);

		file_gononroot();

		if (!pb) {  /* No such user, fail */
			return 0;
		}

		strcpy(lookup->pass, pb->ufld->fd_encrypt);

#endif
#endif

	}


	// If it isnt an anonymous account, crypt the pass
	if (strcmp(lookup->user, "ftp"))
		user_crypt(pass, lookup->pass);

	//consolef("pass test '%s' '%s'\n", lookup->pass, pass);

	// Compare the password, note the anonymous will work as it copied above.
	if (strncmp(lookup->pass, pass, MAXPASSNAME)) {
		log_user("FAILED LOGIN on %s from %s:%d ident %s: incorrect password.\n",
				 t->user, lion_ntoa(t->host), t->port, t->ident);

		return 0; /* Wrong passwd matey */
	}


	if (!server_userfile) /* Zero passwd if it's from /etc/passwd */
		memset(lookup->pass, 0, sizeof(lookup->pass));



	if (!t->ident[0]) {
		consolef("Haven't received ident yet!\n");
	}


	if (server_usermatch) { /* IP matching is enabled */

		/* First, did we receive a shadowftpd AUTH command? */
		if ((t->status & ST_AUTHOK) && t->auth) { /* AUTH command received */

			/* If they are allowed to connect from sftpd..*/
			char *r;

			if ((r = strchr(t->auth, '@'))) { /* Find the @ */

				*r = 0;
				strncpy(t->ident, t->auth, MAXUSERNAME);
				t->ident[MAXUSERNAME+1] = 0;

				t->host = lion_addr(&r[1]);

			}


			free(t->auth);
			t->auth = NULL;
		}


		if (!user_ipmatch(lookup,t)) {

			/*
			 * If we failed IP-match lookup, check if ShadowFTPD lookup is enabled
			 * if so, check if it matches that. If so, wait until we get the
			 * AUTH line, THEN do the real check.
			 */


			log_user("FAILED LOGIN on %s from %s:%d ident %s: no IP pattern.\n",
					 t->user, lion_ntoa(t->host), t->port, t->ident);
			return 0;

		}

	}

	/* Make sure we don't run out of filed's */
#if 0 // FIXME LION
	if (t->socket > max_open_files_safe) {
		socket_print(t, "202-Too many users already logged on, please try again later.\r\n");
		return 0;
	}
#endif


	bcopy(lookup->pass, t->pass, sizeof(lookup->pass));


	/*
	 * Now, copy everything we want to keep from lookup to t
	 */

	t->level = lookup->level;

	if (t->level == UF_OLDALL)
		t->level = UF_ALL;

	t->offset =lookup->offset;
	strcpy(t->user, lookup->user);
	t->options = lookup->options;

	strcpy(t->tagline, lookup->tagline);
	strcpy(t->last_ident, lookup->ident);

	t->last_host = lookup->host;
	t->last_login = lookup->time;
	t->last_logout = lookup->logoff_time;
	t->num_logins = lookup->num_logins;
	t->num_uploads = lookup->num_uploads;
	t->num_downloads = lookup->num_downloads;

	/* Don't copy srp herre */

	for (i=0; i<IPMATCHMAX; i++)
		bcopy(lookup->matchip[i], t->matchip[i], IPMATCHLEN);


	/* proceed with login */


	t->idle = lion_global_time;
	t->time = lion_global_time;

	/* Register our quota node into login node. */
	quota_new(t);



	if (server_quotafile)
		quota_logincheck(t, lookup); /* This will check is daily should be updated
									  * and daily time limit.*/

#if 0
	if (server_showlastlogin) {

		local_login(t);


		if (!lookup->host || !lookup->port || !lookup->time)
			socket_print(t, "230-Welcome, this is your first login\r\n");
		else
			socket_print(t,
						 "230-Your last login from %s@%s:%d was at %s",
						 lookup->ident, lion_ntoa(lookup->host), lookup->port,
						 ctime(&lookup->time));

		if (server_quotafile) {

			socket_print(t, "230-Daily quota is %s, ratio is %s and user time limit is %s\r\n",
						 config_quotadailya(),config_quotaratioa(),
						 config_quotatimea());

			quota_status(t);
		}

		socket_print(t, "230-Concurrent data sessions is set to max %d\r\n",
					 server_xferlimit);

		if (t->level) socket_print(t, "230-You has special access %s\r\n",
								   flagstoa(t->level));

	}
#endif


	return 1; /* Success */
}

void user_crypt(char *line, char *salts)
{
	char salt[9], *ret;

	if (!salts) {
		(void)srandom((int)time((time_t *)NULL));
		to64(&salt[0], random(), 2);
	}

	if ((ret = crypt(line, (salts?salts:salt)))) {

		strncpy(line, ret, MAXPASSNAME);
		line[MAXPASSNAME] = (char) 0;

	} else {

		perror("crypt()");

	}
}




void user_sitelist(struct login_node *t)
{
	struct login_node tmp;
	struct quota_node *q;
	int sep = 0;


	// seek to the start
	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return ;
	}

//".--[ USERS ]----------------------------------------------------------------."
//"| User: friar       Groups: ABC EFGH iJKL     Ratio: leech   Logins: 10     |"
//"| Credits: 13919.4 MB    Flags: UF_ALL                                      |"
//"|  -----------------------------------------------------------------------  |"
//"| User: Sond        Groups: None              Ratio: 1:3     Logins: 1      |"
//"| Credits: 0.4 MB        Flags: UF_ALL                                      |"
//"|  -----------------------------------------------------------------------  |"
//"| User: Sond        Groups: None              Ratio: 1:3     Logins: 1      |"
//"| Credits: 0.4 MB        Flags: UF_ALL                                      |"
//"`---------------------------------------------------------------------------'"

	socket_print(t, "200-[ USERS BEGIN ]\r\n");
	socket_print(t,
		".--[ USERS ]----------------------------------------------------------------.\r\n");

	while(read(user_db_fd, &tmp, sizeof(tmp)) == sizeof(tmp)) {

		// Deleted user
		if (!tmp.user[0])
			continue;

		if (sep)
			socket_print(t, "|  -----------------------------------------------------------------------  |\r\n");

		quota_new(&tmp);
		q = (struct quota_node *)tmp.quota;
		socket_print(t, "| User: %-14.14s Groups: %-19.19s Ratio: %-6.6s Logins: %02d |\r\n",
			tmp.user, group_listgroups(tmp.user), misc_format_ratio(q->ratio),
			tmp.num_logins);
		socket_print(t, "| Credits: %- 10.2f  Flags: %-45.45s |\r\n",
			q->credits / 1048576.0, flagtowhat(tmp.level));

		quota_exit(&tmp);

		sep = 1;

	}

	socket_print(t,
		"`---------------------------------------------------------------------------'\r\n");
	socket_print(t, "200 [ USERS END ]\r\n");

#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif

}



/*
 *
 * Argument functions
 *
 */

void user_listuser()
{
	struct login_node tmp;
	char buf[WORDLEN];
	struct tm *now;
	struct quota_node zero_quota, *quota;

	bzero(&zero_quota, sizeof(zero_quota));
	bzero(&tmp, sizeof(tmp));

	consolef("Enter usercode pattern (* for all): "); fflush(stdout);
	if (!get_word(stdin, buf)) exit(1);

	/* seek to the start */
	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return ;
	}

	consolef("Usercode     Level       Up.GB   Down.GB Ratio     Last Login           yy/mm/dd\n");

	while(read(user_db_fd, &tmp, sizeof(tmp)) == sizeof(tmp)) {
		/* Now, does it match the name/pattern? */

		if (!tmp.user[0]) continue; /* Deleted users */

		if (!fnmatch(buf, tmp.user, FNM_CASEFOLD)) {

			//now = localtime(&tmp.time);

			now = localtime(&tmp.logoff_time);

			quota_new(&tmp);

			if (!(quota = quota_getquota(&tmp))) {
				consolef("Not found.\n");
				quota = &zero_quota;
			}

			consolef("%-11s %08X %9.2f %9.2f %5.5s %8s@%-15s %02d/%02d/%02d\n",
					 tmp.user,
					 tmp.level,
					 (float)quota->bytes_up / 1073741824.0,
					 (float)quota->bytes_down / 1073741824.0,
					 misc_format_ratio(quota->ratio),
					 tmp.ident, lion_ntoa(tmp.host),
					 now->tm_year >= 100 ? now->tm_year - 100 : now->tm_year,
					 now->tm_mon+1, now->tm_mday+1);

			quota_exit(&tmp);
		}
	}
	consolef("\nThat's all.\n");
}


void user_listuser_long()
{
	struct login_node tmp;
	char buf[WORDLEN];
	struct tm *now;
	struct quota_node zero_quota, *quota;

	bzero(&zero_quota, sizeof(zero_quota));
	bzero(&tmp, sizeof(tmp));

	consolef("Enter usercode pattern (* for all): "); fflush(stdout);
	if (!get_word(stdin, buf)) exit(1);

	/* seek to the start */
	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return ;
	}


	consolef("Not really implemented, what other info would you like?\n");

	consolef("Usercode     Level       Up.GB   Down.GB Ratio     Last Login           yy/mm/dd\n");

	while(read(user_db_fd, &tmp, sizeof(tmp)) == sizeof(tmp)) {
		/* Now, does it match the name/pattern? */

		if (!tmp.user[0]) continue; /* Deleted users */

		if (!fnmatch(buf, tmp.user, FNM_CASEFOLD)) {
			now = localtime(&tmp.time);

			quota_new(&tmp);

			if (!(quota = quota_getquota(&tmp))) {
				consolef("Not found.\n");
				quota = &zero_quota;
			}

			consolef("%-11s %08X %9.2f %9.2f %5.5s %8s@%-15s %02d/%02d/%02d\n",
					 tmp.user,
					 tmp.level,
					 (float)quota->bytes_up / 1073741824.0,
					 (float)quota->bytes_down / 1073741824.0,
					 misc_format_ratio(quota->ratio),
					 tmp.ident, lion_ntoa(tmp.host),
					 now->tm_year >= 100 ? now->tm_year - 100 : now->tm_year,
					 now->tm_mon+1, now->tm_mday+1);

			quota_exit(&tmp);
		}
	}
	consolef("\nThat's all.\n");
}



void user_list_top(void)
{
	struct login_node tmp;
	char buf[WORDLEN];
	struct tm *now;
	struct quota_node zero_quota, *quota;

	bzero(&zero_quota, sizeof(zero_quota));
	bzero(&tmp, sizeof(tmp));

	consolef("Enter usercode pattern (* for all): "); fflush(stdout);
	if (!get_word(stdin, buf)) exit(1);

	/* seek to the start */
	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return ;
	}

	consolef("Usercode f-up-all f-up-mth f-up-wek f-up-day f-dn-all f-dn-mth f-dn-wek f-dn-day\n");

	while(read(user_db_fd, &tmp, sizeof(tmp)) == sizeof(tmp)) {
		/* Now, does it match the name/pattern? */

		if (!tmp.user[0]) continue; /* Deleted users */

		if (!fnmatch(buf, tmp.user, FNM_CASEFOLD)) {
			now = localtime(&tmp.time);

			quota_new(&tmp);

			if (!(quota = quota_getquota(&tmp))) {
				consolef("Not found.\n");
				quota = &zero_quota;
			}

			consolef("%-9.9s%8d %8d %8d %8d %8d %8d %8d %8d\n",
					 tmp.user,
					 quota->files_up,
					 quota->files_up_month,
					 quota->files_up_week,
					 quota->files_up_day,
					 quota->files_down,
					 quota->files_down_month,
					 quota->files_down_week,
					 quota->files_down_day);

			quota_exit(&tmp);
		}
	}
	consolef("\nThat's all.\n");
}




void user_adduser()
{
	struct login_node neww;
	char buf[WORDLEN];
	int i;

	while(1) {
		bzero(&neww, sizeof(neww));

		consolef("Enter ftpd usercode: "); fflush(stdout);
		if (!get_word(stdin, buf)) exit(1);
		strncpy(neww.user, buf, MAXUSERNAME);
		neww.user[MAXUSERNAME] = 0;

		if (user_findbyname(neww.user)) {
			consolef("User already exists!\n");
			continue;
		}

		if (server_userfile) {
			consolef("Enter %s'%s passwd: ", neww.user,
					 (neww.user[strlen(neww.user)-1] == 's'?"":"s"));
			fflush(stdout);

			if (!get_word(stdin, buf)) exit(1);

			if (buf[0] == 1) {

				strncpy(neww.pass, &buf[1], MAXPASSNAME);
				neww.pass[MAXPASSNAME] = 0;

			} else {

				strncpy(neww.pass, buf, MAXPASSNAME);
				neww.pass[MAXPASSNAME] = 0;

				user_crypt(neww.pass, NULL);

			}


			consolef("%s\n", neww.pass);

		} else {
			consolef("Using passwd stored in /etc/passwd\n");
		}

		consolef("Enter special priviledges level, normal users and anonymous ftp user should be\nset to level 0 (default), and perhaps UF_ALL for admin.\n");
		consolef("Enter special access flags for user other than those offered in default flags.\n");
		consolef("Enter as FLAG1|FLAG2..., use ~ to remove a flag. e.g. UF_KICK|~UF_STAT\n");
		consolef("Flags available are: UF_NONE, UF_OVERWRITE, UF_DELETE, UF_DIRDELETE, UF_STAT\n");
		consolef("                   : UF_PASSWORD, UF_RESTART, UF_PUNISH, UF_MOVE,\n");
		consolef("                   : UF_KICK, UF_DUPE, UF_CREDITS, UF_ADDUSER,UF_SETIP,\n");
		consolef("                   : UF_SETRATIO, UF_SETLIMT, UF_SETFLAGS, UF_SEEALL, or just award\n");
		consolef("                   : everything with UF_ALL\n");
		consolef("Default flags are: %s\n", flagstoa(server_defaultflags));
		consolef("Use UF_NONE or 0 for normal users.\n");

		consolef("Enter ftpd level: "); fflush(stdout);
		neww.level = 0;

		if (!get_word(stdin, buf)) exit(1);

		if (*buf == '0') neww.level = 0;
		else
			neww.level = atoflags(buf);




		if (server_usermatch) {
			consolef("\nPlease specify up to %d IP match patterns for user in the format\n"

					 , IPMATCHMAX);
			consolef("ident@IP.IP.IP.IP  For example, smith@202.50.198.*\n");
			consolef("If ident lookups are not required or prefered, use *@202.50.198.*\n");
			consolef("Finish by entering a single 'q' and pressing return.\n");

			putchar('|'); for (i=0; i<IPMATCHLEN-2; i++) putchar('-');
			putchar('|');putchar('\n');

			for (i=0; i < IPMATCHMAX; i++) {

				get_word(stdin, buf);
				if (*buf == '\n' || *buf == '\r' || !*buf || *buf=='q' ||
					*buf=='Q') break;
				strncpy(neww.matchip[i], buf, IPMATCHLEN);
				neww.matchip[i][IPMATCHLEN-1] = (char) 0;

			}
			consolef("\n");
		}

		if (server_quotafile) {
			consolef("Enter ratio: "); fflush(stdout);

			i = get_int(stdin);
			/*    neww.ratio = i;*/


		} else i = 0;


		neww.num_logins    = server_loginlimit;
		neww.num_uploads   = server_uploadlimit;
		neww.num_downloads = server_downloadlimit;



		if (!user_addnew(&neww)) {
			consolef("Failed to add user %s, exit.\n", neww.user);
		} else
			quota_addnew(&neww, i);

		log_admin("%s (%d/%d) created account '%s'\n", getlogin(),
				  getuid(), geteuid(),
				  neww.user);

		consolef("\n\nAdd another? (Y/N): "); fflush(stdout);

		do {
			get_word(stdin, buf);
		} while(*buf != 'y' && *buf != 'Y' && *buf != 'n' && *buf != 'N');

		if (*buf == 'n' || *buf == 'N') {
			break;
		}
	}
}





void user_dump_adduser(void)
{
	struct login_node neww;
	int i, firsttime = 1;

	/* seek to the start */
	if (lseek(user_db_fd, 0L, SEEK_SET) != 0) {
		perror("lseek(rewind)");
		return ;
	}


	while(read(user_db_fd, &neww, sizeof(neww)) == sizeof(neww)) {
		/* Now, does it match the name/pattern? */


		if (!*neww.user) continue;/* deleted entry */

		if (firsttime)
			firsttime = 0;
		else
			consolef("y\n");

		consolef("%s\n", neww.user);

		if (server_userfile) {

			consolef("%c%s\n", (char) 1, neww.pass);

		}


		consolef("%s\n", flagstoa(neww.level));

		if (server_usermatch) {
			for (i=0; i < IPMATCHMAX; i++) {
				if (neww.matchip[i][0]) consolef("%s\n", neww.matchip[i]);
				else break;
			}

			if (i != IPMATCHMAX) consolef("q\n");
		}

		if (server_quotafile) {
			struct quota_node *quota;

			quota_new(&neww);

			if (!(quota = quota_getquota(&neww))) {
				consolef("3\n");
			} else {
				consolef("%d\n", quota->ratio);
			}
		}
	}

	consolef("n\n");

}


void user_deleteuser()
{
	struct login_node *neww;
	char buf[WORDLEN];
	struct group *g;
	struct group_member *gm;

	consolef("Enter ftpd usercode: "); fflush(stdout);
	if (!get_word(stdin, buf)) exit(1);

	neww = user_findbyname(buf);

	if (!neww) {
		consolef("No such user.\n");
		return;
	}

	consolef("Are you you want to delete user %s? (Yes/No): ", buf);
	fflush(stdout);

	if (!get_word(stdin, buf)) exit(1);

	if (!mystrccmp(buf, "yes")) {

		log_admin("%s (%d/%d) deleted account '%s'\n", getlogin(),
				 getuid(), geteuid(),
				 neww->user);

		while((g = group_findany_byname(neww->user)) &&
			  (gm = group_hasuser(g, neww->user)))
			group_member_del(g, gm);
		group_save();


		neww->user[0] = (char) 0;
		user_update(neww);
		consolef("Deleted.\n");

	} else consolef("Nevermind then.\n");
}


void user_edituser()
{
	struct login_node *neww, new2;
	char buf[1024];
	char tnam[256] = "/tmp/tmp.XXXXXXXX";
	FILE *editfile = NULL;
	struct quota_node *quota, zero_quota;
	int f;

	bzero(&zero_quota, sizeof(zero_quota));

	consolef("Enter ftpd usercode: "); fflush(stdout);
	if (!get_word(stdin, buf)) exit(1);

	neww = user_findbyname(buf);

	if (!neww) {
		consolef("No such user.\n");
		return;
	}

	bcopy(neww, &new2, sizeof(new2));

	if (!getenv("EDITOR")) {
		consolef("You have not set your EDITOR environment variable to your favourite editor!\n");
#ifdef WIN32
		consolef("In windows, you can use NOTEPAD.EXE\n");
#endif

		return;
	}



#ifdef WIN32
	sprintf(tnam, "tmpedit.txt");
#else
	f = mkstemp(tnam);
	editfile = fdopen(f, "w+");
#endif


	if (!editfile) {
		if (!tnam || !(editfile = fopen(tnam, "w+"))) {
			perror("Can't open temporary file");
			return;
		}
	}

	quota_new(neww);
	if (!(quota = quota_getquota(neww)))
		quota = &zero_quota;

	fprintf(editfile, "Username             : %s\n", neww->user);
	fprintf(editfile, "Password             : %s\n", neww->pass);
	fprintf(editfile, "Level                : %s\n", flagstoa(neww->level));

	if (server_quotafile) {
		if (server_userquotadaily >= 0)
			fprintf(editfile, "QuotaDaily           : %ld\n", quota->daily);
		if (server_userquotaratio >= 0)
			fprintf(editfile, "QuotaRatio           : %d\n", quota->ratio);
		if (server_userquotatime >= 0)
			fprintf(editfile, "QuotaTime            : %d\n", quota->time_limit);
	}

#if defined ( IRIX ) || ( HPUX ) || ( WIN32 )

	fprintf(editfile, "Credits              : %llu\n", quota->credits);

	fprintf(editfile, "Bytes_Up_All         : %llu\n", quota->bytes_up);
	fprintf(editfile, "Bytes_Down_All       : %llu\n", quota->bytes_down);

	fprintf(editfile, "Bytes_Up_Month       : %llu\n", quota->bytes_up_month);
	fprintf(editfile, "Bytes_Down_Month     : %llu\n", quota->bytes_down_month);

	fprintf(editfile, "Bytes_Up_Week        : %llu\n", quota->bytes_up_week);
	fprintf(editfile, "Bytes_Down_Week      : %llu\n", quota->bytes_down_week);

	fprintf(editfile, "Bytes_Up_Day         : %llu\n", quota->bytes_up_day);
	fprintf(editfile, "Bytes_Down_Day       : %llu\n", quota->bytes_down_day);

#elif defined ( DEC )

	fprintf(editfile, "Credits              : %lu\n", quota->credits);

	fprintf(editfile, "Bytes_Up_All         : %lu\n", quota->bytes_up);
	fprintf(editfile, "Bytes_Down_All       : %lu\n", quota->bytes_down);

	fprintf(editfile, "Bytes_Up_Month       : %lu\n", quota->bytes_up_month);
	fprintf(editfile, "Bytes_Down_Month     : %lu\n", quota->bytes_down_month);

	fprintf(editfile, "Bytes_Up_Week        : %lu\n", quota->bytes_up_week);
	fprintf(editfile, "Bytes_Down_Week      : %lu\n", quota->bytes_down_week);

	fprintf(editfile, "Bytes_Up_Day         : %lu\n", quota->bytes_up_day);
	fprintf(editfile, "Bytes_Down_Day       : %lu\n", quota->bytes_down_day);

#else

	fprintf(editfile, "Credits              : %qu\n", quota->credits);

	fprintf(editfile, "Bytes_Up_All         : %qu\n", quota->bytes_up);
	fprintf(editfile, "Bytes_Down_All       : %qu\n", quota->bytes_down);

	fprintf(editfile, "Bytes_Up_Month       : %qu\n", quota->bytes_up_month);
	fprintf(editfile, "Bytes_Down_Month     : %qu\n", quota->bytes_down_month);

	fprintf(editfile, "Bytes_Up_Week        : %qu\n", quota->bytes_up_week);
	fprintf(editfile, "Bytes_Down_Week      : %qu\n", quota->bytes_down_week);

	fprintf(editfile, "Bytes_Up_Day         : %qu\n", quota->bytes_up_day);
	fprintf(editfile, "Bytes_Down_Day       : %qu\n", quota->bytes_down_day);


#endif

	fprintf(editfile, "Seconds_Up_All       : %u\n", quota->seconds_up);
	fprintf(editfile, "Seconds_Down_All     : %u\n", quota->seconds_down);

	fprintf(editfile, "Seconds_Up_Month     : %u\n", quota->seconds_up_month);
	fprintf(editfile, "Seconds_Down_Month   : %u\n", quota->seconds_down_month);

	fprintf(editfile, "Seconds_Up_Week      : %u\n", quota->seconds_up_week);
	fprintf(editfile, "Seconds_Down_Week    : %u\n", quota->seconds_down_week);

	fprintf(editfile, "Seconds_Up_Day       : %u\n", quota->seconds_up_day);
	fprintf(editfile, "Seconds_Down_Day     : %u\n", quota->seconds_down_day);



	fprintf(editfile, "Files_Up_All         : %u\n", quota->files_up);
	fprintf(editfile, "Files_Down_All       : %u\n", quota->files_down);

	fprintf(editfile, "Files_Up_Month       : %u\n", quota->files_up_month);
	fprintf(editfile, "Files_Down_Month     : %u\n", quota->files_down_month);

	fprintf(editfile, "Files_Up_Week        : %u\n", quota->files_up_week);
	fprintf(editfile, "Files_Down_Week      : %u\n", quota->files_down_week);

	fprintf(editfile, "Files_Up_Day         : %u\n", quota->files_up_day);
	fprintf(editfile, "Files_Down_Day       : %u\n", quota->files_down_day);


	fprintf(editfile, "Num_Logins           : %u\n", neww->num_logins);
	fprintf(editfile, "Num_Uploads          : %u\n", neww->num_uploads);
	fprintf(editfile, "Num_Downloads        : %u\n", neww->num_downloads);

	if(server_usermatch) {
		int i;
		for (i=0; i<IPMATCHMAX; i++)
			if (neww->matchip[i][0])
				fprintf(editfile, "Pattern %-2d           : %s\n",
						i+1,neww->matchip[i]);
			else
				fprintf(editfile, "Pattern %-2d           : NA\n",
						i+1);

	}

	fflush(editfile);

	sprintf(buf, "%s %s", getenv("EDITOR"), tnam);

	/* I actually thought that rewinding the file would work but emacs
	 * renamed the file to ~ so rewinding it would still refer the the
	 * original file :-/ */

	fclose(editfile);


#ifdef WIN32
	consolef("Just about to execute '%s'...\n", buf);


	if (system(buf)) {


		consolef("\n\nFailed to start editor.\nPlease edit '%s' manually and push 'y' then return when ready: ", tnam);
		fflush(stdout);
		get_word(stdin,buf);

	}


	editfile = fopen(tnam, "r");

	if (!editfile) {
		perror("Failed to re-open edit file");
		unlink(tnam);
		return;
	}

#else


	if (system(buf) || !(editfile = fopen(tnam, "r"))) {

		perror("Call to editor failed, ignoring contents of edit file");
		fclose(editfile);
		unlink(tnam);
		return;

	}

#endif

	consolef("\n\n\n");

	/* Here parse what they've put. */

	consolef("The following items were changed: \n");

	while (get_word(editfile, buf)) {
		int l = strlen(buf), i;
		unsigned long ll;
		lion64u_t ll2;

		if (!strncasecmp(buf, "Username", l)) {
			get_word(editfile, buf); /* Get rid of : */
			get_word(editfile, buf);
			if (strcmp(buf, neww->user)) {
				consolef("Username ");
				strncpy(new2.user, buf, MAXUSERNAME);
				new2.user[MAXUSERNAME-1] = (char)0;
			}
		} else if(!strncasecmp(buf, "Password", l)) {
			get_word(editfile, buf); /* Get rid of : */
			get_word(editfile, buf);
			if (strcmp(buf, neww->pass)) {
				consolef("Password ");
				strncpy(new2.pass, buf, MAXPASSNAME);
				new2.pass[MAXPASSNAME-1] = (char)0;

				user_crypt(new2.pass, NULL);


			}
		} else if(!strncasecmp(buf, "Level", l)) {
			unsigned int lev = 0;
			get_word(editfile, buf); /* Get rid of : */
			get_word(editfile, buf);

			lev = atoflags(buf);

			if (lev != neww->level) {
				consolef("Level ");
				new2.level = lev;
			}
		} else if(!strncasecmp(buf, "QuotaDaily", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->daily) {
				consolef("QuotaDaily ");
				quota->daily = ll;
			}
		} else if(!strncasecmp(buf, "QuotaRatio", l)) {
			get_word(editfile, buf); /* Get rid of : */
			i = get_int(editfile);
			if (i != quota->ratio) {
				consolef("QuotaRatio ");
				quota->ratio = i;
			}
		} else if(!strncasecmp(buf, "QuotaTime", l)) {
			get_word(editfile, buf); /* Get rid of : */
			i = get_int(editfile);
			if (i != quota->time_limit) {
				consolef("QuotaTime ");
				quota->time_limit = i;
			}
		} else if(!strncasecmp(buf, "Credits", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);

			if (ll2 != quota->credits) {
				consolef("Credits ");
				quota->credits = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Up_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);

			if (ll2 != quota->bytes_up) {
				consolef("Bytes_Up ");
				quota->bytes_up = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Up_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);

			if (ll2 != quota->bytes_up_month) {
				consolef("Bytes_Up_Month ");
				quota->bytes_up_month = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Up_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);

			if (ll2 != quota->bytes_up_week) {
				consolef("Bytes_Up_Week ");
				quota->bytes_up_week = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Up_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);

			if (ll2 != quota->bytes_up_day) {
				consolef("Bytes_Up_Day ");
				quota->bytes_up_day = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Down_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);
			if (ll2 != quota->bytes_down) {
				consolef("Bytes_Down ");
				quota->bytes_down = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Down_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);
			if (ll2 != quota->bytes_down_month) {
				consolef("Bytes_Down_Month ");
				quota->bytes_down_month = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Down_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);
			if (ll2 != quota->bytes_down_week) {
				consolef("Bytes_Down_Week ");
				quota->bytes_down_week = ll2;
			}
		} else if(!strncasecmp(buf, "Bytes_Down_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll2 = get_longlong(editfile);
			if (ll2 != quota->bytes_down_day) {
				consolef("Bytes_Down_Day ");
				quota->bytes_down_day = ll2;
			}
		} else if(!strncasecmp(buf, "Seconds_Up_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_up) {
				consolef("Seconds_Up ");
				quota->seconds_up = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Up_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_up_month) {
				consolef("Seconds_Up_Month ");
				quota->seconds_up_month = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Up_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_up_week) {
				consolef("Seconds_Up_Week ");
				quota->seconds_up_week = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Up_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_up_day) {
				consolef("Seconds_Up_Day ");
				quota->seconds_up_day = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Down_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_down) {
				consolef("Seconds_Down ");
				quota->seconds_down = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Down_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_down_month) {
				consolef("Seconds_Down_Month ");
				quota->seconds_down_month = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Down_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_down_week) {
				consolef("Seconds_Down_Week ");
				quota->seconds_down_week = ll;
			}
		} else if(!strncasecmp(buf, "Seconds_Down_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->seconds_down_day) {
				consolef("Seconds_Down_Day ");
				quota->seconds_down_day = ll;
			}
		} else if(!strncasecmp(buf, "Files_Up_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_up) {
				consolef("Files_Up ");
				quota->files_up = ll;
			}
		} else if(!strncasecmp(buf, "Files_Up_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_up_month) {
				consolef("Files_Up_Month ");
				quota->files_up_month = ll;
			}
		} else if(!strncasecmp(buf, "Files_Up_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_up_week) {
				consolef("Files_Up_Week ");
				quota->files_up_week = ll;
			}
		} else if(!strncasecmp(buf, "Files_Up_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_up_day) {
				consolef("Files_Up_Day ");
				quota->files_up_day = ll;
			}
		} else if(!strncasecmp(buf, "Files_Down_All", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_down) {
				consolef("Files_Down ");
				quota->files_down = ll;
			}
		} else if(!strncasecmp(buf, "Files_Down_Month", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_down_month) {
				consolef("Files_Down_Month ");
				quota->files_down_month = ll;
			}
		} else if(!strncasecmp(buf, "Files_Down_Week", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_down_week) {
				consolef("Files_Down_Week ");
				quota->files_down_week = ll;
			}
		} else if(!strncasecmp(buf, "Files_Down_Day", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != quota->files_down_day) {
				consolef("Files_Down_Day ");
				quota->files_down_day = ll;
			}
		} else if(!strncasecmp(buf, "Num_Logins", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != neww->num_logins) {
				consolef("Num_Logins ");
				new2.num_logins = ll;
			}
		} else if(!strncasecmp(buf, "Num_Uploads", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != neww->num_uploads) {
				consolef("Num_Uploads ");
				new2.num_uploads = ll;
			}
		} else if(!strncasecmp(buf, "Num_Downloads", l)) {
			get_word(editfile, buf); /* Get rid of : */
			ll = get_long(editfile);
			if (ll != neww->num_downloads) {
				consolef("Num_Downloads ");
				new2.num_downloads = ll;
			}
		} else if(!strncasecmp(buf, "Pattern", l)) {
			i = get_int(editfile)-1;
			if (i>=IPMATCHMAX) {
				consolef("Max patterns is set to %d (0-%d) and you specified %d.\n",
						 IPMATCHMAX, IPMATCHMAX-1, i+1);
				continue;
			}
			get_word(editfile, buf); /* Get rid of : */
			get_word(editfile, buf);

			if (strcmp(neww->matchip[i],buf)) {
				consolef("Pattern_%d ", i+1);
				strncpy(new2.matchip[i], buf, IPMATCHLEN);
				new2.matchip[i][IPMATCHLEN-1] = (char)0;
			}
		} else {
			fprintf(stderr,"Unknown keyword '%s'\r\n", buf);
		}
	}
	fclose(editfile);
	unlink(tnam);

	consolef("\n\nAccept changes? (Y/N): "); fflush(stdout);

	do {
		get_word(stdin, buf);
	} while(*buf != 'y' && *buf != 'Y' && *buf != 'n' && *buf != 'N');

	if (*buf == 'y' || *buf == 'Y') {
		user_update(&new2);
		quota->dirty = 1;
		quota_exit(neww);


	}

	consolef("Edit complete.\n");
	return;
}





void user_addglobal(char *str)
{
	struct global_struct *s;

	s = (struct global_struct *) malloc(sizeof(struct global_struct));

	if (!s) {
		consolef("Out of memory:(\n");
		return ;
	}

	s->matchip = str;

	s->next = global_head;
	global_head = s;
}


int user_ipmatch(struct login_node *valid, struct login_node *remote)
{
	/* Return 0 is no match found! */
	int i;
	char rempat[IPMATCHLEN];
	//struct global_struct *global;

	sprintf(rempat, "%s@%s", remote->ident, lion_ntoa(remote->host));

	for (i=0; i< IPMATCHMAX; i++)

		if (valid->matchip[i]) { /* If this match is defined... */

			if (!fnmatch(valid->matchip[i], rempat, 0)) {
				consolef("user_ipmatch(%s): valid pattern '%s'\n",
						 remote->user ? remote->user : "(null)",
						 valid->matchip[i]);

				if (check_nocapmatch(rempat)) {
					remote->status |= ST_NOCAP;
				}

				return 1;
			}
		}

	/* Check global patterns */
	if (misc_patt_match_gbl(PATT_GLOBAL, rempat)) {

		consolef("user_ipmatch(%d): valid GLOBAL pattern '%s'\n",
				 -1, rempat);

		if (check_nocapmatch(rempat))
			remote->status |= ST_NOCAP;


		return 1;
	}

	consolef("user_ipmatch(%d): failed all IP patterns\n", -1);
	return 0;
}


int user_getuid(struct login_node *l)
{
	if (server_useridstart < 100)
		return -1;

	return (l->offset / sizeof(struct login_node)) + server_useridstart;
}

#if 0 // #ifndef INTERNAL_LS - NO LONGER SUPPORTED.

void user_passwd(void)
{ /* Create a fake /etc/passwd file. */
	struct login_node buf;
	char path[256];
	FILE *fd;

	if (!server_useridstart || geteuid())
		return;

	file_gononroot(); /* Just in case */

	sprintf(path, "%s/etc/passwd", server_usechroot);

	if (!strcmp(path, "/etc/passwd") ||
		!strcmp(path, "etc/passwd") ||
		!strcmp(path, "//etc/passwd"))
		{
			consolef("REFUSING TO OVERWRITE REAL /etc/passwd, rootdir not set?!\n");
			return ;
		}


	if (!(fd = fopen(path, "w")))
		{
			consolef("Couldn't open %s\n", path);
			return ;
		}

	/* Rewind */
	if (lseek(user_db_fd, 0, SEEK_SET) < 0) {
		perror("user_findbyname(lseek(rewind))");
		return ;
	}


	fprintf(fd, "root:*:0:0:root:/:/dev/null\n");


	while(read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf))
		{
			if (*buf.user)
				fprintf(fd, "%s:*:%d:1:FTP User:/:/dev/null\n", buf.user,
						user_getuid(&buf));
		}

	fclose(fd);

	return;
}
#endif

/* if ( (VALUE) == global_tub##Z[0] ) exit(0); */
#define USERTOP_SCANUP( VALUE, FILES, SECONDS, Z )  \
    for (i = 0; i < NUM_TOP; i++) {                 \
        if ( (VALUE) >= global_tub##Z[i]) {         \
            /* Move all down one, insert here */    \
            for (j = NUM_TOP-1; j > i ; j--) {                          \
                strcpy((char *)global_tuu##Z[j], (char *)global_tuu##Z[j - 1]); \
                strcpy((char *)global_tug##Z[j], (char *)global_tug##Z[j - 1]); \
                global_tub##Z[j] = global_tub##Z[j - 1];                \
                global_tuf##Z[j] = global_tuf##Z[j - 1];                \
                global_tus##Z[j] = global_tus##Z[j - 1];                \
                strncpy((char *)global_tut##Z[j], (char *)global_tut##Z[j - 1], MAXTAGLINE); \
                global_tut##Z[j][MAXTAGLINE - 1] = 0;                   \
            }                                                           \
            strcpy((char *)global_tuu##Z[i], (char *)buf.user);         \
            if (buf.current_group)                                      \
                strcpy((char *)global_tug##Z[i], (char *)((struct group *)buf.current_group)->name); \
            else                                                        \
                strcpy((char *)global_tug##Z[i], "-");                          \
            strncpy((char *)global_tut##Z[i], (char *)buf.tagline, MAXTAGLINE); \
            global_tut##Z[i][MAXTAGLINE - 1] = 0;                       \
            /* We don't do SPEED yet, temp hack so it doesn't core! */  \
            global_tub##Z[i] = (VALUE);                                 \
            global_tuf##Z[i] = (FILES);                                 \
            global_tus##Z[i] = (SECONDS)?(SECONDS):1;                   \
            break;                                                      \
        }                                                               \
    }


#define USERTOP_SCANDN( VALUE, FILES, SECONDS, Z ) \
    for (i = 0; i < NUM_TOP; i++) {                \
        if ( (VALUE) > global_tdb##Z[i]) {         \
            /* Move all down one, insert here */   \
            for (j = NUM_TOP-1; j > i ; j--) {                          \
                strcpy((char *)global_tdu##Z[j], (char *)global_tdu##Z[j - 1]); \
                strcpy((char *)global_tdg##Z[j], (char *)global_tdg##Z[j - 1]); \
                global_tdb##Z[j] = global_tdb##Z[j - 1];                \
                global_tdf##Z[j] = global_tdf##Z[j - 1];                \
                global_tds##Z[j] = global_tds##Z[j - 1];                \
                strncpy((char *)global_tdt##Z[j], (char *)global_tdt##Z[j - 1], MAXTAGLINE); \
                global_tdt##Z[j][MAXTAGLINE - 1] = 0;                   \
            }                                                           \
            strcpy((char *)global_tdu##Z[i], (char *)buf.user);         \
            if (buf.current_group)                                      \
                strcpy((char *)global_tug##Z[i],(char *)((struct group *)buf.current_group)->name); \
            else                                                        \
                strcpy((char *)global_tug##Z[i], "-");                          \
            strncpy((char *)global_tdt##Z[i], (char *)buf.tagline, MAXTAGLINE); \
            global_tdt##Z[i][MAXTAGLINE - 1] = 0;                       \
            /* We don't do SPEED yet, temp hack so it doesn't core! */  \
            global_tdb##Z[i] = (VALUE);                                 \
            global_tdf##Z[i] = (FILES);                                 \
            global_tds##Z[i] = (SECONDS)?(SECONDS):1;                   \
            break;                                                      \
        }                                                               \
    }


int user_top(void)
{
	/*
	 *
	 * Right. This function runs through all usernodes / quota, and builds
	 * various TOP lists. It's a bit of a bitch but only runs every 10 mins.
	 *
	 */

	struct login_node buf;
	struct quota_node *quota;
	int i, j;


	/* Rewind */
	if (lseek(user_db_fd, 0, SEEK_SET) < 0) {
		perror("user_top(lseek(rewind))");
#ifdef PARANOIA1
		lseek(user_db_fd, 0, SEEK_END);
#endif
		return 0;
	}

	for (i = 0; i < NUM_TOP; i++) {
		/* Clear the Bytes flag, as we don't print a line with 0 bytes. */

		global_tubd[i] = 0;     /* DAY */
		global_tuud[i][0] = 0;
		global_tutd[i][0] = 0;
		global_tugd[i][0] = 0;
		global_tufd[i] = 0;
		global_tusd[i] = 0;

		global_tdbd[i] = 0;
		global_tdud[i][0] = 0;
		global_tdtd[i][0] = 0;
		global_tdgd[i][0] = 0;
		global_tdfd[i] = 0;
		global_tdsd[i] = 0;


		global_tubw[i] = 0;     /* WEEK */
		global_tuuw[i][0] = 0;
		global_tutw[i][0] = 0;
		global_tugw[i][0] = 0;
		global_tufw[i] = 0;
		global_tusw[i] = 0;

		global_tdbw[i] = 0;
		global_tduw[i][0] = 0;
		global_tdtw[i][0] = 0;
		global_tdgw[i][0] = 0;
		global_tdfw[i] = 0;
		global_tdsw[i] = 0;


		global_tubm[i] = 0;     /* MONTH */
		global_tuum[i][0] = 0;
		global_tutm[i][0] = 0;
		global_tugm[i][0] = 0;
		global_tufm[i] = 0;
		global_tusm[i] = 0;

		global_tdbm[i] = 0;
		global_tdum[i][0] = 0;
		global_tdtm[i][0] = 0;
		global_tdgm[i][0] = 0;
		global_tdfm[i] = 0;
		global_tdsm[i] = 0;


		global_tub[i] = 0;      /* ALL-TIME */
		global_tuu[i][0] = 0;
		global_tut[i][0] = 0;
		global_tug[i][0] = 0;
		global_tuf[i] = 0;
		global_tus[i] = 0;

		global_tdb[i] = 0;
		global_tdu[i][0] = 0;
		global_tdt[i][0] = 0;
		global_tdg[i][0] = 0;
		global_tdf[i] = 0;
		global_tds[i] = 0;

	}



	while(read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf)) {

		if (!buf.user[0]) continue; /* User been deleted? */

		/* Look up this user's quota field (if any) */

		quota_new(&buf);

		quota = quota_getquota(&buf);


		if (!quota) continue; /* No quota, no entry into TOP */


		/* Set group */
		buf.current_group = group_findcurr_byname(buf.user);


#if 1
		/* DAY-CHECK UP! */
		USERTOP_SCANUP(quota->bytes_up_day, quota->files_up_day,
					   quota->seconds_up_day, d);

		/* DAY-CHECK DOWN! */
		USERTOP_SCANDN(quota->bytes_down_day, quota->files_down_day,
					   quota->seconds_down_day, d);

		/* WEEK-CHECK UP! */
		USERTOP_SCANUP(quota->bytes_up_week, quota->files_up_week,
					   quota->seconds_up_week, w);

		/* WEEK-CHECK DOWN! */
		USERTOP_SCANDN(quota->bytes_down_week, quota->files_down_week,
					   quota->seconds_down_week, w);

#endif

		/* MONTH-CHECK UP! */
		USERTOP_SCANUP(quota->bytes_up_month, quota->files_up_month,
					   quota->seconds_up_month, m);

#if 1
		/* MONTH-CHECK DOWN! */
		USERTOP_SCANDN(quota->bytes_down_month, quota->files_down_month,
					   quota->seconds_down_month, m);


		/* ALLTIME-CHECK UP! */
		USERTOP_SCANUP(quota->bytes_up, quota->files_up,
					   quota->seconds_up, );

		/* ALLTIME-CHECK DOWN! */
		USERTOP_SCANDN(quota->bytes_down, quota->files_down,
					   quota->seconds_down, );

#endif

		quota_exit(&buf);

	}


	return 1;

}



char *user_get_name_by_uid(int uid)
{
	static struct login_node buf;
	static int cache_uid[MAX_CACHE], first_time = 1;
	static char *cache_name[MAX_CACHE];

	int last_free = -1, i;

	/* Adding caching for quicker lookup...
	 * search through cached entries, if found, promote to first
	 * if not pick last entry and look it up..
	 */

#ifndef NO_UID_CACHE /* Don't use cache lookup */

	if (first_time) {
		first_time = 0;
		memset(cache_uid, -1, sizeof(cache_uid));
	}


	for (i = 0; i < MAX_CACHE; i++) {
		if (cache_uid[i] < 0) last_free = i;
		else if (cache_uid[i] == uid) {
			char *spare;
			/* Promote to first! */

			if (i) { /* Not already at the front */
				spare = cache_name[i];

				last_free = i;

				/* Copy things down. */
				for (i = last_free - 1; i >= 0; i--) {
					cache_uid[i+1] = cache_uid[i];
					cache_name[i+1] = cache_name[i];
				}

				cache_uid[0] = uid;
				cache_name[0] = spare;
				i = 0;
			}

			return cache_name[i];

		}
	}

	if (last_free < 0) last_free = MAX_CACHE-1; /* if no free, pick last */

#endif

	if (lseek(user_db_fd, uid * sizeof(buf) , SEEK_SET) < 0) {
		perror("user_get_name_by_uid(lseek(rewind))");
		return NULL;
	}

	if (read(user_db_fd, &buf, sizeof(buf)) == sizeof(buf)) {
#ifdef PARANOIA1
		lseek(user_db_fd, 0, SEEK_END);
#endif

		/* Found it, add to cache*/

#ifndef NO_UID_CACHE /* no cache */

		if (cache_uid[last_free] >= 0)
			free(cache_name[last_free]);

		cache_uid[last_free] = uid;
		cache_name[last_free] = mystrcpy(buf.user);

#endif

		return buf.user;
	}


#ifdef PARANOIA1
	lseek(user_db_fd, 0, SEEK_END);
#endif
	return NULL;
}


char *user_get_username(int uid)
{
	struct passwd *pw = NULL;
	char *name;

	if (!server_lookupuid)
		pw = NULL;
#ifndef WIN32
	else
		pw = getpwuid(uid);
#endif

	if (!pw) {

		/* Look up the user in our data base, this needs to be fixed!! */
		if (server_useridstart) {

			/* Check for upper limit */
			if ((uid >= server_useridstart) && (uid < 65535)) {

				name = user_get_name_by_uid(uid - server_useridstart);

				if (name) return name;

			}
		}

		return server_nobody;
	}

#ifndef WIN32
	return pw->pw_name;
#else
	return server_nobody;
#endif
}


int user_getgidbyname(char *user)
{
	struct login_node *l;

	if (server_useridstart < 100)
		return -1;

	if (!(l = login_isloggedin(user)) && !(l = user_findbyname(user)))
		return -1;

	return user_getgid(l);
}

int user_getuidbyname(char *user)
{
	struct login_node *l;

	if (server_useridstart < 100)
		return -1;

	if (!(l = login_isloggedin(user)) && !(l = user_findbyname(user)))
		return -1;
	return (l->offset / sizeof(struct login_node)) + server_useridstart;
}


//------------------------- SITE FUNCTIONS -------------------------//
void user_site_user(struct login_node *t, char *args)
{
	struct login_node *luser;
	struct quota_node *q;
	struct group *g;
	char *ar = args, *user;
	char *aflags, *flag;
	char ratio[10], lasttime[80], credz[13];
	int i, j;

	user = digtoken((char **)&ar, " ");

	if (user) {

		/* Can't find the user. */
		if (!(luser = login_isloggedin(user)) && !(luser = user_findbyname(user))) {
			socket_print(t, "500 No such user as %s\n", user);
			return;
		}

		/* Not looking at themselves */


		// BZZZT! Wrong. Multiple logins, means different node, but same login.
		// we should compare the usernames, but a faster way is check same
		// quota node, if they have one, or even better, check they save to
		// same offset. (the uid.)
		//if (luser != t) {
		if (luser->offset != t->offset) {

			/* They don't have UF_ADDUSER */
			if ( !((t->level|server_defaultflags)&UF_ADDUSER)) {

				// We should fail the lookup here based on permission.
				// However, there are some special cases where would still
				// allow it. If user is gadmin, or
				/* User isn't a member of their current GADMIN group. */
				// which I'm not sure what it means.

				// If they have no group, fail.
				if (!t->current_group) {
					socket_number(t, 599, "USER");
					return;
				}

				/* Not a GADMIN of the current_group */
				if (t->current_group && !group_hasgadmin(t->current_group, t->user)) {
					socket_number(t, 599, "USER");
					return;
				}

				/* User isn't a member of their current GADMIN group. */
				if (t->current_group && !group_hasuser(t->current_group, luser->user)) {
					socket_number(t, 599, "USER");
					return;
				}
			}
		}

		/* Grab Quota */
		if (!luser->quota)
			quota_new(luser);

		/* Display the user. */

		q = (struct quota_node *)luser->quota;
		g = (struct group *)luser->current_group;
		if (q) {
			snprintf(ratio, 9, "%s", misc_format_ratio(q->ratio));
			snprintf(credz, 12, "%.1f MB", q->credits / 1048576.0);
		} else {
			snprintf(ratio, 9, "unkwn");
			sprintf(credz, "0.0 MB");
		}

		if (!luser->idle) {
			strcpy(lasttime, "User has never logged in.");
		} else {
			snprintf(lasttime, 79, "User last logged in from %s@%s at %s",
					 luser->ident, lion_ntoa(luser->host),
					 ctime(&luser->time));
			chop(lasttime);
		}


		socket_print(t,
			"200- [WHOIS]\r\n"
			".-[ GENERAL ]------------------------------------------------------------------.\r\n"
			"|   Username: %-20.20s | Max Logins: %-4d |    Ratio: %-12.12s |\r\n"
			"|     Groups: %-20.20s |     Max Up: %-4d |  Credits: %-12.12s |\r\n"
			"| Actv Group: %-20.20s |   Max Down: %-4d |   Logins: %-12d |\r\n"
			"|    Tagline: %-64.64s |\r\n",
			/* LINE 1 */
			luser->user, luser->num_logins, ratio,
			/* LINE 2 */
			group_list_groups(user), luser->num_uploads, credz,
			/* LINE 3 */
			(g) ? g->name : "None", luser->num_downloads, (q) ? q->login_times : 0,
			/* LINE 4 */
			(luser->tagline[0]) ? luser->tagline : "No tagline!");
		if (luser->level) {
			i = 0;
			socket_print(t,
			"|-[ FLAGS ]--------------------------------------------------------------------|\r\n");
			aflags = flagstoa(luser->level);
			while ((flag = digtoken((char **)&aflags, "|"))) {
				socket_print(t, "| %-12.12s: %-*s ", flag, 22 + (i%2), flagtowhat(atoflags(flag)));
				if (i % 2)
					socket_print(t, "|\r\n");
				i++;
			}
			if ((i%2))
				socket_print(t, "| %37s |\r\n", " ");
			i = 0;
		}

		if (q) {
			socket_print(t,
			"|-[ STATS ]--------------------------------------------------------------------|\r\n"
			"|              |         Today |     This Week |    This Month |       Alltime |\r\n"
			"|--------------+---------------+---------------+---------------+---------------|\r\n"
			"|    Uploaded: | %10.1f Mb | %10.1f Mb | %10.1f Mb | %10.1f Mb |\r\n"
			"|  Downloaded: | %10.1f Mb | %10.1f Mb | %10.1f Mb | %10.1f Mb |\r\n"
			"|       Total: | %10.1f Mb | %10.1f Mb | %10.1f Mb | %10.1f Mb |\r\n"
			"|       Nuked: |               |               |               | %10.1f Mb |\r\n",
			/* STATS 1 */
			q->bytes_up_day / 1048576.0 , q->bytes_up_week / 1048576.0,
			q->bytes_up_month / 1048576.0, q->bytes_up / 1048576.0,
			/* STATS 2 */
			q->bytes_down_day / 1048576.0, q->bytes_down_week / 1048576.0,
			q->bytes_down_month / 1048576.0, q->bytes_down / 1048576.0,
			/* STATS 3 */
			(q->bytes_down_day + q->bytes_up_day) / 1048576.0,
			(q->bytes_down_week + q->bytes_up_week) / 1048576.0,
			/* STATS 4 */
			(q->bytes_down_month + q->bytes_up_month) / 1048576.0,
			(q->bytes_down       + q->bytes_up) / 1048576.0,
			/* STATS 5 */
			q->got_nuked_bytes / 1048576.0);
		}

		socket_print(t,
			"|------------------------------------------------------------------------------|\r\n"
			"| %-76.76s |\r\n",
			lasttime);
		if (file_hasmessages(t)) {
			socket_print(t,
			"| User has unread messages.                                                    |\r\n");
		}

		/* Show IP MATCHES */
		if (luser->matchip[0][0] && (luser->matchip[0][0] != '\r' && luser->matchip[0][0] != '\n')) {

			socket_print(t,
			"|-[ IPMATCHES ]----------------------------------------------------------------|\r\n");

			for (i=0,j=0; i < IPMATCHMAX; i++) {
				if (!luser->matchip[i][0] || luser->matchip[i][0] == '\r' || luser->matchip[i][0] == '\n')
					continue;
				socket_print(t, "| IP %2d: %-*s ", i + 1, 29 + (j%2), luser->matchip[i]);
				if (j % 2)
					socket_print(t, "|\r\n");
				j++;
			}
			if (j%2)
				socket_print(t, "| %37s |\r\n", " ");
		}


		socket_print(t,
			"`------------------------------------------------------------------------------'\r\n"
			"200 WHOIS command successful\r\n");

		/* Release Quota */
		if (!login_isloggedin(user))
			quota_exit(luser);

		return;
	}

	/* No user supplied, list all */
	/* TODO XXX - A proper list of users on system with stats */

	if (!((t->level|server_defaultflags)&UF_ADDUSER)) {
		socket_number(t, 599, "USER");
		return;
	}

	socket_print(t, "200-USER:\r\n");

	user_sitelist(t);  // User site list now sends 200.

	// socket_number(t, 200, "USER");

}
