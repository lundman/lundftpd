/*
 *
 * This defines global variables which are updated independently
 * at subjective time intervals
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "login.h"
#include "global.h"
#include "log.h"
#include "data.h"
#include "file.h"
#include "groups.h"
#include "check.h"
#include "quota.h"
#include "user.h"

#include "../lion/src/misc.h"

#ifdef IRCBOT
#include "irc.h"
#endif


//
// WITH STRINGS, DO NOT SET DEFAULT "values" since they are in data segment
// and can not be free'd. Please add to global.c:global_set_default().
//

// MISC GLOBALS
time_t global_time_now   = 0;
char *server_configfile  = NULL;
int server_hasroot       = 0;
int server_userfilecheck = 0;
int server_dupecheck     = 0;


//---------------------------------
// NETWORK SETTINGS
int server_port = 21;
char *server_bindif = NULL;
int server_useident = 0;
int server_data_buffer_size = 1024;
int server_data_bind_port = 0;
int server_capping = 0;
int server_cap_download = 0;
int server_cap_upload = 0;
int server_cap_user_download = 0;
int server_cap_user_upload = 0;
int server_cap_total_download = 0;
int server_cap_total_upload = 0;
int server_data_pasv_start = 0;
int server_data_pasv_end = 0;

//-----------------------------------------------
// FILE LOCATIONS/FILESYSTEM SETUP
char *server_userfile = NULL;
char *server_msgpath = NULL;
char *server_wallpath = NULL;
char *server_quotafile = NULL;
char *server_dupedbfile = NULL;
char *server_logxfer = NULL;
char *server_loguser = NULL;
char *server_logadmin = NULL;
char *server_usechroot = NULL;
int server_logrotate = 0;
int server_dupemaxdays = 0;
int server_usesoftchroot = 0;
int server_permfile = 0644;
int server_goodfile = 0444;
int server_permdir = 0755;
char *server_mkdirstring = NULL;
int server_num_list_helpers = 1;

//-----------------------------------------------
// SERVER SETTINGS
char *server_greeting = NULL;
char *server_rawgreeting = NULL;
char *server_email = NULL;
int server_walllogin = 0;
int server_maximumlogins = -1;
int server_userquotadaily = -1;
int server_userquotaratio = 3;
int server_userquotatime = -1;
int server_anonymous = 0;
int server_forceglobalonly = 0;
int server_usesetuid = 1;
int server_useridstart = 10000;
int server_lookupuid = 0;
char *server_nobody = NULL;
int server_useridle = 900;
int server_usermatch = 0;
int server_showlastlogin = 1;
int server_forcetagline = 0;
char *server_extractmp3format = 0;
int server_unnuke = 0;

//-----------------------------------------------
// ADDUSER DEFAULTS
int server_loginlimit = 0;
int server_uploadlimit = 0;
int server_downloadlimit = 0;
int server_usercredits = 15;
char *server_defaulttagline = NULL;
int server_defaultflags = 0;

//-----------------------------------------------
// GROUP DEFAULTS
char *server_groupfile = NULL;
char *server_grouppath = NULL;
int server_groupmaxlogins = -1;
int server_groupmaxratio = 3;
int server_groupmaxidle = 900;
int server_grouptotalslots = 5;
int server_groupleechslots = 0;
char *server_nogroupname = NULL;
int server_group_pattern_limit = 0;

//-----------------------------------------------
// FILECHECK
int server_maxsizecheck = 0;
int server_backgroundtest = 1;
char *server_autoextract_diz = NULL;

//-----------------------------------------------
// IRC
int server_irccmds = 0;
int server_irc_src_port = 0;


//-----------------------------------------------
// ENCRYPTION
int server_allow_only_secure = 0;
int server_allow_only_secure_data = 0;
char *server_srp_passwdfile = NULL;
char *server_srp_masterkey = NULL;
char *server_tls_rsafile = NULL;
char *server_tls_ciphers = NULL;
char *server_egdsocket = NULL;


/* The actual variable definitions are in global_var.c */

int global_resync_day = 0;
int global_resync_week = 0;
int global_resync_month = 0;
int global_resync_total = 0;

static int global_init = 0;

int global_hourofday = 0;
int global_dayofweek = 0;
int global_dayofmonth = 0;
int global_daysinmonth = 0;

lion64u_t global_total_bytes_up   = 0;
lion64u_t global_total_bytes_down = 0;


int server_rehash = 0;


struct global_struct *global_head = NULL;


void global_update(void)
{
  struct tm *dtime;
  time_t now;

  time(&now);

  dtime = localtime(&now);



  if (!(dtime->tm_min % 10))  /* 10 minute thing */
    global_update_10minutes();

  if (!(dtime->tm_min % 30))  /* 30 minute thing */
    global_update_30minutes();

  if (!(dtime->tm_min)) {     /* Hour thing */

    global_update_hour();


#ifdef SPECIAL_CAP_ON_HOUR
	 if (dtime->tm_hour == SPECIAL_CAP_ON_HOUR)
		 server_capping = 1;
#endif

#ifdef SPECIAL_CAP_OFF_HOUR
	 if (dtime->tm_hour == SPECIAL_CAP_OFF_HOUR)
		 server_capping = 0;
#endif

    if (!(dtime->tm_hour % 12))/* half day */
      global_update_12hours();


    if (!dtime->tm_hour) {    /* midnight */

      global_update_daily();

      if (!dtime->tm_wday)    /* Sunday/Monday */
	global_update_weekly();

      if (dtime->tm_mday == 1)    /* Monthly */
	global_update_monthly();


    }

  }

}




void global_update_minute(void)
{
  /* Do all 1 minute things */


  if (global_resync_day || global_resync_week || global_resync_month ||
      global_resync_total) {

    quota_top_clear();

    /* Write it back to disk that we'd done work on it */
    //group_write();

    global_resync_day   = 0;
    global_resync_week  = 0;
    global_resync_month = 0;
    global_resync_total = 0;

  }



  /* Now check which other updates needs to be called. */

  global_update();




#ifdef IRCBOT
  irc_server();
#endif


#if 0  // DIFF for ST to turn capping off and on at certain times.
 {
	 struct tm *loc;

	 loc = localtime(&global_time_now);

	 if ((loc->tm_hour >= 8) &&
		 (loc->tm_hour <= 21))
		 server_cpscap_enabled = 1;
	 else
		 server_cpscap_enabled = 0;
 }

#endif



}

void global_update_10minutes(void)
{

  /* Update TOP */
  user_top();

#ifdef IRCBOT
  irc_topup();
  irc_topdn();
  irc_wall();
  irc_logo();
#endif


}

void global_update_30minutes(void)
{

}

void global_update_hour(void)
{
  time_t now;
  struct tm *tmm;

  time(&now);
  tmm = localtime(&now);

  global_hourofday = tmm->tm_hour;

}

void global_update_12hours(void)
{

}

void global_update_daily(void)
{
  time_t now;
  struct tm *tmm;

  time(&now);
  tmm = localtime(&now);

  global_dayofweek = tmm->tm_wday + 1;

  global_dayofmonth = tmm->tm_mday;

  if (!global_init)
    global_resync_day = 1;

  if (!global_init)
    global_resync_total = 1;


#ifdef IRCBOT

  // Send day stats.
  if (global_resync_day) {

	  consolef("[global] new day stats...\n");

	  irc_announce("NEWDAY|GBUP=%.2f|FUP=%u|SUP=%u|GBDN=%.2f|FDN=%u|SDN=%u\n",
				   (float)global_ubd / 1073741824.0, global_ufd, global_usd,
				   (float)global_dbd / 1073741824.0, global_dfd, global_dsd
				   );

  }

#endif


  if (global_resync_day && server_logrotate) {
	  file_goroot();
	  log_rotate();
	  file_gononroot();
  }

#ifdef DUPECHECK
  if (global_resync_day && server_dupemaxdays) {
	  file_goroot();
	  //dupe_purge(server_dupemaxdays * 24 * 60 * 60);
	  file_gononroot();
  }
#endif

}

void global_update_weekly(void)
{

  if (!global_init)
    global_resync_week = 1;

}


/*
'A year is a leap year if the date of the year is divisible by four without
remainder, unless it is the last year of the century. The last year of the
century is a leap year only if its number is divisible by 400 without
remainder, e.g the years 1800, 1900 had only 365 days but the year 2000
will have 366 days'
*/

void global_update_monthly(void)
{
  int daysinmonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
  time_t now;
  struct tm *tmm;

  time(&now);
  tmm = localtime(&now);

  if (tmm->tm_mon == 1) { /* Febuary, bastard */

    if (!(tmm->tm_year % 4)) {

      if ( (tmm->tm_year % 100) || !(tmm->tm_year % 400)) {
	/* 2000 should have 366 */

	global_daysinmonth = 29;

      } else {

	global_daysinmonth = 28;  /* 1900 should have 265 */

      }

    } else {

      global_daysinmonth = 28;

    }

  } else {

    global_daysinmonth = daysinmonth[tmm->tm_mon];

  }


  if (!global_init)
    global_resync_month = 1;

}


void global_update_all(void)
{
  global_init = 1;

  global_resync_total = 1;

  /* Need to call user_top() before resync */
  user_top();
  global_update_minute();
  global_update_10minutes();
  global_update_30minutes();
  global_update_hour();
  global_update_12hours();
  global_update_daily();
  global_update_weekly();
  global_update_monthly();

  global_init = 0;

  /* Initialize things */
  global_readwall();

#if 0
  consolef("ok, %d/24 %d/7 %d/%d\n",
	   global_hourofday, global_dayofweek, global_dayofmonth,
	   global_daysinmonth);
#endif

}




void global_readwall(void)
{
  int i;

  for (i = 0; i < NUM_WALL; i++)
    global_mwm[i] = NULL;

  /* Read the file */

  file_readwall();

}



//
// We can't set static string defaults, since we will try free things.
//
void global_set_default(void)
{

	SAFE_COPY(server_configfile, "lundftpd.conf");
	SAFE_COPY(server_nobody, "nobody");

}

