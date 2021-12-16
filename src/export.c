/*
 * Main export/import routines. Converting database to text file 
 * and vice-verse
 */
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>


#include "export.h"
#include "global.h"
#include "login.h"
#include "user.h"
#include "log.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "misc.h"


// This function is identical to get_longlong in misc.c?
lion64_t my_atoq(char *word)
{
#if HAVE_STRTOUQ       // unsigned 64 bit
  return strtouq(word, NULL, 10);
#elif HAVE_STRTOULL    // unsigned 64 bit
  return strtoull(word, NULL, 10);
#elif HAVE_STRTOLL     //   signed 64 bit
  return strtoll(word, NULL, 10);
#elif HAVE_STRTOUL     // unsigned 32 bit
  return (lion64u_t)strtoul(word, NULL, 10);
#else
  return atol(word);   //   signed 32 bit
#endif
}


void export_users(void)
{
  struct login_node *t;
  struct quota_node *q;
  FILE *out;
  int i;


#if 1
  out = fopen("users.txt", "wb");
#else
  out = stdout;
#endif

  if (!out) {

    perror("Failed to open users.txt for export dump");
    return;

  }

  consolef("Opening 'users.txt' for export list...\n");


  /* Loop all users */
  t = user_dumpall( 1 );

  while (t) {

    /* Make sure it's an active user */
    if (t->user[0]) {

      consolef("Exporting user %s...\n",   t->user);

      fprintf(out, "user_start     '%s'\r\n",  t->user);

      fprintf(out, "          pass '%s'\r\n",  t->pass);
      fprintf(out, "          time '%lu'\r\n", t->time);
      fprintf(out, "         level '%u'\r\n",  t->level);
      fprintf(out, "          idle '%lu'\r\n", t->idle);
      for (i=0; i<IPMATCHMAX; i++) 
	if (t->matchip[i][0])
	  fprintf(out, "   pattern_%-3d '%s'\r\n", 
		  i+1,t->matchip[i]);
	else
	  fprintf(out, "   pattern_%-3d 'NA'\r\n", 
		  i+1);
      fprintf(out, "       options '%u'\r\n",  t->options);
      fprintf(out, "       tagline '%s'\r\n",  t->tagline);
      fprintf(out, "    num_logins '%u'\r\n",  t->num_logins);
      fprintf(out, "   num_uploads '%u'\r\n",  t->num_uploads);
      fprintf(out, " num_downloads '%u'\r\n",  t->num_downloads);


      /* Quota node after that */

      quota_new(t);

      q = (struct quota_node *)t->quota;
      
      fprintf(out, "         daily '%lu'\r\n", q->daily);
      
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "    bytes_down '%llu'\r\n", q->bytes_down);
#elif defined ( DEC )
      fprintf(out, "    bytes_down '%lu'\r\n", q->bytes_down);
#else
      fprintf(out, "    bytes_down '%qu'\r\n", q->bytes_down);
#endif
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_up '%llu'\r\n", q->bytes_up);
#elif defined ( DEC )
      fprintf(out, "      bytes_up '%lu'\r\n", q->bytes_up);
#else
      fprintf(out, "      bytes_up '%qu'\r\n", q->bytes_up);
#endif

      fprintf(out, "         ratio '%d'\r\n", q->ratio);
      fprintf(out, "    time_limit '%u'\r\n", q->time_limit);


      fprintf(out, "      files_up '%u'\r\n", q->files_up);
      fprintf(out, "    files_down '%u'\r\n", q->files_down);

      fprintf(out, "    seconds_up '%u'\r\n", q->seconds_up);
      fprintf(out, "  seconds_down '%u'\r\n", q->seconds_down);

      fprintf(out, "   nuked_files '%u'\r\n", q->nuked_files);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "   nuked_bytes '%llu'\r\n", q->nuked_bytes);
#elif defined ( DEC )
      fprintf(out, "   nuked_bytes '%lu'\r\n", q->nuked_bytes);
#else
      fprintf(out, "   nuked_bytes '%qu'\r\n", q->nuked_bytes);
#endif

      fprintf(out, "got_nuked_files '%u'\r\n", q->got_nuked_files);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "got_nuked_bytes '%llu'\r\n", q->got_nuked_bytes);
#elif defined ( DEC )
      fprintf(out, "got_nuked_bytes '%lu'\r\n", q->got_nuked_bytes);
#else
      fprintf(out, "got_nuked_bytes '%qu'\r\n", q->got_nuked_bytes);
#endif

      /* ** */

      fprintf(out, "      files_up_day '%u'\r\n", q->files_up_day);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_up_day '%llu'\r\n", q->bytes_up_day);
#elif defined ( DEC )
      fprintf(out, "      bytes_up_day '%lu'\r\n", q->bytes_up_day);
#else
      fprintf(out, "      bytes_up_day '%qu'\r\n", q->bytes_up_day);
#endif
      fprintf(out, "    seconds_up_day '%u'\r\n", q->seconds_up_day);
      fprintf(out, "  highest_files_up_day '%d'\r\n", q->highest_files_up_day);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_up_day '%llu'\r\n", q->highest_bytes_up_day);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_up_day '%lu'\r\n", q->highest_bytes_up_day);
#else
      fprintf(out, "  highest_bytes_up_day '%qu'\r\n", q->highest_bytes_up_day);
#endif

      fprintf(out, "   times_topup_day '%d'\r\n", q->times_topup_day);


      fprintf(out, "      files_down_day '%u'\r\n", q->files_down_day);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_down_day '%llu'\r\n", q->bytes_down_day);
#elif defined ( DEC )
      fprintf(out, "      bytes_down_day '%lu'\r\n", q->bytes_down_day);
#else
      fprintf(out, "      bytes_down_day '%qu'\r\n", q->bytes_down_day);
#endif
      fprintf(out, "    seconds_down_day '%u'\r\n", q->seconds_down_day);
      fprintf(out, "  highest_files_down_day '%d'\r\n", q->highest_files_down_day);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_down_day '%llu'\r\n", q->highest_bytes_down_day);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_down_day '%lu'\r\n", q->highest_bytes_down_day);
#else
      fprintf(out, "  highest_bytes_down_day '%qu'\r\n", q->highest_bytes_down_day);
#endif

      fprintf(out, "   times_topdown_day '%d'\r\n", q->times_topup_day);


      /* ** */


      fprintf(out, "      files_up_week '%u'\r\n", q->files_up_week);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_up_week '%llu'\r\n", q->bytes_up_week);
#elif defined ( DEC )
      fprintf(out, "      bytes_up_week '%lu'\r\n", q->bytes_up_week);
#else
      fprintf(out, "      bytes_up_week '%qu'\r\n", q->bytes_up_week);
#endif
      fprintf(out, "    seconds_up_week '%u'\r\n", q->seconds_up_week);
      fprintf(out, "  highest_files_up_week '%d'\r\n", q->highest_files_up_week);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_up_week '%llu'\r\n", q->highest_bytes_up_week);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_up_week '%lu'\r\n", q->highest_bytes_up_week);
#else
      fprintf(out, "  highest_bytes_up_week '%qu'\r\n", q->highest_bytes_up_week);
#endif

      fprintf(out, "   times_topup_week '%d'\r\n", q->times_topup_week);


      fprintf(out, "      files_down_week '%u'\r\n", q->files_down_week);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_down_week '%llu'\r\n", q->bytes_down_week);
#elif defined ( DEC )
      fprintf(out, "      bytes_down_week '%lu'\r\n", q->bytes_down_week);
#else
      fprintf(out, "      bytes_down_week '%qu'\r\n", q->bytes_down_week);
#endif
      fprintf(out, "    seconds_down_week '%u'\r\n", q->seconds_down_week);
      fprintf(out, "  highest_files_down_week '%d'\r\n", q->highest_files_down_week);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_down_week '%llu'\r\n", q->highest_bytes_down_week);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_down_week '%lu'\r\n", q->highest_bytes_down_week);
#else
      fprintf(out, "  highest_bytes_down_week '%qu'\r\n", q->highest_bytes_down_week);
#endif

      fprintf(out, "   times_topdown_week '%d'\r\n", q->times_topup_week);


      /* ** */


      fprintf(out, "      files_up_month '%u'\r\n", q->files_up_month);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_up_month '%llu'\r\n", q->bytes_up_month);
#elif defined ( DEC )
      fprintf(out, "      bytes_up_month '%lu'\r\n", q->bytes_up_month);
#else
      fprintf(out, "      bytes_up_month '%qu'\r\n", q->bytes_up_month);
#endif
      fprintf(out, "    seconds_up_month '%u'\r\n", q->seconds_up_month);
      fprintf(out, "  highest_files_up_month '%d'\r\n", q->highest_files_up_month);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_up_month '%llu'\r\n", q->highest_bytes_up_month);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_up_month '%lu'\r\n", q->highest_bytes_up_month);
#else
      fprintf(out, "  highest_bytes_up_month '%qu'\r\n", q->highest_bytes_up_month);
#endif

      fprintf(out, "   times_topup_month '%d'\r\n", q->times_topup_month);


      fprintf(out, "      files_down_month '%u'\r\n", q->files_down_month);
#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "      bytes_down_month '%llu'\r\n", q->bytes_down_month);
#elif defined ( DEC )
      fprintf(out, "      bytes_down_month '%lu'\r\n", q->bytes_down_month);
#else
      fprintf(out, "      bytes_down_month '%qu'\r\n", q->bytes_down_month);
#endif
      fprintf(out, "    seconds_down_month '%u'\r\n", q->seconds_down_month);
      fprintf(out, "  highest_files_down_month '%d'\r\n", q->highest_files_down_month);

#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  highest_bytes_down_month '%llu'\r\n", q->highest_bytes_down_month);
#elif defined ( DEC )
      fprintf(out, "  highest_bytes_down_month '%lu'\r\n", q->highest_bytes_down_month);
#else
      fprintf(out, "  highest_bytes_down_month '%qu'\r\n", q->highest_bytes_down_month);
#endif

      fprintf(out, "   times_topdown_month '%d'\r\n", q->times_topup_month);


      /* ** */

      fprintf(out, "   time_on_day '%lu'\r\n", q->time_on_day);
      fprintf(out, "   time_on_week '%lu'\r\n", q->time_on_week);
      fprintf(out, "   time_on_month '%lu'\r\n", q->time_on_month);
      fprintf(out, "   time_on_alltime '%lu'\r\n", q->time_on_alltime);

      fprintf(out, "   last_nuked '%lu'\r\n", q->last_nuked);

      fprintf(out, "   login_times '%u'\r\n", q->login_times);


#if defined ( IRIX ) || ( HPUX )  || ( WIN32 )
      fprintf(out, "  credits '%llu'\r\n", q->credits);
#elif defined ( DEC )
      fprintf(out, "  credits '%lu'\r\n", q->credits);
#else
      fprintf(out, "  credits '%qu'\r\n", q->credits);
#endif

      quota_exit(t);


      fprintf(out, "user_end   '%s'\r\n", t->user);

    }

    t = user_dumpall( 0 );

  }

  fclose(out);

}






/*
 * Read a text file and import users
 */

void import_users(void)
{
  FILE *in;
  struct login_node t;
  struct quota_node q;
  char buff[1024];
  int has_user, i;


  consolef("Opening file users.txt for importing...\r\n");


  in = fopen("users.txt", "rb");

  if (!in) {
    perror("Failed to open import file");
    return;
  }



  /* While we can get a keyword from the file */
  while (get_keyword(in, buff) != EOF) {
    
    /* Wait for start of user */
    if (!mystrccmp(buff, "user_start")) {
      
      /* We've started a new user - get the user name */
      if (get_keyword(in, buff) ==  EOF) {
	consolef("Parse error reading user -- skipping\n");
	continue;
      }
      
      /* Check if the user is already present */
      if (user_findbyname( buff )) {
	consolef("User '%s' already present -- skipping\n", buff);
	continue;
      }
      
      has_user = 1;
      memset(&t, 0, sizeof(t));
      memset(&q, 0, sizeof(q));
      
      /* Copy over username */
      strncpy(t.user, buff, MAXUSERNAME);
      t.user[MAXUSERNAME] = 0;
      
      
      consolef("Reading user '%s'...\n", t.user);

      continue;
      
    } 



    /* If we are parsing a user, read the fields required */
    if (has_user) {

    
      if (!mystrccmp(buff, "user_end")) {

	/* This writes us to disk */
	user_addnew( &t ); 

	/* Ask to get the (empty) quota node assigned */
	quota_addnew(&t, 0);
	quota_new(&t);

	/* Copy over some essential fields */
	q.offset =    ((struct quota_node *)t.quota)->offset;
	q.next =      ((struct quota_node *)t.quota)->next;
	q.ref_count = ((struct quota_node *)t.quota)->ref_count;

	/* Make us be dirty so we are written to disk */
	q.dirty = 1;

	/* Copy the whole structure back */
	memcpy(t.quota, &q, sizeof(q));
	
	/* Release quota and hence write it to disk. */
	quota_exit(&t);

	has_user = 0;
	
	consolef("Completed user '%s' -- writing...\n", t.user);
	
      }
      else if (!mystrccmp(buff,"pass") && (get_keyword(in, buff) != EOF)) {
	strncpy(t.pass, buff, MAXPASSNAME);
	t.pass[MAXPASSNAME] = 0;
      } else if (!mystrccmp(buff,"time") && (get_keyword(in, buff) != EOF)) {
	t.time = atol(buff);
      } else if (!mystrccmp(buff,"level") && (get_keyword(in, buff) != EOF)) {
	t.level = strtoul(buff, NULL, 10);
      } else if (!mystrccmp(buff,"idle") && (get_keyword(in, buff) != EOF)) {
	t.idle = atol(buff);
      } else if (!strncasecmp(buff,"pattern_", 8)) { 
	i = atoi( &buff[8] ) - 1;
	if ((i >= IPMATCHMAX) || (i < 0)) {
	  consolef("Pattern %d defined, but MAX is %d -- skipping\n",
		   i+1, IPMATCHMAX);
	  continue;
	}

	if (get_keyword(in, buff) == EOF) continue;

	strncpy(t.matchip[i], buff, IPMATCHLEN);
	t.matchip[i][IPMATCHLEN-1] = (char) 0;
	
      } else if (!mystrccmp(buff,"options")&& (get_keyword(in, buff) != EOF)) {
	t.options = atoi(buff);
      } else if (!mystrccmp(buff,"tagline")&& (get_keyword(in, buff) != EOF)) {
	strncpy(t.tagline, buff, MAXTAGLINE);
	t.tagline[MAXTAGLINE] = 0;
      } else if (!mystrccmp(buff,"num_logins")&& (get_keyword(in, buff) != EOF)) {
	t.num_logins = atoi(buff);
      } else if (!mystrccmp(buff,"num_uploads")&& (get_keyword(in, buff) != EOF)) {
	t.num_uploads = atoi(buff);
      } else if (!mystrccmp(buff,"num_downloads")&& (get_keyword(in, buff) != EOF)) {
	t.num_downloads = atoi(buff);


      } else if (!mystrccmp(buff,"daily")&& (get_keyword(in, buff) != EOF)) {
	q.daily = atol(buff);
      } else if (!mystrccmp(buff,"bytes_down")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_down = my_atoq(buff);
      } else if (!mystrccmp(buff,"bytes_up")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_up = my_atoq(buff);
      } else if (!mystrccmp(buff,"ratio")&& (get_keyword(in, buff) != EOF)) {
	q.ratio = atoi(buff);
      } else if (!mystrccmp(buff,"time_limit")&& (get_keyword(in, buff) != EOF)) {
	q.time_limit = atoi(buff);
      } else if (!mystrccmp(buff,"files_up")&& (get_keyword(in, buff) != EOF)) {
	q.files_up = atoi(buff);
      } else if (!mystrccmp(buff,"files_down")&& (get_keyword(in, buff) != EOF)) {
	q.files_down = atoi(buff);
      } else if (!mystrccmp(buff,"seconds_up")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_up = atoi(buff);
      } else if (!mystrccmp(buff,"seconds_down")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_down = atoi(buff);
      } else if (!mystrccmp(buff,"nuked_files")&& (get_keyword(in, buff) != EOF)) {
	q.nuked_files = atoi(buff);
      } else if (!mystrccmp(buff,"nuked_bytes")&& (get_keyword(in, buff) != EOF)) {
	q.nuked_bytes = my_atoq(buff);
      } else if (!mystrccmp(buff,"got_nuked_files")&& (get_keyword(in, buff) != EOF)) {
	q.got_nuked_files = atoi(buff);
      } else if (!mystrccmp(buff,"got_nuked_bytes")&& (get_keyword(in, buff) != EOF)) {
	q.got_nuked_bytes = my_atoq(buff);
	/* ** */
      } else if (!mystrccmp(buff,"files_up_day")&& (get_keyword(in, buff) != EOF)) {
	q.files_up_day = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_up_day")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_up_day = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_up_day")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_up_day = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_up_day")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_up_day = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_up_day")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_up_day = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topup_day")&& (get_keyword(in, buff) != EOF)) {
	q.times_topup_day = atoi(buff);
      } else if (!mystrccmp(buff,"files_down_day")&& (get_keyword(in, buff) != EOF)) {
	q.files_down_day = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_down_day")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_down_day = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_down_day")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_down_day = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_down_day")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_down_day = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_down_day")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_down_day = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topdown_day")&& (get_keyword(in, buff) != EOF)) {
	q.times_topdown_day = atoi(buff);
	/* ** */
      } else if (!mystrccmp(buff,"files_up_week")&& (get_keyword(in, buff) != EOF)) {
	q.files_up_week = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_up_week")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_up_week = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_up_week")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_up_week = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_up_week")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_up_week = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_up_week")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_up_week = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topup_week")&& (get_keyword(in, buff) != EOF)) {
	q.times_topup_week = atoi(buff);
      } else if (!mystrccmp(buff,"files_down_week")&& (get_keyword(in, buff) != EOF)) {
	q.files_down_week = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_down_week")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_down_week = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_down_week")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_down_week = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_down_week")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_down_week = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_down_week")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_down_week = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topdown_week")&& (get_keyword(in, buff) != EOF)) {
	q.times_topdown_week = atoi(buff);
	/* ** */
      } else if (!mystrccmp(buff,"files_up_month")&& (get_keyword(in, buff) != EOF)) {
	q.files_up_month = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_up_month")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_up_month = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_up_month")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_up_month = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_up_month")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_up_month = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_up_month")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_up_month = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topup_month")&& (get_keyword(in, buff) != EOF)) {
	q.times_topup_month = atoi(buff);
      } else if (!mystrccmp(buff,"files_down_month")&& (get_keyword(in, buff) != EOF)) {
	q.files_down_month = atoi(buff);
      } else if (!mystrccmp(buff,"bytes_down_month")&& (get_keyword(in, buff) != EOF)) {
	q.bytes_down_month = my_atoq(buff);
      } else if (!mystrccmp(buff,"seconds_down_month")&& (get_keyword(in, buff) != EOF)) {
	q.seconds_down_month = atoi(buff);
      } else if (!mystrccmp(buff,"highest_files_down_month")&& (get_keyword(in, buff) != EOF)) {
	q.highest_files_down_month = atoi(buff);
      } else if (!mystrccmp(buff,"highest_bytes_down_month")&& (get_keyword(in, buff) != EOF)) {
	q.highest_bytes_down_month = my_atoq(buff);
      } else if (!mystrccmp(buff,"times_topdown_month")&& (get_keyword(in, buff) != EOF)) {
	q.times_topdown_month = atoi(buff);




      } else if (!mystrccmp(buff,"time_on_day")&& (get_keyword(in, buff) != EOF)) {
	q.time_on_day = atol(buff);
      } else if (!mystrccmp(buff,"time_on_week")&& (get_keyword(in, buff) != EOF)) {
	q.time_on_week = atol(buff);
      } else if (!mystrccmp(buff,"time_on_month")&& (get_keyword(in, buff) != EOF)) {
	q.time_on_month = atol(buff);
      } else if (!mystrccmp(buff,"time_on_alltime")&& (get_keyword(in, buff) != EOF)) {
	q.time_on_alltime = atol(buff);

      } else if (!mystrccmp(buff,"last_nuked")&& (get_keyword(in, buff) != EOF)) {
	q.last_nuked = atol(buff);
      } else if (!mystrccmp(buff,"login_times")&& (get_keyword(in, buff) != EOF)) {
	q.login_times = atoi(buff);

      } else if (!mystrccmp(buff,"credits")&& (get_keyword(in, buff) != EOF)) {
	q.credits = my_atoq(buff);


      } else {
	consolef("Unknown keyword '%s' in user-section\n", buff);
      }



    } /* has_user */
    
    
  } /* while getword */

  
  fclose(in);

}

