#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_DIRENT_H
#include <dirent.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "global.h"
#include "login.h"
#include "data.h"
#include "userflag.h"
#include "misc.h"
#include "file.h"
#include "socket.h"
#include "help.h"
#include "user.h"
#include "log.h"
#include "site.h"
#include "check.h"
#include "groups.h"
#include "quota.h"
#include "exec.h"
#include "section.h"
#include "sitecust.h"
#include "race.h"
#include "fnmatch.h"

#include "global.h"
#include "dupe.h"
#include "sfv.h"

#include "irc.h"

extern time_t server_uptime;
void main_rehash(void);




void login_site_kick(struct login_node *t, char *args)
{

	struct login_node *runn;

	// SKIP LEADING SPACES
	while(*args == ' ')
		args++;

	if (!*args) {
    	help_commands(t, "KICK");
		return;
	}

	// PERMITTED TO KICK?
	if (!((t->level|server_defaultflags)&UF_KICK)) {

		if (strcasecmp(t->user, args)) {
			socket_number(t, 599, "KICK");
			return;
		}

	}

	// FIND USER TO KICK
	if ((runn = login_isloggedin_notus(args, t))) {

		socket_number(runn, 569, t->user);
		socket_number(t, 269, runn->user);
		login_exit(runn);

		return;
	}

	socket_number(t, 568, args);

}


void login_site_unnuke(struct login_node *t, char *args)
{

	char *ar=args, *npath, *tmp, *reason, path[256], keep = 0;
	int multi, ratio;
	lion64_t nuked_bytes;
	struct wipedata *wdata = NULL, *wrun;
	DIR *dh;
	struct dirent *dp;
	struct login_node *udata;
	struct quota_node *q;

	consolef("site unnuke: (%p/%s) %s\n", t, t->user ? t->user : "(null)", args);

	// Can we unnuke??
	if (!server_unnuke) {
		socket_print(t, "599 Sorry, this server is not enabled for unnuking!\r\n");
		return;
	}

	// Check their flags
	if (!((t->level|server_defaultflags)&UF_PUNISH)) {
		socket_print(t, "599 You do not have access to UNNUKE!\r\n");
		return;
	}

	// Grab path
	if (!(npath = digtoken((char **)&ar, " "))) {
		socket_print(t, "599 Usage: UNNUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	// Grab multiplier
	if (!(tmp = digtoken((char **)&ar, " "))) {
		socket_print(t, "599 Usage: UNNUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	multi = atoi(tmp);

	// Grab reason
	if ((strlen(ar)==0) || (strlen(args)==0)) {
		socket_print(t, "599 Usage: UNNUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	reason = ar;

	// Make sure path is OK first.
	while (*npath == ' ')
		npath++;

	snprintf(path, sizeof(path), "NUKED-%s", npath);

	if (!file_pathchecklogin(t, path) || !file_isdir(t, path)) {
		socket_number(t, 550, npath);
		return;
	}

	// Remove "NUKED by *"
	if (!(dh = opendir(path))) {
		socket_print(t, "500 Failed to UNNUKE, couldn't find that dir!\r\n");
		return;
	}

	// Count up the files using file_recursive...
	file_wipe_recursive(path, &wdata, 0);


	ratio = section_ratio(path);
	if (!ratio)
		ratio = server_userquotaratio;
	socket_print(t, "200-[ UNNUKE BEGIN ]\r\n"
					"200-UnNuked: %s at x%d\r\n"
					"200-Reason : %s\r\n",
					npath, multi, reason);

	while ((dp = readdir(dh)) != NULL) {
		if (!fnmatch("NUKED_by_*", dp->d_name, FNM_CASEFOLD)) {
			snprintf(genbuf, sizeof(genbuf), "%s/%s", path, dp->d_name);
			consolef("Removing %s\n", genbuf);
			remove(genbuf);
		}
	}
	closedir(dh);

	// Move the dir from NUKED to normal
	tmp = path2file(path);

	if (tmp) {
		tmp--;
		keep = *tmp;
		*tmp = 0;
	}

	// TODO: Allow nuke_format from config file.
	snprintf(genbuf, sizeof(genbuf), "%s/%s", path, npath);

	if (tmp)
		*tmp = keep;

	// Remove temporary file placement.
	unlink(genbuf);

	file_goroot();
	rename(path, genbuf);
	file_gononroot();


	// Is it complete, or incomplete? TODO, replace incomplete if unnuked
	//sfv_state_load(genbuf);


#ifdef IRCBOT
	// Announce UNNUKE
	if (section_announce(path)) {
		irc_announce("UNNUKE|section=%s|dir=%s|nuker=%s|multiplier=%d|reason=%s\n",
				section_name(path), npath, t->user, multi, reason);
	}
#endif

	// Go through wdata array removing credits from each user and their
	// group...  Also announce who was nuked
	for (wrun = wdata; wrun; wrun = wrun->next) {

		// Ignore files owned by Unknown...
		if (!strcasecmp(wrun->user, "Unknown"))
			continue;

		// Add the credits!
		if ( !(udata = login_isloggedin(wrun->user)) && !(udata = user_findbyname(wrun->user))) {
			consolef("[UNNUKE] An error occured finding user %s to reimberse\n", wrun->user);
			continue;
		}

		quota_new(udata);

		// Multiply the bytes nuked by the multi by the section ratio
		nuked_bytes = wrun->bytes * ratio * multi;

		q = (struct quota_node *)udata->quota;

		q->got_nuked_files -= wrun->files;
		q->got_nuked_bytes -= wrun->bytes;
		q->credits += nuked_bytes;

#ifdef IRCBOT
		// Announce NUKEE
		if (section_announce(path)) {
			irc_announce("UNNUKEE|section=%s|user=%s|group=%s|files=%d|mbytes=%0.2f|credits=%0.2f\n",
				section_name(path), wrun->user, wrun->group, wrun->files,
				wrun->bytes / 1024.0 / 1024.0, nuked_bytes / 1024.0 / 1024.0);
		}
#endif

		socket_print(t, "200- `--> %s/%s reimbursed for %d files %.2f MB in size (%.2f MB creds)\r\n",
			wrun->user, wrun->group, wrun->files,
			wrun->bytes / 1024.0 / 1024.0, nuked_bytes / 1024.0 / 1024.0);


	}

	socket_print(t, "200 [ UNNUKE END ]\r\n");

}


void login_site_nuke(struct login_node *t, char *args)
{
	char *ar=args, *npath, *tmp, *reason;
	int multi;
	char path[256], keep = 0;
	struct wipedata *wdata = NULL, *wrun;
	struct section_node *run;
	struct login_node *udata;
	struct quota_node *q;
	int ratio;
	lion64_t  nuked_bytes;
	FILE *TMP;

	consolef("site nuke: (%p/%s) %s\n", t, t->user ? t->user : "(null)", args);

	// Check their flags
	if (!((t->level|server_defaultflags)&UF_PUNISH)) {
		socket_print(t, "599 You do not have access to NUKE!\r\n");
		return;
	}

	// Grab path
	if (!(npath = digtoken((char **)&ar, " "))) {
		socket_print(t, "599 Usage: NUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	// Grab multiplier
	if (!(tmp = digtoken((char **)&ar, " "))) {
		socket_print(t, "599 Usage: NUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	multi = atoi(tmp);

	// Grab reason
	if ((strlen(ar)==0) || (strlen(args)==0)) {
		socket_print(t, "599 Usage: NUKE <RELEASE> <MULTI> <REASON>\r\n");
		return;
	}

	reason = ar;

	// Make sure path is OK first.
	while (*npath == ' ')
		npath++;

	strcpy(path, npath);

	if (!file_pathchecklogin(t, path) || !file_isdir(t, path)) {
    	socket_number(t, 550, npath);
		return;
	}

	// Make sure path isn't root/section
	if (!strcmp(path, "/")) {
		socket_print(t, "500 Invalid path\r\n");
		return;
	}

	// TODO: split from path the section base...
	for (run = section_node_head; run; run = run->next) {
		if (!(strcasecmp(run->path, path))) {
			socket_print(t, "500 Invalid path\r\n");
			return;
		}
	}


	// "Wipe" the directory.  This returns who and how many bytes
	// have been wiped in the wdata array.  server_nukestyle
	// determines if the files are deleted or left.
	file_wipe_recursive(path, &wdata, (server_unnuke) ? 0 : 1);

	ratio = section_ratio(path);
	if (!ratio)
		ratio = server_userquotaratio;
	socket_print(t, "200-[ NUKE BEGIN ]\r\n"
					"200-Nuked : %s at x%d\r\n"
					"200-Reason: %s\r\n",
					npath, multi, reason);


	// Remove incomplete files.
	if (section_incompletes(path)) {
		sfv_incomplete(path, 1); // 1 = rmdir
	}


#ifdef IRCBOT
	// Announce NUKE
	if (section_announce(path)) {
		irc_announce("NUKE|section=%s|dir=%s|nuker=%s|multiplier=%d|reason=%s\n",
				section_name(path), section_rlsfind(path), t->user,
				multi, reason);
	}
#endif

	// Go through wdata array removing credits from each user and their
	// group...  Also announce who was nuked
	for (wrun = wdata; wrun; wrun = wrun->next) {

		// Ignore files owned by Unknown...
		if (!strcasecmp(wrun->user, "Unknown"))
			continue;

		// Remove the credits!
		if ( !(udata = login_isloggedin(wrun->user)) && !(udata = user_findbyname(wrun->user))) {
			consolef("[NUKE] An error occured finding user %s to punish\n", wrun->user);
			continue;
		}

		quota_new(udata);

		// Multiply the bytes nuked by the multi by the section ratio
		nuked_bytes = wrun->bytes * ratio * multi;

		q = (struct quota_node *)udata->quota;

		q->got_nuked_files += wrun->files;
		q->got_nuked_bytes += wrun->bytes;
		q->last_nuked = time(0);
		q->credits -= nuked_bytes;

#ifdef IRCBOT
		// Announce NUKEE
		if (section_announce(path)) {
			irc_announce("NUKEE|section=%s|user=%s|group=%s|files=%d|mbytes=%0.2f|credits=%0.2f\n",
				section_name(path), wrun->user, wrun->group, wrun->files,
				wrun->bytes / 1024.0 / 1024.0, nuked_bytes / 1024.0 / 1024.0);
		}
#endif

		socket_print(t, "200- `--> %s/%s punished for %d files %.2f MB in size (%.2f MB creds)\r\n",
			wrun->user, wrun->group, wrun->files,
			wrun->bytes / 1024.0 / 1024.0, nuked_bytes / 1024.0 / 1024.0);


	}

	socket_print(t, "200 [ NUKE END ]\r\n");


	// Make a file in the dir with the nuke reason
	snprintf(genbuf, sizeof(genbuf), "%s/NUKED_by_%s_at_x%d_for_%s",
		path, t->user, multi, reason);

	file_goroot();
	if (!server_unnuke)
		mkdir(path, (mode_t)server_permdir);
	TMP = fopen(genbuf, "w");
	if (TMP)
		fclose(TMP);
	file_gononroot();

	tmp = path2file(path);

	if (tmp) {
		tmp--;
		keep = *tmp;
		*tmp = 0;
	}

	// TODO: Allow nuke_format from config file.
	snprintf(genbuf, sizeof(genbuf), "%s/NUKED-%s", path, &tmp[1]);

	if (tmp)
		*tmp = keep;

	file_goroot();
	rename(path, genbuf);
	file_gononroot();


	log_xfer("NUKE %s %s x%d %s\n",
		t->user, npath, multi, reason);

	// We create an empty file with exactly the same name as the directory
	// so it can't be created again. But we need to remove it in unnuke.
	TMP = fopen(path, "w");
	if (TMP)
		fclose(TMP);


	return;

}



void login_site_topup(struct login_node *t, char *args)
{
  char *ar = args, *type;

  type = digtoken((char **)&ar, " ");

  if (!type) {
    socket_number(t, 3280, NULL);
    socket_number(t, 200, "TOP UP");
    return;
  }

  switch(*type) {

  case 'd':
  case 'D':
    /* Daily */
    socket_number(t, 3200, NULL);
    break;

  case 'w':
  case 'W':
    /* Week */
    socket_number(t, 3220, NULL);
    break;

  case 'm':
  case 'M':
    /* Month */
    socket_number(t, 3240, NULL);
    break;

  case 'a':
  case 'A':
    /* Alltime */
    socket_number(t, 3260, NULL);
    break;

  default:
    socket_number(t, 3280, NULL);
  }

  socket_number(t, 200, "TOP UP");
  /*
    socket_print(t, "200-Not Implemented Yet.\r\n");break;
    */
}


void login_site_topdown(struct login_node *t, char *args)
{
  char *ar = args, *type;

  type = digtoken((char **)&ar, " ");

  if (!type) {
    socket_number(t, 3290, NULL);
    socket_number(t, 200, "TOP DOWN");
    return;
  }

  switch(*type) {

  case 'd':
  case 'D':
    /* Daily */
    socket_number(t, 3210, NULL);
    break;

  case 'w':
  case 'W':
    /* Week */
    socket_number(t, 3230, NULL);
    break;

  case 'm':
  case 'M':
    /* Month */
    socket_number(t, 3250, NULL);
    break;

  case 'a':
  case 'A':
    /* Alltime */
    socket_number(t, 3270, NULL);
    break;

  default:
    socket_number(t, 3290, NULL);
  }

  socket_number(t, 200, "TOP DOWN");

}



void login_site_msg(struct login_node *t, char *args)
{

	struct login_node *touser;
	char *ar = args, *user, path[256];
	FILE *fd;

	// No MSGPATH to write notes to, bail.
	if (!server_msgpath) {
		socket_number(t, 560, NULL);
		return;
	}

	user = digtoken((char **)&ar, " ");

	if (!user) {  /* we are checking for messages */

		t->status &= ~ST_MESSAGE;

		sprintf(path, "%s/%s", server_msgpath, t->user);

		consolef("[sitemsg] Checking %s for messages\n", path);

		file_goroot();
		fd = fopen(path, "r");
		file_gononroot();

		if (!fd) {
			socket_number(t, 4200, NULL);
			return;
		}

		while(fgets(genbuf, 256, fd)) {
			chop(genbuf);
			socket_print(t, "200-%s\r\n", genbuf);
		}

		fclose(fd);

		file_goroot();
		unlink(path);
		file_gononroot();

		socket_number(t, 200, "MSG (Messages Purged)");

	} else { /* We are leaving message */

		consolef("[sitemsg] Sending message to %s\n", user);

		/* find user: */
		if (!(touser = user_findbyname(user))) {
      		socket_number(t, 530, user);
			return;
		}

		/* Open user's note file */
		sprintf(path, "%s/%s", server_msgpath, touser->user);
		file_goroot();
		fd = fopen(path, "a");
		file_gononroot();

		if (!fd) {
			socket_number(t, 553, user);
			return;
		}

		fprintf(fd, "Message from %s at %s", t->user, ctime(&t->idle));
		fprintf(fd, ">>> %s\n", ar);

		fclose(fd);

		socket_number(t, 200, "Message");

		// If user is online, flag 'em
		if ((touser = login_isloggedin(user)))
			touser->status |= ST_MESSAGE;

	}

}



void login_site_chgrp(struct login_node *t, char *args)
{

	struct group *curr;
	char *ar = args, *grp;

	consolef("site chgrp: %s\n", args);

	grp = digtoken((char **)&ar, " ");

	if (!grp) {  /* help on last */
		if (!t->current_group)
			socket_number(t, 280, NULL);
		else
			socket_number(t, 280, ((struct group *)t->current_group)->name);
    	return;
	}

	curr = group_find_byname(grp);

	if (!curr) {
		socket_number(t, 580, grp);
		return;
	}


	if (!group_hasuser_byname(grp, t->user)) {
		socket_number(t, 581, curr->name);
		return;
	}


	t->current_group = (void *) curr;
	group_member_setcurrent(curr, t->user);
	group_save();

	socket_number(t, 280, curr->name);

}


void login_site_setgroup(struct login_node *t, char *args)
{
  char *ar = args, *grp, *user;
  struct login_node *luser;

  /* group +user group add user */
  /* group -user group del user */

  consolef("site setgroup: %s\n", args);


  if (!((t->level|server_defaultflags)&UF_GROUP)) {
    socket_number(t, 599, "SETGROUP");
    return;
  }

  user = digtoken((char **)&ar, " ");
  grp = digtoken((char **)&ar, " ");

  if (!user || !grp) {
    help_commands(t, "SETGROU");
    return;
  }

  if (*user != '+' && *user != '-') {
    help_commands(t, "SETGROU");
    return;
  }

  if (!group_find_byname(grp)) {
    socket_number(t, 580, grp);
    return;
  }

  if (!(luser = user_findbyname(&user[1]))) {
    socket_number(t, 530, &user[1]);
    return;
  }

  if (*user == '+') {

    if (group_hasuser_byname(grp, &user[1])) {
      socket_number(t, 200, "SETGROUP");
      return;
    }

    //group_adduser(luser->user, grp);
    //socket_number(t, 290, grp);
	socket_print(t, "500 Not implemented\r\n");

    return;
  }

  if (!group_hasuser_byname(grp, &user[1])) {
    socket_number(t, 200, "SETGROUP");
    return;
  }

  //group_rmuser(luser->user, grp);

	socket_print(t, "500 Not implemented\r\n");
  //socket_number(t, 290, grp);

  return;
}




void login_site_gtop(struct login_node *t, char *args)
{
  char *ar = args, *type;
  int week;

  type = digtoken((char **)&ar, " ");

  if (!type || ((*type != 'a') && (*type != 'w'))) {
    help_commands(t, "GTOP");
    return;
  }

  week = (*type == 'a');

  /* Right, sort groups the right way. */


  //group_list(t, week);

  //socket_number(t, 200, "GTOP");
  socket_print(t, "500 No implemented\r\n");

}



void login_site_options(struct login_node *t, unsigned int flag, char *args)
{
  char *ar = args, *type;

  consolef("login_site_options: %d %s\n", flag, args);


  type = digtoken((char **)&ar, " ");

  if (!type) { /* toggle */

    t->options ^= flag;


  } else if (atoi(type)) { /* set on */


    t->options |= flag;


  } else { /* clear it */


    t->options &= ~flag;


  }


  consolef("Colour %d\n", t->options);
  socket_number(t, 200, NULL);

}



void login_site_xdupe(struct login_node *t, char *args)
{
  char *ar = args, *type;
  int mode;

  consolef("login_site_xdupe: %s %s\n", t->user, args);

  type = digtoken((char **)&ar, " ");

  if (!type) { /* toggle */

	  if (t->options & UO_XDUPE_2)
		  socket_print(t, "200 Extended dupe mode 2 is enabled.\r\n");
	  else if (t->options & UO_XDUPE_3)
		  socket_print(t, "200 Extended dupe mode 3 is enabled.\r\n");
	  else
		  socket_print(t, "200 Extended dupe mode is disabled.\r\n");

	  return ;

  }

  mode = atoi(type);

  switch(mode) {
  case 2:
	  t->options |= UO_XDUPE_2;
	  t->options &= ~UO_XDUPE_3;
	  break;
  case 3:
	  t->options |= UO_XDUPE_3;
	  t->options &= ~UO_XDUPE_2;
	  break;
  default:
	  t->options &= ~(UO_XDUPE_2|UO_XDUPE_3);
	  break;
  }

  socket_number(t, 200, NULL);

}




void login_site_new(struct login_node *t, char *args)
{

  consolef("login_site_new: %s\n", args);

  socket_number(t, 6500, NULL);

}


void login_site_chown(struct login_node *t, char *args)
{
  char *ar = args, *user, *dir;
  struct login_node *tu;

  consolef("login_site_chown: %s\n", args);

  if ((t->level & UF_ALL) != UF_ALL) {

    socket_number(t, 599, "chown");

    return;

  }

  if (!(user = digtoken((char **)&ar, " "))) {
    help_commands(t, "chown");
    return;
  }

  if (!(dir = digtoken((char **)&ar, " "))) {
    help_commands(t, "chown");
    return;
  }

  tu = user_findbyname(user);

  if (!tu) {
    socket_number(t, 530, user);
    return;
  }

#if 0
  if (!file_isdir(t, dir)) {

    socket_number(t, 550, dir);
    return 0;

  }
#endif

  sprintf(genbuf, "%s/%s/%s", server_usechroot, t->cwd, dir);
  file_undot(genbuf);

  consolef("Chowning %s\n", genbuf);

  file_chown(tu, genbuf);

  socket_number(t, 200, "chown");

}

void login_site_setpass(struct login_node *t, char *args)
{

	char *ar = args, *user, *newpass;
	struct login_node *tt;

	consolef("login_site_setpass: %s\n", args);


	if (!((t->level|server_defaultflags)&UF_PASSWORD)) {
		socket_number(t, 599, "SETPASS");
		return;
	}

	if (!(user = digtoken((char **)&ar, " "))
		|| !(newpass = digtoken((char **)&ar, " ")))
	{
		help_commands(t, "SETP");
		return;
	}


	// NO SUCH USER
	if ( !(tt = login_isloggedin(user)) && !(tt = user_findbyname(user))) {
		socket_number(t, 530, user);
		return;
	}


	strncpy(tt->pass, newpass, MAXPASSNAME);
	tt->pass[MAXPASSNAME] = 0;

	user_crypt(tt->pass, NULL);
	user_update(tt);

	socket_print(t, "200 Changed password for user '%s'\r\n", user);

	log_admin("%s changed %s's password\n", t->user, user);

}


void login_site_passwd(struct login_node *t, char *args)
{

	char *ar = args, *old, *new;

	consolef("login_site_passwd: %s\n", args);

	if (!(old = digtoken((char **)&ar, " "))
		|| !(new = digtoken((char **)&ar, " ")))
	{
		help_commands(t, "PAS2");
		return;
	}


	strcpy(genbuf, new);
	user_crypt(old, t->pass);

	if (strncmp(t->pass, old, MAXPASSNAME)) {
		socket_number(t, 530, NULL);
		return;
	}

	strncpy(t->pass, genbuf, MAXPASSNAME);
	t->pass[MAXPASSNAME] = 0;

	consolef("login_site_chpw() New passwd: %s\n", t->pass);

	user_crypt(t->pass, NULL);
	socket_number(t, 200, "PASWD");

}



void login_site_tagline(struct login_node *t, char *args)
{
  char *r;

  consolef("login_site_tagline(): Setting tag line %s\n", args);


  r = args;
  while(*r == ' ') r++;


  strncpy(t->tagline, r, MAXTAGLINE);
  t->tagline[MAXTAGLINE] = 0;

  socket_number(t, 200, "TAGLINE");

}



void login_site_wall(struct login_node *t, char *args)
{
  int i;

  consolef("login_site_wall(): %s\n", args);


  if (!*args || !args[1]) { /* List them */

    socket_number(t, 240, NULL);
    socket_number(t, 200, "WALL");

    return;

  }

  /* Truncate */
  while(*args == ' ') args++;


  if (strlen(args) > 70) args[70] = 0;

  /* if last position taken, free the first, move them all up */

  if (global_mwm[NUM_WALL - 1]) {

    if (global_mwm[0]) {

      free(global_mwm[0]);

    }

    for (i = 1; i < NUM_WALL; i++) {

        strcpy((char *)global_mwu[i - 1], (char *)global_mwu[i]);
      global_mwm[i - 1] = global_mwm[i];
      global_mwd[i - 1] = global_mwd[i];

    }

    strcpy((char *)global_mwu[NUM_WALL - 1], t->user);
    global_mwm[NUM_WALL - 1] = (unsigned char *)mystrcpy(args);
    time(&global_mwd[NUM_WALL - 1]);

  } else { /* There is a free node, find it... */

    for (i = 0; i < NUM_WALL; i++) {

      if (!global_mwm[i]) {
	strcpy((char *)global_mwu[i], t->user);
	global_mwm[i] = (unsigned char *)mystrcpy(args);
	time(&global_mwd[i]);

	break;

      }

    }

  }

#ifdef IRCBOT
  irc_announce("WALL|user=%s|msg=%s\n", t->user, args);
#endif


  file_writewall();


  socket_number(t, 200, "WALL");

}



void login_site_rehash(struct login_node *t, char *args)
{

  consolef("login_site_rehash(%s)\n", t->user);

  if ((t->level & UF_ALL) != UF_ALL) {

    socket_number(t, 599, "rehash");
    return;

  }

  server_rehash = 1;

#ifdef IRCBOT
	irc_announce("REHASH|user=%s\n", t->user);
#endif

  socket_number(t, 200, "REHASH initiated (Its use is discouraged as it is not fully implemented)");

}



void login_site_rules(struct login_node *t, char *args)
{

  consolef("login_site_rules(%s)\n", t->user);

  socket_number(t, 5200, NULL);

}





char *data_type(int type)
{
  static char dt[128];

  *dt = 0;

  if (type & DATA_LIST)      strcat(dt, "LIST");
  if (type & DATA_RETR)      strcat(dt, "RETR");
  if (type & DATA_STOR)      strcat(dt, "STOR");
  if (type & DATA_FILECHECK) strcat(dt, "CHEK");
  if (type & DATA_NLST)      strcat(dt, "NLST");
  if (type & DATA_PASSIVE)   strcat(dt, "p");
  if (type & DATA_FREE)      strcat(dt, "f");
  if (type & DATA_RENAME)    strcat(dt, "RENA");
  if (type & DATA_QUEUE)     strcat(dt, "q");

  return dt;

}


char *data_status(int status)
{
  static char dt[128];

  *dt = 0;

  if (status & ST_LOGIN)    strcat(dt, "LOGN");
  if (status & ST_NEWLOGIN) strcat(dt, "NEWL");
  if (status & ST_MESSAGE)  strcat(dt, "+m");
  //  if (status & ST_LISTEN)   strcat(dt, "LISN");
  //  if (status & ST_PENDING)  strcat(dt, "PEND");
  if (status & ST_ACTIVE)   strcat(dt, "ACTV");
  if (status & ST_AUTHOK)   strcat(dt, "+a");

  return dt;
}


void login_site_debug(struct login_node *t, char *args)
{
  char *ar = args, *user;
  struct quota_node *q;
  struct login_node *touser;

  consolef("login_site_debug(%s)\n", t->user);


  if (!(user = digtoken((char **)&ar, " "))) {
    help_commands(t, "DEBU");
    return;
  }


  /* See if the user is logged in, if so, display stats
   * if not, check to see if there are a quota node for them resident
   */

  if ((touser = login_isloggedin(user))) {

    q = (struct quota_node *)touser->quota;

    socket_print(t, "220-User %s is logged on, stats follow\r\n",
		 user);
    socket_print(t, "220-quotanode at %08x\r\n", q);
    socket_print(t, "220-ref count %d \r\n", q->ref_count);
    socket_print(t, "220-downloads %d uploads %d\r\n",
		 q->num_active_retr, q->num_active_stor);
    socket_print(t, "220 done.\r\n");

  }
}



void login_site_dupe(struct login_node *t, char *args)
{
#ifdef DUPECHECK
  int ret;
  struct dupe_node *dupe;



  while (*args == ' ') args++;


  if (!*args) {

    help_commands(t, "DUPE");
    return;

  }

  if (*args == '+') {  /* Add to dupe */

    if (!((t->level|server_defaultflags)&UF_DUPE)) {
      socket_number(t, 599, "DUPE");
      return;
    }

    args++;

    ret = dupe_add(args, "-", 0, global_time_now);

    if (!ret) {

      socket_number(t, 200, "DUPE");
      return;

    } else if (ret == 1) {

      socket_number(t, 561, args);
      return;

    } else {

      socket_number(t, 598, "DUPE add");
      return;

    }


  } else if (*args == '-') { /* Del from dupe */

    if (!((t->level|server_defaultflags)&UF_DUPE)) {
      socket_number(t, 599, "DUPE");
      return;
    }

    args++;

    ret = dupe_del(args);

    if (!ret) {

      socket_number(t, 200, "DUPE");
      return;

    }

    socket_number(t, 562, args);
    return;

  } else { /* Display in dupe */

    socket_number(t, 6300, NULL);

    dupe = dupe_find(args);

    if (!dupe) {

      /* Might be a wildacrd pattern, check... */
      if (strchr(args, '*') || strchr(args, '?') || strchr(args, '[') ||
	  strchr(args, '{')) {

	/* Fork here */
#ifdef WIN32
		  switch(0) {
#else
		  switch(fork()) {
#endif

	case 0: /* Child */
		//exec_clearsignal();

	  for (dupe = dupe_fnmatch(args); dupe; dupe = dupe_fnmatch(NULL)) {

	    socket_number(t, 6200, (char *)dupe);

	  }

	  /*socket_number(t, 200, "DUPE");*/
	  socket_number(t, 6400, NULL);
#ifndef WIN32
	  _exit(0);
#endif
	  return;

	case -1:
	  socket_number(t, 598, "fork failed");
	  return;

	default: /* Parent */
	  return;

	}

      }

      /* Couldn't find it, fail */
      /* Print couldn't find it here if you want */
      socket_number(t, 6400, NULL);
      /*socket_number(t, 562, args);*/
      return;

    }

    socket_number(t, 6200, (char *)dupe);
    socket_number(t, 6400, NULL);
    /*socket_number(t, 200, "DUPE");*/

    return;

  }


#endif

}




void login_site_setcred(struct login_node *t, char *args)
{
  char *ar = args, *user, *cred;
  lion64u_t amount = 0;
  struct login_node *touser;


  consolef("login_site_setcred(%s) %s\n", t->user, args);

  //socket_print(t, "500 NO! BAD! THIS NEEDS FIXING --- UNIMPLEMENTED\r\n");


  if (!((t->level|server_defaultflags)&UF_CREDITS)) {
    socket_number(t, 599, "CREDITS");
    return;
  }


  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETC");
    return;
  }


  if (!(cred = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETC");
    return;
  }


  touser = user_findbyname(user);

  if (!touser) {
    socket_number(t, 530, user);
    return;
  }


  amount = strtoull(*cred == '-' ? &cred[1] : cred, NULL, 10);

  // Make it into MB
  amount *= (lion64u_t) 1048576;

  /* Get the quota node */
  quota_new(touser);



  if (((struct quota_node *)touser->quota)->ratio <= 0) {
    socket_print(t, "200 %s has no ratio, ignored.\r\n",
		 touser->user);

    quota_exit(touser);
    return;
  }




  if ((*cred == '+')) {   /* add credits */


    ((struct quota_node *)touser->quota)->credits += amount;


    socket_print(t, "200 %s +%-5.0f MB credits\r\n",
		 touser->user,
		 (float) amount / 1048576.0);


  } else if ((*cred == '-')) {   /* remove credits */

	  consolef("vs %qd and %qd\n",
			   ((struct quota_node *)touser->quota)->credits,
			   amount);

	  if (((struct quota_node *)touser->quota)->credits < amount)
		  {

			  ((struct quota_node *)touser->quota)->credits = 0;

			  socket_print(t, "200 %s set to 0MB credits\r\n",
						   touser->user);

		  } else {


			  ((struct quota_node *)touser->quota)->credits -= amount;

			  socket_print(t, "200 %s -%-5.0f MB credits\r\n",
						   touser->user,
						   (float)amount / 1048576.0);

		  }


  } else {   /* Set credits */


	  ((struct quota_node *)touser->quota)->credits = amount;

	  socket_print(t, "200 %s credits set to %-5.0f MB\r\n",
				   touser->user,
				   (float)amount / 1048576.0);


  }



  ((struct quota_node *)touser->quota)->dirty = 1;


  quota_exit(touser);


}



void login_site_give(struct login_node *t, char *args)
{
  char *ar = args, *user, *cred;
  long int amount = 0;
  struct login_node *touser;

  consolef("login_site_give(%s) %s\n", t->user, args);

  // Unless we are admins of course.
  if (((struct quota_node *)t->quota)->ratio <= 0) {
    socket_print(t, "200 Disabled account may not GIVE credits\r\n");
    return;
  }


  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "GIVE");
    return;
  }

  if (!(cred = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "GIVE");
    return;
  }


  touser = user_findbyname(user);


  if (!touser) {
    socket_number(t, 530, user);
    return;
  }


  amount = atol(cred);

  if (amount <= 0) {
    socket_print(t, "200 Nice try, you can only give +'ve amounts\r\n");
    return;
  }


  if (((struct quota_node *)t->quota)->credits < (amount * 1024 * 1024)) {
    socket_print(t, "200 You do not have %ldMB credits to GIVE\r\n",
		 amount);
    return;
  }



  /* Get the quota node */
  quota_new(touser);



  if (((struct quota_node *)touser->quota)->ratio <= 0) {
    socket_print(t, "200 %s has unlimited ratio, ignored.\r\n",
		 touser->user);

    quota_exit(touser);
    return;
  }


  ((struct quota_node *)touser->quota)->credits += (amount * 1024 * 1024);
  ((struct quota_node *)touser->quota)->dirty = 1;
  ((struct quota_node *)t->quota)->credits -= (amount * 1024 * 1024);
  ((struct quota_node *)t->quota)->dirty = 1;

  quota_exit(touser);


  consolef("set_cred(%s): Changed %ldMB from user %s\n", t->user,
	   amount, touser->user);

  socket_print(t, "200 %s credits adjusted by %ldMB\r\n", touser->user,
	       amount);

}


void login_site_adduser(struct login_node *t, char *args)
{
  char *ar = args, *user, *pass, *patt;
  struct login_node *touser, neww;
  int i = 0;
  struct group *group_admin;

  consolef("login_site_adduser(%s) %s\n", t->user, args);

  group_admin = NULL;

  if (!((t->level|server_defaultflags)&UF_ADDUSER)) {

	  // Ok, lets check if they are GROUP_ADMINs
	  if (t->current_group && group_hasgadmin(t->current_group, t->user)) {

		  group_admin = (struct group *)t->current_group;
		  consolef("login_site_adduser(%s) is groupadmin of '%s'\n",
				   t->user,
				   group_admin->name);

	  } else {

		  socket_number(t, 599, "ADDUSER");
		  return;
	  }
  }

  // Enough slots?
  if (group_admin) {
	  if (group_admin->usedslots >= group_admin->totalslots) {
		  socket_number(t, 583, "slots");
		  return;
	  }
  }


  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "ADDU");
    return;
  }

  if (!(pass = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "ADDU");
    return;
  }


  touser = user_findbyname(user);
  if (touser) {
    socket_number(t, 567, user);
    return;
  }

  memset(&neww, 0, sizeof(neww));

  strncpy(neww.user, user, MAXUSERNAME);
  neww.user[MAXUSERNAME] = 0;


  strncpy(neww.pass, pass, MAXPASSNAME);
  neww.pass[MAXPASSNAME] = 0;

  user_crypt(neww.pass, NULL);

  if (server_usermatch)
    while((patt = digtoken((char **)&ar, " \r\n"))) {  /* We have a pattern */

    if (i >= IPMATCHMAX) {

      socket_print(t, "200-Excess pattern '%s' ignored (max %d)\r\n", patt,
		   IPMATCHMAX);
      continue;

    }

	// Check the patterns are valid if we are group_admin


    strncpy(neww.matchip[i], patt, IPMATCHLEN);

    neww.matchip[i][IPMATCHLEN-1] = (char) 0;

    i++;

    } /* while */


  neww.num_logins    = server_loginlimit;
  neww.num_uploads   = server_uploadlimit;
  neww.num_downloads = server_downloadlimit;

  if (!user_addnew(&neww)) {
    socket_print(t, "200 Failed to create user?\r\n");
    return;
  }


  quota_addnew(&neww, server_userquotaratio);
  quota_new(&neww);

  socket_print(t, "200-User '%s' created successfully.\r\n", neww.user);
  socket_print(t, "200-Limits:  logins=%d  upload=%d  download=%d  ratio=%d\r\n",
	       neww.num_logins, neww.num_uploads, neww.num_downloads,
	       server_userquotaratio);
  for (i = 0; i < IPMATCHMAX; i++)
    if (neww.matchip[i][0])
      socket_print(t, "200-Pattern %d : '%s'\r\n",
		   i+1, neww.matchip[i]);


  // If group_admin, force said user into group.
  if (group_admin) {

	  if (server_userquotaratio > group_admin->maxratio)
		  ((struct quota_node *)neww.quota)->ratio = group_admin->maxratio;

	  group_member_add(group_admin, neww.user);

	  neww.current_group = (void *) group_admin;
	  group_member_setcurrent(group_admin, neww.user);
	  group_save();

  }


  socket_number(t, 200, "adduser");

  quota_exit(&neww);

  log_admin("%s created account '%s' %s%s\n", t->user,
			user,
			group_admin ? "As group: " : "",
			group_admin ? group_admin->name : "");


}









void login_site_deluser(struct login_node *t, char *args)
{
  char *ar = args, *user;
  struct login_node *touser, *runn;
  struct group *g;
  struct group_member *gm;
  struct group *group_admin;

  group_admin = NULL;

  consolef("login_site_deluser(%s) %s\n", t->user, args);


  if (!((t->level|server_defaultflags)&UF_ADDUSER)) {

	  // Ok, lets check if they are GROUP_ADMINs
	  if (t->current_group && group_hasgadmin(t->current_group, t->user)) {

		  group_admin = (struct group *)t->current_group;
		  consolef("login_site_deluser(%s) is groupadmin of '%s'\n",
				   t->user,
				   group_admin->name);

	  } else {

		  socket_number(t, 599, "DELUSER");
		  return;
	  }
  }


  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "DELU");
    return;
  }


  // Make sure said user is in same group eh
  if (group_admin && !group_hasuser(group_admin, user)) {
	  socket_print(t, "500 User '%s' isn't in group '%s'\r\n",
				   user, group_admin->name);
	  return;
  }

  touser = user_findbyname(user);
  if (!touser) {
    socket_number(t, 568, user);
    return;
  }




  while ((runn = login_isloggedin_notus(user, t))) {

	  socket_number(runn, 569, t->user);
	  login_exit(runn);

  }

  while((g = group_findany_byname(user)) &&
		(gm = group_hasuser(g, user)))
	  group_member_del(g, gm);
  group_save();


  touser->user[0] = 0;
  user_update(touser);

  socket_number(t, 200, "deluser");


  log_admin("%s deleted account '%s' %s%s\n", t->user,
			user,
			group_admin ? "As group: " : "",
			group_admin ? group_admin->name : "");


}




void login_site_renuser(struct login_node *t, char *args)
{
	char *ar = args, *user, *newname;
	struct login_node *touser, *runn;

  consolef("login_site_renuser(%s) %s\n", t->user, args);


  if (!((t->level|server_defaultflags)&UF_ADDUSER)) {
    socket_number(t, 599, "RENUSER");
    return;
  }


  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "RENU");
    return;
  }

  if (!(newname = digtoken((char **)&ar, " \r\n"))) {
	  help_commands(t, "RENU");
	  return;
  }


  touser = user_findbyname(newname);
  if (touser) {
    socket_number(t, 567, newname);
    return;
  }

  touser = user_findbyname(user);
  if (!touser) {
    socket_number(t, 568, user);
    return;
  }

  strncpy(touser->user, newname, MAXUSERNAME);
  touser->user[MAXUSERNAME] = 0;


  while ((runn = login_isloggedin_notus(user, t))) {

	  socket_number(runn, 569, t->user);
	  login_exit(runn);

  }


  // Race, if they are logged on...
  user_update(touser);

  socket_number(t, 200, "renuser");


  log_admin("%s renamed account '%s' to '%s'\n", t->user,
	   user, newname);


}











void login_site_setip(struct login_node *t, char *args)
{
	char *ar = args, *user, *num, *patt;
	struct login_node *tt;
	int i = 0;
	struct group *group_admin;

	group_admin = NULL;

	consolef("login_site_setip(%s) %s\n", t->user, args);


	if (!((t->level|server_defaultflags)&UF_SETIP)) {

	  // Ok, lets check if they are GROUP_ADMINs
	  if (t->current_group && group_hasgadmin(t->current_group, t->user)) {

		  group_admin = (struct group *)t->current_group;
		  consolef("login_site_setip(%s) is groupadmin of '%s'\n",
				   t->user,
				   group_admin->name);

	  } else {

		socket_number(t, 599, "SETIP");
		return;

	  }

	}


	if (!(user = digtoken((char **)&ar, " \r\n"))) {
		help_commands(t, "SETI");
		return;
	}

	if (!(num = digtoken((char **)&ar, " \r\n"))) {
		help_commands(t, "SETI");
		return;
	}

	if (!(patt = digtoken((char **)&ar, " \r\n"))) {
		help_commands(t, "SETI");
		return;
	}

	i = atoi(num);
	i--;

	if ((i < 0) || (i >= IPMATCHMAX)) {
		socket_print(t, "200 Invalid pattern number %d (1<=x<=%d)\r\n",
					i+1, IPMATCHMAX);
		return;
	}


	if (!(tt = login_isloggedin(user)) && !(tt = user_findbyname(user))) {
		socket_number(t, 530, user);
		return;
	}

	if (group_admin) {
		// Make sure said user is in same group eh
		if (!group_hasuser(group_admin, user)) {
			socket_print(t, "500 User '%s' isn't in group '%s'\r\n",
						 user, group_admin->name);
			return;
		}
	}

	strncpy(tt->matchip[i], patt, IPMATCHLEN);
	tt->matchip[i][IPMATCHLEN-1] = (char) 0;

	user_update(tt);

	socket_print(t, "200 User '%s' pattern %d set to '%s'\r\n",
					user, i+1, patt);

	log_admin("%s set %s's %d%s pattern to %s\n", t->user,
					user, i + 1, postnumber(i+1), patt);

}



//
// arg1 is struct
// arg2 is char *user
//
// Type 0 is setlimit logins
// Type 1 is setlimit downloads
// Type 2 is setlimit uploads
// Type 3 is setratio
// Type 4 is setlevel


struct login_site_setlimit_struct {
	int type;
	int i;
	int dirty;
};

int login_site_setlimit_sub(lion_t *handle, void *arg1, void *arg2)
{
	struct login_site_setlimit_struct *data =
		(struct login_site_setlimit_struct *) arg1;
	struct login_node *p;

	// NOT LOGIN NODE
	if (lion_get_handler(handle) != login_handler)
		return 1;

	// Grab login_node data for this handle
	p = (struct login_node *) lion_get_userdata( handle );

	// The man we're looking for?
	if (p && !strcasecmp(p->user, (char *)arg2)) {

		switch(data->type) {
		case 0:
			p->num_logins = data->i;
			break;
		case 1:
			p->num_downloads = data->i;
			break;
		case 2:
			p->num_uploads = data->i;
			break;
		case 3:
			if (p->quota) {
				((struct quota_node *)p->quota)->ratio = data->i;
			}
			break;
		case 4:
			p->level = data->i;
			break;
		}

		// Sync the first hit to disk
		if (!data->dirty) {
			user_update(p);
		}

		data->dirty = 1;

	}

	// Keep searching...
	return 1;
}





void login_site_setratio(struct login_node *t, char *args)
{
  char *ar = args, *user, *num;
  struct login_node *tt;
  int i = 0;
  struct login_site_setlimit_struct data;
  struct group *group_admin;
  struct group_member *gm;

  group_admin = NULL;

  consolef("login_site_setratio(%s) %s\n", t->user, args);


  if (!((t->level|server_defaultflags)&UF_SETRATIO)) {

	  // Ok, lets check if they are GROUP_ADMINs
	  if (t->current_group && group_hasgadmin(t->current_group, t->user)) {

		  group_admin = (struct group *)t->current_group;
		  consolef("login_site_setratio(%s) is groupadmin of '%s'\n",
				   t->user,
				   group_admin->name);

	  } else {

		  socket_number(t, 599, "SETRATIO");
		  return;
	  }
  }

  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETR");
    return;
  }

  if (!(num = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETR");
    return;
  }


  if (!strncasecmp("unlim", num, 5) || !strncasecmp("leech", num, 5))
	  i = -1;
  else
	  if (!strncasecmp("guest", num, 5))
		  i = 0;
	  else
		  i = atoi(num);


  // Group_admin? Enough sluts or, too high ratio?
  if (group_admin) {


	  // Make sure said user is in same group eh
	  if (!(gm = group_hasuser(group_admin, user))) {
		  socket_print(t, "500 User '%s' isn't in group '%s'\r\n",
					   user, group_admin->name);
		  return;
	  }


	  if (i == -1) {
		  if (group_admin->usedleechslots >= group_admin->leechslots) {
			  socket_number(t, 583, "leechslots");
			  return;
		  }

		  // Find groupmember, if we _change_ status, update counter and
		  // set the _group_ ratio
		  if (gm->ratio != -1)
			  group_admin->usedleechslots++;
		  gm->ratio = -1;
		  group_save();

	  } else { // Not unlim

		  if (group_admin->maxratio < i) {
			  socket_number(t, 583, "bigger than max ratio");
			  return;
		  }

		  // Was it unlim before?
		  if (gm->ratio == -1)
			  group_admin->usedleechslots--;
		  gm->ratio = i;
		  group_save();

	  }
  }




  data.i = i;
  data.type = 3;
  data.dirty = 0;
  lion_find( login_site_setlimit_sub, (void *)&data, (void *)user);



  if (!data.dirty) {  /* Crap they were not logged in, so read it off disk */

    tt = user_findbyname(user);

    if (!tt) {
      socket_number(t, 530, user);
      return;
    }

    quota_new(tt);

    if (tt->quota) {
      ((struct quota_node *)tt->quota)->ratio = i;
      ((struct quota_node *)tt->quota)->dirty = 1;
    }

    quota_exit(tt);

    user_update(tt);

  }


  if (i <= 0) {
    socket_print(t, "200 User '%s' ratio set to: %s.\r\n",
		 user, i == 0 ? "guest" : "unlimited");
  } else {
    socket_print(t, "200 User '%s' ratio set to 1:%d\r\n",
		 user, i);
  }


  log_admin("%s set %s's ratio to %d\n", t->user,
	   user, i);



}





void login_site_setlimit(struct login_node *t, char *args)
{
  char *ar = args, *user, *num, *lim;
  struct login_node *tt;
  int i = 0;
  int type = 0;
  struct login_site_setlimit_struct data;

  consolef("login_site_setlimit(%s) %s\n", t->user, args);


  if (!((t->level|server_defaultflags)&UF_SETLIMIT)) {
    socket_number(t, 599, "SETLIMIT");
    return;
  }

  if (!(user = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETL");
    return;
  }

  if (!(lim = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETL");
    return;
  }

  if (!(num = digtoken((char **)&ar, " \r\n"))) {
    help_commands(t, "SETL");
    return;
  }


  i = atoi(num);

  if (!strncasecmp("login", lim, 5)) {

    type = 0;

    if (!strncasecmp("disable", num, 7))
      i = 0;
    else if (!strncasecmp("enable", num, 6))
      i = server_loginlimit;

  } else if (!strncasecmp("down", lim, 4)) {

    type = 1;

    if (!strncasecmp("def", num, 3))
      i = server_downloadlimit;

  } else if (!strncasecmp("up", lim, 2)) {

    type = 2;

    if (!strncasecmp("def", num, 3))
      i = server_uploadlimit;

  } else {

    help_commands(t, "SETL");
    return;

  }


  // For every node logged in, modify

  // lion needs a way to iterate in-code without need to
  // ff to last position.
  data.i = i;
  data.type = type;
  data.dirty = 0;
  lion_find( login_site_setlimit_sub, (void *)&data, (void *)user);

  if (!data.dirty) {  /* Crap they were not logged in, so read it off disk */

	  tt = user_findbyname(user);

	  if (!tt) {
		  socket_number(t, 530, user);
		  return;
	  }

	  switch(type) {
	  case 0:
		  tt->num_logins = i;
		  break;
	  case 1:
		  tt->num_downloads = i;
		  break;
	  case 2:
		  tt->num_uploads = i;
		  break;
	  }

	  user_update(tt);

  }


  socket_print(t, "200 User '%s' %s limit set to %d\r\n",
			   user, lim, i);

  log_admin("%s changed %s's %s limit to %d\n", t->user,
		   user, lim, i);

}




void login_site_setflags(struct login_node *t, char *args)
{
	char *ar = args, *user, *num, *str;
	struct login_node *tt;
	int i = 0;
	struct login_site_setlimit_struct data;


	consolef("login_site_setflags(%s) %s\n", t->user, args);


	if (!((t->level|server_defaultflags)&UF_SETFLAGS)) {
		socket_number(t, 599, "SETFLAGS");
		return;
	}

	if (!(user = digtoken((char **)&ar, " \r\n"))) {
		help_commands(t, "SETF");
		return;
	}

	tt = user_findbyname(user);
	if (!tt) {
		socket_number(t, 530, user);
		return;
	}

	if (!(num = digtoken((char **)&ar, " \r\n"))) {
		// Display current flags.
		str = flagstoa(tt->level);
		socket_print(t, "200 User '%s' userflags are: %s.\r\n",
					 tt->user,
					 str);
		return;
	}

	// We use "+FLAGS" to add new flags, or
	// "-FLAGS" to remove them.
	if (*num == '+') {
		// Add flags
		i  = tt->level;
		i |= atoflags(&num[1]);
	} else if (*num == '-') {
		// Remove flags
		i  = tt->level;
		i &= ~atoflags(&num[1]);
	} else {
		// Set flags
		i = atoflags(num);
	}

	data.i = i;
	data.type = 4;
	data.dirty = 0;
	lion_find( login_site_setlimit_sub, (void *)&data, (void *)user);

	if (!data.dirty) {  /* Crap they were not logged in, so read it off disk */

		tt->level = i;

		user_update(tt);

	}


	str = flagstoa(tt->level);

    socket_print(t, "200 User '%s' userflags are now: %s.\r\n",
				 tt->user, str);

	log_admin("%s set %s's userflags to %s (%04X)\n", t->user,
			 user, str, tt->level);

}








void login_site_uptime(struct login_node *t, char *args)
{
  consolef("login_site_uptime(%s) %s\n", t->user, args);

  socket_print(t, "200 Uptime: %s  Users: %d  Xfers: %d\r\n",
	       idletime(lion_global_time - server_uptime),
	       server_num_logins,
	       server_num_data);
}





void login_site_touch(struct login_node *t, char *args)
{
	//char *ar = args, *user, *dir;
  //struct login_node *tu;

  consolef("login_site_touch: %s\n", args);

  while(*args == ' ') args++;

  file_undot(args);

#if 0
  if (file_pathchecklogin(t, args)) {

    consolef("touching 2%s\n", args);

    //file_chown(tu, genbuf);

    socket_number(t, 200, "TOUCH");


  }
#endif
  consolef("touching %s\n", args);

  //file_chown(tu, genbuf);

  socket_number(t, 200, "TOUCH");

}



#ifdef BUSY
void login_site_ticks(struct login_node *t, char *args)
{
  consolef("login_ticks(%s)\n", t->user);

  io_ticks(t);

}
#endif



int site_tcpstat_sub( lion_t *handle, void *arg1, void *arg2 )
{
	struct login_node *user = (struct login_node *) arg1;
	struct login_node *p = NULL;
	struct data_node *d = NULL;

	if (!handle) // Shouldn't happen!
		return 1;

	// Check its a login node
	if (lion_get_handler(handle) != login_handler)
		return 1;

	if (!user || !user->handle) return 0;

	p = (struct login_node *) lion_get_userdata( handle );

	if (!p) return 1;

	lion_printf(user->handle, "200-%-8s  %16s:%-5d  %8s %2d %s",
				p->user, lion_ntoa(p->host), p->port, p->ident,
				p->data_active, ctime(&p->time));

	/* For all data-nodes, list... */
	for (d = (struct data_node *)p->data; d; d = d->next) {

		lion_printf(user->handle,
					"200-        %-8s (%3d<->%-3d = %-8s) "
					"%"PRIu64" bytes : %s\r\n",
					data_type(d->type),
					d->handle, d->althandle,
					data_status(d->status), d->bytes,
					d->name && *d->name ? d->name : "N/A");

	}


	return 1;

}




void login_site_tcpstat(struct login_node *t, char *args)
{

	time_t tt = time(NULL);
	//struct login_node *p;
	//	struct data_node *d;
	//global_update_daily();

	/* WHO command */
	if (!((t->level|server_defaultflags)&UF_STAT)) {
		socket_number(t, 599, "TCPSTAT");
		return;
	}

	/* 200- */
	socket_print(t, "200-Users currently logged on at %s", ctime(&tt));

	socket_print(t, "200-%-8s  %16s:%-5s  %8s %2s %s",
		"Usercode", "Internet IP", "port", "ident",
		"#D", "on since\r\n");
	socket_print(t, "200-        datatype (fd1<->fd2 = status) bytes : dataname\r\n");


	// Start listing.
	lion_find( site_tcpstat_sub, (void *)t, NULL);


#if 0  // FIXME LION

	for (p = login_node_head; p; p=p->next) {
		socket_print(t, "200-%-8s  %16s:%-5d  %8s %2d %s",
					p->user, ul2a(p->host), p->port, p->ident,
					p->data_active, ctime(&p->time));

		/* For all data-nodes, list... */
		for (d = (struct data_node *)p->data; d; d = d->next) {

			socket_print(t,
				"200-        %-8s (%3d<->%-3d = %-8s) "
					"%"PRIu64" bytes : %s\r\n",
				data_type(d->type),
				d->handle, d->althandle,
				data_status(d->status), d->bytes,
				d->name && *d->name ? d->name : "N/A");

		}


	}
#endif

	socket_print(t, "200  (end of 'tcpstat')\r\n");

}




int site_rescan_sub(const char *name, void *arg)
{
	struct data_node dummy;

	consolef("[sub] called for '%s'\n", name);

	memset(&dummy, 0, sizeof(dummy));

	dummy.name = (char *)name;

	check_filebad(&dummy);

	return 1;
}





//
// Site rescan <arg>
//
// Where arg is a filename, or wildcard pattern of items to retest.
//
// If you supply to arguments it will dump testing statistics.
//
void login_site_rescan(struct login_node *t, char *args)
{
	char *pat, *ar;
	int count;

	snprintf(genbuf, sizeof(genbuf), "%s/%s", server_usechroot, t->cwd);

	if (!file_pathchecklogin(t, genbuf)) {
		socket_number(t, 550, NULL);
		return;
	}

	lion_printf(t->handle, "200-SITE ReScan\r\n");

	ar = args;

	check_stats(t);

	if (!(pat = digtoken((char **)&ar, " \r\n"))) {

		// No arg, send stats.

		lion_printf(t->handle, "200 SITE ReScan\r\n");
		return;

	}

	consolef("[site] rescan '%s'\n", pat);



	// Currently this doesnt handle "dir/*" just assumes CWD. Fixme?
	count = fnmatch_func(pat,
						 genbuf,
						 site_rescan_sub,
						 FNMATCH_FILESONLY, NULL);

	if (count >= 0)
		lion_printf(t->handle, " Re-added %d item%s to be re-checked.\r\n",
					count, count == 1 ? "" : "s");

	lion_printf(t->handle, "200 SITE ReScan\r\n");

}



int site_request_sub(const char *name, void *arg)
{
	struct login_node *t = (struct login_node *)arg;
	struct stat sb;
	char *req;

	req = path2file((char *)name);
	if (!strncmp("REQUEST-", req, 8))
		req = &req[8];

	if (!stat(name, &sb)) {

		socket_print(t, " '%s' by %s created %s ago.\r\n",
					 req,
					 user_get_username(sb.st_uid),
					 idletime(lion_global_time - sb.st_mtime));

	} else {

		socket_print(t, " '%s'\r\n",
					 req);
	}

	return 1;
}


void login_site_request(struct login_node *t, char *args)
{
	char *ar = args, *dir, *section;
	int count = 0;

	consolef("login_site_request(%s) %s\n", t->user, args);


	// Locate the request section. First check if CWD is
	// a request area, if not, locate the first section with request
	// enabled in a multi-request area system.
	section = section_request(t->cwd);

	if (!section) {
		socket_print(t, "500 No REQUEST section defined on this site.\r\n");
		return;
	}

	consolef("Picked section '%s'\n", section);


	// If no arguments, lists current requests.
	// If arguments, create a new request.

	if (!(dir = digtoken((char **)&ar, " \r\n"))) {
		// List

		snprintf(genbuf, sizeof(genbuf), "%s/", section);

		if (!file_pathchecklogin(t, genbuf)) {
			socket_number(t, 550, section);
			return;
		}

		socket_print(t, "200-Currently active requests in '%s'...\r\n",
					 section);

		count = fnmatch_func("REQUEST-*",
							 (char *)genbuf,
							 site_request_sub,
							 FNMATCH_DIRSONLY,
							 (void *) t);


		socket_number(t, 200, "REQUEST");
		return;
	}

	// Check if it already exists?
	snprintf(genbuf, sizeof(genbuf), "/%s/REQUEST-%s", section, dir);

	consolef("Full path '%s'\n", genbuf);

	// mkdir sends reply code, so it needs to be last.
	file_mkdir(t, genbuf);

	return;
}


int site_fillrequest_sub(const char *name, void *arg)
{
	return 1;
}

int site_fillrequest_dirs(const char *path, void *arg)
{
	int count = 0;

	consolef("[fillrequest] Is '%s' incomplete?\n", path);

	count = fnmatch_func("-*_*INCOMPLETE_*_OF_*_FILES_*-",
						 (char *)path,
						 site_fillrequest_sub,
						 FNMATCH_FILESONLY, NULL);
	if (count) return count;

	count = fnmatch_func("*",
						 (char *)path,
						 site_fillrequest_dirs,
						 FNMATCH_DIRSONLY, NULL);
	return count;

}

void login_site_fillrequest(struct login_node *t, char *args)
{
	char *ar = args, *dir, *section, path[1024];
	int count = 0;

	consolef("login_site_fillrequest(%s) %s\n", t->user, args);

	if (!(dir = digtoken((char **)&ar, " \r\n"))) {
		help_commands(t, "FILL");
		return;
	}

	// Locate the request section. First check if CWD is
	// a request area, if not, locate the first section with request
	// enabled in a multi-request area system.
	section = section_request(t->cwd);

	if (!section) {
		socket_print(t, "500 No REQUEST section defined on this site.\r\n");
		return;
	}

	// Check if it already exists?
	if (!strncasecmp("REQUEST-", dir, 8))
		snprintf(genbuf, sizeof(genbuf), "/%s/%s", section, dir);
	else
		snprintf(genbuf, sizeof(genbuf), "/%s/REQUEST-%s", section, dir);

	if (!file_pathchecklogin(t, genbuf)) {
		socket_number(t, 550, dir);
		return;
	}

	consolef("Full path '%s'\n", genbuf);

	// Does the directory exist?
	if (!file_num_dir_entries(genbuf, ".", NULL)) {
		socket_number(t, 550, dir);
		return;
	}

	// Is it complete?
	count = site_fillrequest_dirs(genbuf,NULL);

	if (count > 0) {
		socket_print(t, "500 Request '%s' is listed as incomplete. (%d)\r\n",
					 dir, count);
		return;
	}

	socket_print(t, "200-Request appears to be complete.\r\n");

	// Skip the REQUEST- part so it only says FILLED-dir
	if (!strncasecmp("REQUEST-", dir, 8))
		dir += 8;

	snprintf(path, sizeof(path), "/%s/FILLED-%s", section, dir);
	if (!file_pathchecklogin(t, path)) {
		socket_number(t, 550, dir);
		return;
	}

	file_goroot();
	count = rename(genbuf, path);
	file_gononroot();

	if (count) {
		socket_number(t, 550, dir);
		return;
	}

#ifdef IRCBOT
	if (section_announce(path)) {
		irc_announce("FILLED|section=%s|user=%s|dir=%s\n",
					 section_name(path), t->user, section_rlsfind(path));
	}
#endif

	socket_number(t, 200, "REQFILLED");
	return;
}



void login_site(struct login_node *t, char *args)
{

	consolef("login_site(%p): %s\n", t->handle, args);

	// GROUP FUNCTIONS
	if (!strncasecmp("GROUPLIST", args, 9)) {

		group_site_list(t, &args[9]);

	} else if (!strncasecmp("GROUPINFO", args, 9)) {

		group_site_info(t, &args[9]);

	} else if (!strncasecmp("GROUPADD", args, 8)) {

		group_site_add(t, &args[8]);

	} else if (!strncasecmp("GROUPMOD", args, 8)) {

		group_site_mod(t, &args[8]);

	} else if (!strncasecmp("GROUPDEL", args, 8)) {

		group_site_del(t, &args[8]);

	} else if (!strncasecmp("GROUPUSER", args, 9)) {

		group_site_user(t, &args[9]);

	} else if (!strncasecmp("GROUPADMIN", args, 10)) {

		group_site_admin(t, &args[10]);

	} else if (!strncasecmp("GADDUSER", args, 8)) {

		group_site_adduser(t, &args[8]);

	// MISC. FUNCTIONS
	} else if (!strncasecmp("INCOMPLETES", args, 11)) {

		sfv_site_incompletes(t, &args[11]);

	} else if (!strncasecmp("SECTION", args, 7)) {

		section_site_section(t, &args[7]);

  } else if (!mystrccmp("TCPSTAT", args)) {

	login_site_tcpstat(t, args);

  } else if (!mystrccmp("WHO", args)) {

    local_site_who(t);

  } else if (!strncasecmp("HELP", args, 4)) {

    if (strlen(args) > 4) {
      help_commands(t, &args[4]);
      return;
    }

    socket_number(t, 214, NULL);

  } else if (!strncasecmp("NUKE", args, 4)) {

    login_site_nuke(t, &args[4]);

  } else if (!strncasecmp("UNNUKE", args, 6)) {

    login_site_unnuke(t, &args[6]);

  } else if (!strncasecmp("USER", args, 4)) {

    user_site_user(t, &args[4]);

  } else if (!strncasecmp("TOPUP", args, 5)) {

    login_site_topup(t, &args[5]);

  } else if (!strncasecmp("TOPDN", args, 5)) {

    login_site_topdown(t, &args[5]);

  } else if (!strncasecmp("MSG", args, 3)) {

    login_site_msg(t, &args[3]);

  } else if (!strncasecmp("KICK", args, 4)) {

    login_site_kick(t, &args[4]);

  } else if (!strncasecmp("CHGRP", args, 5)) {

    login_site_chgrp(t, &args[5]);

  } else if (!strncasecmp("SETGROUP", args, 8)) {

    login_site_setgroup(t, &args[8]);

  } else if (!strncasecmp("EXTRA", args, 5)) {

    login_site_options(t, UO_FANCY, &args[5]);

  } else if (!strncasecmp("COLOUR", args, 6)) {

    login_site_options(t, UO_COLOUR, &args[6]);

  } else if (!strncasecmp("COLOR", args, 5)) {

    login_site_options(t, UO_COLOUR, &args[5]);

  } else if (!strncasecmp("XDUPE", args, 5)) {

    login_site_xdupe(t, &args[5]);

  } else if (!strncasecmp("NEW", args, 3)) {

    login_site_new(t, &args[3]);

  } else if (!strncasecmp("GTOP", args, 4)) {

    login_site_gtop(t, &args[4]);

  } else if (!strncasecmp("chown", args, 5)) {

    login_site_chown(t, &args[5]);

  } else if (!strncasecmp("PASSWD", args, 6)) {

    login_site_passwd(t, &args[6]);

  } else if (!strncasecmp("SETPASS", args, 7)) {

    login_site_setpass(t, &args[7]);

  } else if (!strncasecmp("TAGLINE", args, 7)) {

    login_site_tagline(t, &args[7]);

  } else if (!strncasecmp("WALL", args, 4)) {

    login_site_wall(t, &args[4]);

  } else if (!strncasecmp("REHASH", args, 6)) {

    login_site_rehash(t, &args[6]);

  } else if (!strncasecmp("DEBUG", args, 5)) {

    login_site_debug(t, &args[5]);

#ifdef DUPECHECK
  } else if (!strncasecmp("DUPE", args, 4)) {

    login_site_dupe(t, &args[4]);

#endif
  } else if (!strncasecmp("SETCRED", args, 7)) {

    login_site_setcred(t, &args[7]);

  } else if (!strncasecmp("ADDUSER", args, 7)) {

    login_site_adduser(t, &args[7]);

  } else if (!strncasecmp("DELUSER", args, 7)) {

    login_site_deluser(t, &args[7]);

  } else if (!strncasecmp("RENUSER", args, 7)) {

    login_site_renuser(t, &args[7]);

  } else if (!strncasecmp("SETIP", args, 5)) {

    login_site_setip(t, &args[5]);

  } else if (!strncasecmp("SETRATIO", args, 8)) {

    login_site_setratio(t, &args[8]);

  } else if (!strncasecmp("SETLIMIT", args, 8)) {

    login_site_setlimit(t, &args[8]);

  } else if (!strncasecmp("SETFLAGS", args, 8)) {

    login_site_setflags(t, &args[8]);

  } else if (!strncasecmp("UPTIME", args, 6)) {

    login_site_uptime(t, &args[6]);

  } else if (!strncasecmp("GIVE", args, 4)) {

    login_site_give(t, &args[4]);

  } else if (!strncasecmp("TOUCH", args, 5)) {

    login_site_touch(t, &args[5]);

  } else if (!strncasecmp("RESCAN", args, 6)) {

    login_site_rescan(t, &args[6]);

  } else if (!strncasecmp("REQUEST", args, 7)) {

    login_site_request(t, &args[7]);

  } else if (!strncasecmp("REQFILLED", args, 9)) {

    login_site_fillrequest(t, &args[9]);

#ifdef RACESTATS
  } else if (!strncasecmp("RACES", args, 6)) {

    race_show(t, &args[6]);
#endif

#ifdef BUSY
  } else if (!strncasecmp("TICKS", args, 5)) {

    login_site_ticks(t, &args[5]);
#endif

		} else {

			// Attempt to run a SITECUST command, otherwise print error.
			if (sitecust_site_do(t, args) != 0)
				socket_number(t, 500, args);

		}

}
