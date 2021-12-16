/*
 * Copyright (C) 1995-2003 Jorgan Lundman
 * Copyright (C) 2003 friar@drunkmonk.net
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

#include "misc.h"
#include "global.h"
#include "log.h"
#include "login.h"
#include "socket.h"
#include "userflag.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "user.h"
#include "groups.h"
#include "dbutil.h"
#include "section.h"

static struct group *groups_head = NULL;
static unsigned long group_this_month = 0L;
static unsigned long group_this_week  = 0L;
static unsigned long group_this_day   = 0L;


//-------------------------------------------------------------------
// BASIC GROUP FUNCTIONS

void group_load(void)
{

	FILE *fp;
	struct group *g;
	struct group_member *gm;
	int gcnt = 0, gmcnt = 0, i;


	if (!server_groupfile)
		return;

	// Fill out the global group_this_* fields.
	group_time_init();

	consolef("[groups]: Loading '%s'...\n", server_groupfile);

	// Open file
	if ( ( fp = fopen(server_groupfile, "r+b")) == NULL) {
		consolef("[groups]: Unable to load, error opening '%s': %s\n",
				server_groupfile, strerror(errno));
		return;
	}


	db_check_group((unsigned long)0, fileno(fp));


	groups_head = NULL; // XXX memory leak, shouldn't happen.


	while (1) {

		if ( (g = malloc(sizeof(struct group))) == NULL) {
			perror("malloc");
			abort();
		}

		// Read a group entry.
		if (fread(g, sizeof(struct group), 1, fp) != 1 ) {

			free(g);

			if (feof(fp) || ferror(fp))
				break;

			consolef("[groups]: Corrupt groupfile: %s\n",
					strerror(errno));
			break;
		}

		// NULL pointers read from file.
		g->members = NULL;

		// Add to global list.
		g->next = groups_head;
		groups_head = g;
#ifdef DEBUG
		consolef("[groups]: Loaded group '%s' from disk.\n", g->name);
#endif

		gcnt++;

		// Read members now.
		for (i = 0; i < g->usedslots; i++) {

			if ( (gm = malloc(sizeof(struct group_member))) == NULL) {
				perror("malloc");
				abort();
			}

			// Read a group_member entry.
			if (fread(gm, sizeof(struct group_member), 1, fp) != 1 ) {
				free(gm);

				if (feof(fp) || ferror(fp))
					break;

				consolef("[groups]: Unable to load, error reading: %s\n",
					strerror(errno));
				break;
			}

			// Add member into group's linked list.
			gm->next = g->members;
			g->members = gm;
#ifdef DEBUG
			consolef("[groups]: Loaded user %s@%s from disk.\n",
					gm->name, g->name);
#endif

			gmcnt++;

		}

		g->usedleechslots = group_count_ratio(g, -1);
		group_auto_section(NULL, g->name);


	}

	fclose(fp);

	consolef("[groups]: Loaded %d groups and %d members from disk!\n",
					gcnt, gmcnt);

}

void group_save(void)
{

	FILE *fp;
	struct group *g;
	struct group_member *gm;

	if (!server_groupfile)
		return;

	// Open the group file for writing, truncate to 0.
	if ( ( fp = fopen(server_groupfile, "w+b")) == NULL) {
		consolef("[groups]: Unable to save, error opening '%s': %s",
				server_groupfile, strerror(errno));
		return;
	}


	if (fwrite(GROUPS_VERSION_STR, 4, 1, fp) != 1) {
		consolef("[groups]: Unable to save, writing header '%s': %s",
				 server_groupfile, strerror(errno));
		return;
	}


	// Cycle groups.
	for (g = groups_head; g; g = g->next) {

		// Use current m/w/d to keep track of last update to group stats.
		g->last_month = group_this_month;
		g->last_week = group_this_week;
		g->last_day = group_this_day;

		if (fwrite(g, sizeof(struct group), 1, fp) != 1) {
			fclose(fp);
			consolef("[groups]: Unable to save, error writing: %s",
				strerror(errno));
			return;
		}

		// Cycle users.
		for (gm = g->members; gm; gm = gm->next) {


			if (fwrite(gm, sizeof(struct group_member), 1, fp) != 1) {
				fclose(fp);
				consolef("[groups]: Unable to save, error writing: %s",
					strerror(errno));
				return;
			}

		}

	}

	fclose(fp);

}

void group_time_init(void)
{

	time_t now;
	struct tm *tmm;
	char genbuf[32];

    /* setup this_month */
    time(&now);
    tmm = localtime(&now);
    snprintf(genbuf, 31, "%02d%02d", tmm->tm_mon+1, tmm->tm_year);
    group_this_month = atol(genbuf);

    /* setup this_week */
    time(&now);
    tmm = localtime(&now);
    /* make it sunday */
    now -= tmm->tm_wday * 60 * 60 * 24;
    tmm = localtime(&now);
    snprintf(genbuf, 31, "%02d%02d%02d", tmm->tm_mday,
							tmm->tm_mon+1, tmm->tm_year);
    group_this_week = atol(genbuf);

    /* setup this_month */
    time(&now);
    tmm = localtime(&now);
    /* make it start of the day */
    now -= tmm->tm_hour * 60 * 60;
    tmm = localtime(&now);
    snprintf(genbuf, 31, "%02d%02d%02d", tmm->tm_mday,
							tmm->tm_mon+1, tmm->tm_year);
    group_this_day = atol(genbuf);

}


// Returns 0 on no error, -1 if the group cannot be added.
int group_add(char *group, int tslots, int lslots, int maxlog,
											int maxrat, int idle)
{

	struct group *g;

	// See if the group exists already.
	if (group_find_byname(group))
		return -1;

	// Malloc new group
	if ( (g = malloc(sizeof(struct group))) == NULL) {
		perror("malloc");
		abort();
	}
	bzero(g, sizeof(struct group));

	// Copy info over
	strncpy(g->name, group, MAXUSERNAME-1);
	g->gid = group_newgid();
	g->leechslots = lslots;
	g->totalslots = tslots;
	g->maxlogins = maxlog;
	g->maxratio = maxrat;
	g->idle = idle;

	// Add into head
	g->next = groups_head;
	groups_head = g;

	// Save group to disk
	group_save();



	return 0;

}

void group_del(struct group *g)
{

	struct group *grun;
	struct group_member *gm, *gmnext;

	consolef("[groups]: Deleting group '%s'... ", g->name);

	// Remove each user from the group's list.
	for (gm = g->members; gm; gm = gmnext) {
		gmnext = gm->next;
		free(gm);
	}

	// This group is at the head, remove it
	if (groups_head == g) {
		groups_head = g->next;
	} else {

		// Cycle through current groups and unlink this one.
		for (grun = groups_head; grun->next; grun = grun->next) {

			if (grun->next == g) {
				grun->next = g->next;
				free(g);
				break;
			}
		}

	}

	consolef("done!\n");

	group_save();

}

void group_del_byname(char *group)
{
	struct group *g;

	if ((g = group_find_byname(group)))
		group_del(g);
}

int group_newgid(void)
{

	struct group *g;
	int highest = 0;

	for (g = groups_head; g; g = g->next) {
		if (g->gid > highest)
			highest = g->gid;
	}

	return highest + 1;

}

int group_count_flag(struct group *g, int flag)
{
	int cnt = 0;
	struct group_member *gm;

	for (gm = g->members; gm; gm = gm->next) {
		if (flag) {
			if (gm->flags & flag)
				cnt++;
		} else {
			cnt++;
		}
	}

	return cnt;
}

int group_count_ratio(struct group *g, int ratio)
{
	int cnt = 0;
	struct group_member *gm;

	for (gm = g->members; gm; gm = gm->next) {
		if (gm->ratio == ratio)
			cnt++;
	}

	return cnt;
}


//-------------------------------------------------------------------
// GROUP FINDING FUNCTIONS

struct group *group_find_byname(char *group)
{

	struct group *g;

	for (g = groups_head; g; g = g->next) {
		if (!strcasecmp(g->name, group))
			return g;
	}

	return NULL;

}

char *group_findname_bygid(int gid)
{

	struct group *g;

	if (!(g = group_find_bygid(gid)))
		return NULL;

	return g->name;

}


struct group *group_find_bygid(int gid)
{

	struct group *g;

	for (g = groups_head; g; g = g->next) {
		if (g->gid == gid)
			return g;
	}

	return NULL;

}


struct group_member *group_hasuser(struct group *g, char *user)
{
	struct group_member *gm;

	for (gm = g->members; gm; gm = gm->next) {
		if (!strcasecmp(gm->name, user))
			return gm;
	}

	return NULL;
}

struct group_member *group_hasgadmin(struct group *g, char *user)
{
	struct group_member *gm;

	for (gm = g->members; gm; gm = gm->next) {
		if (!strcasecmp(gm->name, user) && (gm->flags & GROUP_ADMIN))
			return gm;
	}

	return NULL;
}

struct group_member *group_iscurrent(struct group *g, char *user)
{

	struct group_member *gm;

	for (gm = g->members; gm; gm = gm->next) {
		if (!strcasecmp(gm->name, user) && (gm->flags & GROUP_CURRENT))
			return gm;
	}

	return NULL;

}

//---------------------------------------------------------------------
// Group Member Functions
void group_member_add(struct group *g, char *username)
{

	struct group_member *gm;

	// Already a member.
	if (group_hasuser(g, username))
		return;

	// Not enough slots
	if (g->usedslots >= g->totalslots)
		return;


	if ( (gm = malloc(sizeof(struct group_member))) == NULL) {
		consolef("CRITICAL: Malloc error: %s\n", strerror(errno));
		abort();
	}
	bzero(gm, sizeof(struct group_member));

	strncpy(gm->name, username, MAXUSERNAME-1);
	gm->ratio = g->maxratio;

	gm->next = g->members;
	g->members = gm;
	g->usedslots++;

	// Make them a current member if they were in no group.
	if (!group_findcurr_byname(username)) {

		struct login_node *l;

		gm->flags |= GROUP_CURRENT;

		// Update their quota.
		if (!(l = login_isloggedin(username)) && !(l = user_findbyname(username)))
			return;

		if (l->quota == NULL)
			quota_new(l);


		((struct quota_node *)l->quota)->ratio = gm->ratio;
		l->current_group = g;

		if (!login_isloggedin(l->user))
			quota_exit(l);

	}

}

// This takes care of current_user, and quota->ratio for the
// specified user too.
void group_member_del(struct group *g, struct group_member *gm)
{

	struct group_member *run;
	struct group *newcurrent;
	int newratio = server_userquotaratio;

	// Remove the user from the group's list.
	if (gm == g->members) {
		g->members = gm->next;
	} else {

		for (run = g->members; run && run->next; run = run->next) {
			if (run->next == gm) {
				run->next = gm->next;
				break;
			}
		}

	}

	g->usedslots--;

	// This group is their current group.  Find another group for them to be
	// a current member of.  Also update their ratio.
	if (group_member_is_current(gm)) {

		struct login_node *l;

		// Find their next group.
		if ((newcurrent = group_findany_byname(gm->name))) {

			// Find the group_member node for this group.
			struct group_member *newgm = group_hasuser(newcurrent, gm->name);
			// Update their flags.
			newgm->flags |= GROUP_CURRENT;
			newratio = newgm->ratio;

		}

		// Update any logged in user with a new current_group.
		if ((l = login_isloggedin(gm->name))) {


			// Update group.
			l->current_group = newcurrent;

		}

		// Update user with a new ratio
		if (l || (l = user_findbyname(gm->name))) {

			// Update ratio.
			if (l->quota == NULL)
				quota_new(l);

			((struct quota_node *)l->quota)->ratio = newratio;

			if (!login_isloggedin(l->user))
				quota_exit(l);

		}

	}

	free(gm);

}

struct group_member *group_member_findcurr(char *user)
{

	struct group *g;
	struct group_member *gm;

	if ((g = group_findcurr_byname(user))) {

		if ((gm = group_hasuser(g, user)) && group_member_is_current(gm))
			return gm;

	}

	return NULL;

}


void group_member_setcurrent(struct group *g, char *user)
{

	struct group_member *gm;

	// Already current
	//	if (group_iscurrent(g, user))
	//	return;

	group_member_clearcurrent(user);

	// Find their group_member node
	if (!(gm = group_hasuser(g, user)))
		return;

	gm->flags |= GROUP_CURRENT;

}

void group_member_clearcurrent(char *user)
{

	struct group *g;
	struct group_member *gm;

	//if ((gm = group_member_findcurr(user)))
	// gm->flags &= (~GROUP_CURRENT);
	for (g = groups_head; g; g = g->next) {
		for (gm = g->members; gm; gm = gm->next) {
			if (!strcasecmp(gm->name, user)) {
				gm->flags &= ~GROUP_CURRENT;
				consolef("[groups] clearing group '%s'\n", g->name);
			}
		}
	}

}



//---------------------------------------------------------------------
// External LundFTPD functions

char *group_list_groups(char *user)
{
	struct group *g;
	static char buf[256];

	buf[0] = 0;

	for (g = groups_head; g; g = g->next) {
		if (group_hasuser(g, user)) {
			if (group_hasgadmin(g, user))
				strcat(buf, "@");
			strcat(buf, g->name);
			strcat(buf, " ");
		}
	}

	if (!*buf)
		strcpy(buf, "None");

	return buf;
}


// Returns 1 (TRUE) if user exists, 0 if not.
int group_hasuser_byname(char *group, char *user)
{
	struct group *g;

	if (!(g = group_find_byname(group)))
		return 0;
	if (group_hasuser(g, user))
		return 1;
	return 0;
}

// Returns 1 (TRUE) if user exists, 0 if not.
int group_hasgadmin_byname(char *group, char *user)
{
	struct group *g;

	if (!(g = group_find_byname(group)))
		return 0;
	if (group_hasgadmin(g, user))
		return 1;
	return 0;
}


char *group_listgroups(char *user)
{

	struct group *g;
	static char grouplist[256];
	char *p;

	bzero(grouplist, 256);

	for (g = groups_head; g; g = g->next) {
		if (group_hasuser(g, user)) {
			strcat(grouplist, g->name);
			strcat(grouplist, " ");
		}
	}

	if ( (p = strrchr(grouplist, ' '))) {
		if (*p+1 == 0)
			*p = 0;
	}

	return grouplist;

}

struct group *group_findany_byname(char *user)
{

	struct group *g;

	for (g = groups_head; g; g = g->next) {
		if (group_hasuser(g, user))
			return g;
	}

	return NULL;

}

struct group *group_findcurr_byname(char *user)
{
	struct group *g;
	struct group_member *gm;

	for (g = groups_head; g; g = g->next) {
		if ((gm = group_hasuser(g, user)))
			if (group_member_is_current(gm))
				return g;
	}

	return NULL;


}



//-------------------------------------------------------------------
// SITE COMMANDS

void group_site_list(struct login_node *t, char *args)
{

	struct group *g;
	int gcnt=0;

	// HEADER
	socket_print(t,
"200- [GROUPLIST]\r\n"
".-[ GROUP LIST ]---------------------------------------------------------------.\r\n"
"|Group        | Slots| Leech|Gadmin|  WeekUp  |  WeekDn  |   AllUp  |   AllDn  |\r\n"
"|-------------+------+------+------+----------+----------+----------+----------|\r\n"
	);

	// Cycle through each group.
	for (g = groups_head; g; g = g->next) {
		gcnt++;

		// GROUP LINE
		socket_print(t,
					 "| %-11.11s | %2d/%02d| %2d/%02d|%6d|%8.02fGB|%8.02fGB|%8.02fGB|%8.02fGB|\r\n",
					 g->name, g->usedslots, g->totalslots,
					 g->usedleechslots, g->leechslots,
					 group_count_flag(g, GROUP_ADMIN),
					 GIGS(g->bytes_uploaded_week), GIGS(g->bytes_downloaded_week),
					 GIGS(g->bytes_uploaded), GIGS(g->bytes_downloaded)
					 );

	}

	// NO GROUPS FOUND
	if (!gcnt) {
		socket_print(t,
"| No groups!                                                                   |\r\n"
		);
	}

	// FOOTER
	socket_print(t,
"`------------------------------------------------------------------------------'\r\n"
"200 [GROUPLIST]\r\n"
	);

}


void group_site_info(struct login_node *t, char *args)
{

	char *ar=args, *name;
	struct group *g;
	struct group_member *gm;
	char maxlogins[7], maxidle[7], leechslots[10], maxratio[7], gadmins[7];
	int gadmincnt;
	char usercap_up[8], usercap_down[8], grpcap_up[8], grpcap_down[8];
	char usercap_total[8], grpcap_total[8];

	// GROUP NAME
	name = digtoken((char **)&ar, " ");
	if (!name) {
		socket_print(t, "500 GROUPINFO <GROUP>\r\n");
		return;
	}

	// May I view a group?
	if (!((t->level|server_defaultflags)&UF_GROUP)
		&& !(group_hasgadmin_byname(name, t->user)))
	{
		socket_number(t, 599, "GROUPADD");
		return;
	}

	// GROUP EXISTS?
	if ((g = group_find_byname(name)) == NULL) {
		socket_print(t, "500 No such group as `%s'\r\n", name);
		return;
	}

	// Format printable data:
	// MAXLOGINS
	if (g->maxlogins == -1)
		snprintf(maxlogins, 6, "unlim");
	else
		snprintf(maxlogins, 6, "%d", g->maxlogins);
	// MAXIDLE
	if (g->idle == -1)
		snprintf(maxidle, 6, "unlim");
	else
		snprintf(maxidle, 6, "%d", g->idle);
	// LEECH
	if (g->leechslots == -1)
		snprintf(leechslots, 6, "unlim");
	else if (g->leechslots == 0)
		snprintf(leechslots, 6, "none");
	else {
		int usedleech = group_count_ratio(g, -1);
		snprintf(leechslots, 6, "%d/%d", usedleech, g->leechslots);
	}
	// MAXRATIO
	if (g->maxratio == -1)
		snprintf(maxratio, 6, "leech");
	else
		snprintf(maxratio, 6, "1:%d", g->maxratio);
	// GADMINS
	gadmincnt = group_count_flag(g, GROUP_ADMIN);
	if (gadmincnt == 0)
		snprintf(gadmins, 6, "none");
	else
		snprintf(gadmins, 6, "%d", gadmincnt);

	// CAPS - UNIMPLEMENTED RIGHT NOW
	snprintf(usercap_up, 7, "none");
	snprintf(usercap_down, 7, "none");
	snprintf(grpcap_up, 7, "none");
	snprintf(grpcap_down, 7, "none");
	snprintf(usercap_total, 7, "none");
	snprintf(grpcap_total, 7, "none");


	// HEADER
	socket_print(t,
"200- [GROUPINFO]\r\n"
".-[ GROUP INFO ]---------------------------------------------------------------.\r\n"
"| Group: %-11s | Slots: %2d/%-2d    | Max Logins: %-6s | Max Idle: %-6s |\r\n"
"|   GID: %-11d | Leech: %-8s |  Max Ratio: %-6s |  GADMINS: %-6s |\r\n"
"|------------------------------------------------------------------------------|\r\n",
			g->name, g->usedslots, g->totalslots, maxlogins, maxidle,
			g->gid, leechslots, maxratio, gadmins
	);

	// Uploads/Downloads/Caps
	socket_print(t,
"| STATS |    Day |    Week |    Month |   Alltime | CAPS   | PerUser |   Group |\r\n"
"|-------+--------+---------+----------+-----------|--------+-------------------|\r\n"
"|   Up: | % 4.1fGB | % 5.1fGB | % 6.1fGB | % 7.1fGB |    Up: | %-7s | %-7s |\r\n"
"| Down: | % 4.1fGB | % 5.1fGB | % 6.1fGB | % 7.1fGB |  Down: | %-7s | %-7s |\r\n"
"|  Tot: | % 4.1fGB | % 5.1fGB | % 6.1fGB | % 7.1fGB | Total: | %-7s | %-7s |\r\n",
			// UP
			GIGS(g->bytes_uploaded_day), GIGS(g->bytes_uploaded_week),
			GIGS(g->bytes_uploaded_month), GIGS(g->bytes_uploaded),
			usercap_up, grpcap_up,
			// DOWN
			GIGS(g->bytes_downloaded_day), GIGS(g->bytes_downloaded_week),
			GIGS(g->bytes_downloaded_month), GIGS(g->bytes_downloaded),
			usercap_down, grpcap_down,
			// TOTAL
			GIGS(g->bytes_uploaded_day + g->bytes_downloaded_day),
			GIGS(g->bytes_uploaded_week + g->bytes_downloaded_week),
			GIGS(g->bytes_uploaded_month + g->bytes_downloaded_month),
			GIGS(g->bytes_uploaded + g->bytes_downloaded),
			usercap_total, grpcap_total
	);

	// MEMBERS HEADER
	socket_print(t,
"|------------------------------------------------------------------------------|\r\n"
"| Member      | Tagline            | Ratio |  WkUp |  WkDn |   AllUp | AllDown |\r\n"
"|-------------+--------------------+-------+-------+-------+---------+---------|\r\n"
	);

	if (g->members == NULL) {
			socket_print(t,
"| No members!                                                                  |\r\n"
			);
	}

	for (gm = g->members; gm; gm = gm->next) {

		char ratio[6];
		struct login_node *l;

		if (!(l = login_isloggedin(gm->name)) && !(l = user_findbyname(gm->name))) {
			socket_print(t,
"| %-11s | Unknown             | Unkwn | Unkwn | Uknwn | Unkwn   | Unkwn   |\r\n",
					gm->name
			);

			// This can happen if user is deleted, but user is not removed from
			// groups that they belong to.

		} else {

			struct quota_node *q;

			if (!(l->quota))
				quota_new(l);

			q = (struct quota_node *)l->quota;

			// Use the ratio for this group, not current users ratio.
			if (gm->ratio == -1)
				snprintf(ratio, 6, "leech");
			else
				snprintf(ratio, 6, "1:%d", gm->ratio);


			socket_print(t,
"|%c%-11s | %-18.18s | %5s | % 3.1fG | % 3.1fG | % 6.1fG | % 6.1fG |\r\n",
					(group_member_is_gadmin(gm)) ? '@' : ' ',
					gm->name, (l->tagline[0]) ? l->tagline : "No tagline!",
					ratio, GIGS(q->bytes_up_week), GIGS(q->bytes_down_week),
					GIGS(q->bytes_up), GIGS(q->bytes_down)
			);

			if (!login_isloggedin(l->user))
				quota_exit(l);

		}
	}

	// FOOTER
	socket_print(t,
"`------------------------------------------------------------------------------'\r\n"
	);

	socket_print(t, "200 [GROUPINFO]\r\n");

}



void group_auto_section(struct login_node *t, char *name)
{
	section_t *def, *old;
	char *path;


	if ((def = section_find_byname(SECTION_GROUP_DEFAULT))) {

		// Create new path,
		path = malloc(strlen(def->path) +
					  strlen(name) +
					  1 + 1); // plus / plus nil

		if (!path) return;

		sprintf(path, "%s/%s", def->path, name);

		// Check if the section already exists.. could be defined
		// in .conf file after all.
		old = section_find(path);

		if (!old || mystrccmp(path, old->path)) {

			old = section_add(path);
			if (!old) return;

			// Section created.
			old->name = strdup(name);

			// Re-think inheritance
			section_apply_defaults();

			// The above does not work :( Assign over manual.
			old->stealth   = def->stealth;
			old->racestats = def->racestats;


			// Force this group only.
			section_add_groups(old, name);

			// Create the dir?


			if (t) socket_print(t, "200-Auto created section '%s'.\r\n",
								path);

		}
	}
}





// Add a group to the database. - F@DM
void group_site_add(struct login_node *t, char *args)
{

	char *ar=args, *name, *tmp;
	int totalslots, leechslots, maxratio, maxlogins, maxidle;

	// May I add a group?
	if (!((t->level|server_defaultflags)&UF_GROUP)) {
		socket_number(t, 599, "GROUPADD");
		return;
	}

	// GROUP NAME
	name = digtoken((char **)&ar, " ");
	if (!name) {
		socket_print(t, "500 GROUPADD <GROUP> [TOTALSLOTS] [LEECHSLOTS] [MAXLOGINS] [MAXRATIO] [MAXIDLE]\r\n");
		return;
	}

    // RATIO SLOTS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        totalslots = server_grouptotalslots;
    else
        totalslots = atoi(tmp);
	if (totalslots <= 0) {
		socket_print(t, "500 You cannot have 0 or less slots for a group!\r\n");
		return;
	}

    // LEECH SLOTS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        leechslots = server_groupleechslots;
    else
        leechslots = atoi(tmp);
	if (leechslots < 0) {
		socket_print(t, "500 You cannot have a negative number of leech slots!\r\n");
		return;
	}
	if (leechslots > totalslots) {
		socket_print(t, "500 You cannot have more leech slots than slots in group!\r\n");
		return;
	}

    // MAX LOGINS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxlogins = server_groupmaxlogins;
    else
        maxlogins = atoi(tmp);
	if (maxlogins < 0)
		maxlogins = -1;

    // MAX RATIO
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxratio = server_groupmaxratio;
    else
        maxratio = atoi(tmp);
	if (maxratio < 1)
		maxratio = -1;

    // MAX IDLETIME
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxidle = server_groupmaxidle;
    else
        maxidle = atoi(tmp);
	if (maxidle < 1)
		maxidle = -1;

	// GROUP EXISTS
	if (group_find_byname(name)) {
		socket_print(t, "500 Group %s already exists!\r\n", name);
		return;
	}

	group_add(name, totalslots, leechslots, maxlogins, maxratio, maxidle);


	// GROUP ADDED - PRINT OUT TO USER
	socket_print(t, "200-Successfully added group %s.\r\n", name);
	// Slots && Leech Slots
	socket_print(t, "200-Group has ");
	if (totalslots == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", totalslots);
	socket_print(t, " slots and ");
	if (leechslots == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", leechslots);
	socket_print(t, " leech slots.\r\n");
	// Logins/Ratio
	socket_print(t, "200-Group has ");
	if (maxlogins == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", maxlogins);
	socket_print(t, " simultaneous logins, ");
	if (maxratio == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "1:%d", maxratio);
	socket_print(t, " maximum ratio.\r\n");
	// Idle
	socket_print(t, "200-Group users may idle ");
	if (maxidle == -1)
		socket_print(t, "indefinitely");
	else
		socket_print(t, "for %d seconds", maxidle);
	socket_print(t, ".\r\n");


	group_auto_section(t, name);


	socket_print(t, "200 OK.\r\n");


}




// Modify a group in the database. - F@DM
void group_site_mod(struct login_node *t, char *args)
{

	char *ar=args, *name, *tmp;
	int totalslots, leechslots, maxratio, maxlogins, maxidle;
	struct group *g;

	// May I add a group?
	if (!((t->level|server_defaultflags)&UF_GROUP)) {
		socket_number(t, 599, "GROUPMOD");
		return;
	}

	// GROUP NAME
	name = digtoken((char **)&ar, " ");
	if (!name) {
		socket_print(t, "500 GROUPMOD <GROUP> [TOTALSLOTS] [LEECHSLOTS] [MAXLOGINS] [MAXRATIO] [MAXIDLE]\r\n");
		return;
	}

    // RATIO SLOTS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        totalslots = server_grouptotalslots;
    else
        totalslots = atoi(tmp);
	if (totalslots <= 0) {
		socket_print(t, "500 You cannot have 0 or less slots for a group!\r\n");
		return;
	}

    // LEECH SLOTS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        leechslots = server_groupleechslots;
    else
        leechslots = atoi(tmp);
	if (leechslots < 0) {
		socket_print(t, "500 You cannot have a negative number of leech slots!\r\n");
		return;
	}
	if (leechslots > totalslots) {
		socket_print(t, "500 You cannot have more leech slots than slots in group!\r\n");
		return;
	}

    // MAX LOGINS
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxlogins = server_groupmaxlogins;
    else
        maxlogins = atoi(tmp);
	if (maxlogins < 0)
		maxlogins = -1;

    // MAX RATIO
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxratio = server_groupmaxratio;
    else
        maxratio = atoi(tmp);
	if (maxratio < 1)
		maxratio = -1;

    // MAX IDLETIME
    tmp = digtoken((char **)&ar, " ");
    if (!tmp)
        maxidle = server_groupmaxidle;
    else
        maxidle = atoi(tmp);
	if (maxidle < 1)
		maxidle = -1;

	// GROUP EXISTS
	if (! (g = group_find_byname(name))) {
		socket_print(t, "500 Group %s does not exist!\r\n", name);
		return;
	}

	// Modify it.
	//group_add(name, totalslots, leechslots, maxlogins, maxratio, maxidle);
	g->leechslots = leechslots;
	g->totalslots = totalslots;
	g->maxlogins = maxlogins;
	g->maxratio = maxratio;
	g->idle = maxidle;

	// GROUP ADDED - PRINT OUT TO USER
	socket_print(t, "200-Successfully modified group %s.\r\n", name);
	// Slots && Leech Slots
	socket_print(t, "200-Group has ");
	if (totalslots == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", totalslots);
	socket_print(t, " slots and ");
	if (leechslots == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", leechslots);
	socket_print(t, " leech slots.\r\n");
	// Logins/Ratio
	socket_print(t, "200-Group has ");
	if (maxlogins == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "%d", maxlogins);
	socket_print(t, " simultaneous logins, ");
	if (maxratio == -1)
		socket_print(t, "unlimited");
	else
		socket_print(t, "1:%d", maxratio);
	socket_print(t, " maximum ratio.\r\n");
	// Idle
	socket_print(t, "200-Group users may idle ");
	if (maxidle == -1)
		socket_print(t, "indefinitely");
	else
		socket_print(t, "for %d seconds", maxidle);
	socket_print(t, ".\r\n");


	socket_print(t, "200 OK.\r\n");

	// Save groups file?

}



// Del a group from the database.
void group_site_del(struct login_node *t, char *args)
{

	char *ar=args, *name;
	struct group *g;

	// May I delete a group?
	if (!((t->level|server_defaultflags)&UF_GROUP)) {
		socket_number(t, 599, "GROUPDEL");
		return;
	}

	// GROUP NAME
	name = digtoken((char **)&ar, " ");
	if (!name) {
		socket_print(t, "500 GROUPDEL <GROUP>\r\n");
		return;
	}

	// FIND GROUP
	if ( (g = group_find_byname(name)) == NULL ) {
		socket_print(t, "500 Group %s did not exist!\r\n", name);
		return;
	}

	// UPDATE GROUP USERS
	while(g->members) {

		// Remove from current group
		group_member_del(g, g->members);
	}

	// TODO: Delete users?

	// DELETE THE GROUP
	group_del(g);
	group_save();

	socket_print(t, "200 Group %s was successfully fragged!\r\n", name);

}

// Add/delete users, UF_GROUP only.
void group_site_user(struct login_node *t, char *args)
{

	char *ar=args, *grp, *user;
	struct group *g;
	struct login_node *l;


	// May I add/remove a user?
	if (!((t->level|server_defaultflags)&UF_GROUP)) {
		socket_number(t, 599, "GROUPUSER");
		return;
	}

	// GROUP
	if (!(grp = digtoken((char **)&ar, " "))) {
		socket_print(t, "500 GROUPUSER <GROUP> +-<USER>\r\n");
		return;
	}

	// USER
	if (!(user = digtoken((char **)&ar, " ")) || user[1] == 0) {
		socket_print(t, "500 GROUPUSER <GROUP> +-<USER>\r\n");
		return;
	}

	// GROUP EXISTS?
	if (!(g = group_find_byname(grp))) {
		socket_print(t, "500 Group '%s' doesn't exist!\r\n", grp);
		return;
	}


	// ADD USER
	if (user[0] == '+') {

		// USER EXISTS?
		if ( !(l = login_isloggedin(&user[1])) && !(l = user_findbyname(&user[1]))) {
			socket_print(t, "500 User '%s' doesn't exist!\r\n", &user[1]);
			return;
		}

		// User already in group
		if (group_hasuser(g, &user[1])) {
			socket_print(t,
				"500 User '%s' is already in group '%s'.\r\n", &user[1], grp);
				return;
		}

		// Add into group
		group_member_add(g, &user[1]);

		socket_print(t, "200 Successfully added user '%s' to group '%s'.\r\n",
				&user[1], grp);

	// REMOVE USER
	} else {

		struct group_member *gm;

		// User not in group
		if (!(gm = group_hasuser(g, &user[1]))) {
			socket_print(t,
				"500 User '%s' is not in group '%s'.\r\n", &user[1], grp);
			return;
		}

		group_member_del(g, gm);

		socket_print(t, "200 Successfully removed user '%s' "
						"from group '%s'.\r\n",
						&user[1], grp);

	}

	group_save();


}

void group_site_admin(struct login_node *t, char *args)
{

	char *ar=args, *grp, *user;
	struct group *g;
	struct login_node *l;
	struct group_member *gm;

	// May I add/remove a user?
	if (!((t->level|server_defaultflags)&UF_GROUP)) {
		socket_number(t, 599, "GROUPADMIN");
		return;
	}

	// GROUP
	if (!(grp = digtoken((char **)&ar, " "))) {
		socket_print(t, "500 GROUPADMIN <GROUP> +-<USER>\r\n");
		return;
	}

	// USER
	if (!(user = digtoken((char **)&ar, " ")) || user[1] == 0) {
		socket_print(t, "500 GROUPADMIN <GROUP> +-<USER>\r\n");
		return;
	}

	// GROUP EXISTS?
	if (!(g = group_find_byname(grp))) {
		socket_print(t, "500 Group '%s' doesn't exist!\r\n", grp);
		return;
	}

	// USER EXISTS?
	if (!(l = login_isloggedin(&user[1])) && !(l = user_findbyname(&user[1]))) {
		socket_print(t, "500 User '%s' doesn't exist!\r\n", &user[1]);
		return;
	}

	// USER EXISTS IN GROUP?
	if ((gm = group_hasuser(g, &user[1])) == NULL) {
		socket_print(t, "500 User '%s' is not in group '%s'\r\n",
				&user[1], grp);
		return;
	}

	// ADD ADMIN
	if (user[0] == '+') {

		// ALREADY ADMIN
		if (group_member_is_gadmin(gm)) {
			socket_print(t, "500 User '%s' is already an admin of group '%s'\n",
					&user[1], grp);
			return;
		}

		gm->flags |= GROUP_ADMIN;

		socket_print(t, "200 User '%s' is now an admin of group '%s'\r\n",
				&user[1], grp);

	// REMOVE ADMIN
	} else {

		// NOT ADMIN
		if (!group_member_is_gadmin(gm)) {
			socket_print(t, "500 User '%s' isn't an admin of group '%s'\n",
					&user[1], grp);
			return;
		}

		gm->flags &= (~GROUP_ADMIN);

		socket_print(t, "200 User '%s' is no longer an admin of group '%s'.\r\n",
				&user[1], grp);

	}

	group_save();
}




void group_site_adduser(struct login_node *t, char *args)
{

	socket_print(t, "200 Someone should code this\r\n");

}

