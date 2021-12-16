/*
 * ARGH! What the hell is wrong with Linux and its users?!? Seems there is a
 * mixture of db uses, either "dbopen", "db_open" or "ndbm". I've added all
 * but man what a pain. The world would be a better place without Linux
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

/* #define LINUX2 */
#ifdef DUPECHECK

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#if USE_DB_H
# include <db.h>
#elif USE_GDBM_H
# include <gdbm.h>
#elif USE_NDBM_H
# include <ndbm.h>
#else
# error "No DB type defined"
#endif

#include "global.h"
#include "login.h"
#include "data.h"
#include "log.h"
#include "dupe.h"
#include "misc.h"
#include "fnmatch.h"
#include "file.h"

#if USE_GDBM_H
#define DB GDBM_FILE
#define DBT datum
#define data dptr
#define size dsize
#endif

#if USE_NDBM_H
#define DB DBM
#define DBT datum
#define data dptr
#define size dsize
#endif

#if USE_GDBM_H
  static DB dupe_db = NULL;
#else
  static DB *dupe_db = NULL;
#endif



static  char *dupe_path_list[DUPE_MAX_PATH];
static unsigned int dupe_num_path = 0;

// Always run as root.
void dupe_init(void)
{

	if (!server_dupecheck || !server_dupedbfile)
		return;

	file_goroot();

#if USE_NDBM_H
  dupe_db = dbm_open(server_dupedbfile, O_CREAT|O_RDWR, 0644);
#elif USE_GDBM_H
  dupe_db = gdbm_open(server_dupedbfile, 512, GDBM_WRCREAT, 0644, 0);
#else
  dupe_db = dbopen(server_dupedbfile, O_CREAT|O_RDWR, 0644, DB_BTREE, NULL);
#endif

	file_gononroot();

  if (!dupe_db) {

    perror("dbopen()");
    consolef("Couldn't open dupe-database file %s\n", server_dupedbfile);
    exit(0);
  }

}


// Always run as root.
void dupe_quit(void)
{

  if (!dupe_db) return;

	file_goroot();

#if USE_NDBM_H
  dbm_close(dupe_db);
#elif USE_GDBM_H
  gdbm_close(dupe_db);
#else
  dupe_db->close(dupe_db);
#endif
  dupe_db = NULL;


  file_gononroot();

}


void dupe_sync(void)
{

  if (!dupe_db) return;

  file_goroot();

#if USE_NDBM_H
  /* No sync feature? */
#elif USE_GDBM_H
  gdbm_sync(dupe_db);
#else
  dupe_db->sync(dupe_db, 0);
#endif

  file_gononroot();

}



struct dupe_node *dupe_find(char *filename)
{
  DBT key, data;
  int ret;
  static struct dupe_node retbuf;


  if (!dupe_db) return NULL;


  strcpylower(genbuf, filename);

  key.data = genbuf;
  key.size = strlen(genbuf) + 1;


  file_goroot();

#if USE_NDBM_H
  data = dbm_fetch(dupe_db, key);
  ret = !data.size;
#elif USE_GDBM_H
  data = gdbm_fetch(dupe_db, key);
  ret = !data.size;
#else
  ret = dupe_db->get(dupe_db, &key, &data, 0);
#endif

  file_gononroot();


  if (!ret) { /* Good hit, it was in db, which is BAD for the user :) */

    memcpy(&retbuf, data.data, sizeof(retbuf));

    consolef("dupe: %s in dupe-database %s\n", genbuf,
	     retbuf.username);

    return &retbuf;

  }

  consolef("dupe: no occurance of %s in dupe-database\n", genbuf);

  return NULL;

}



int dupe_add(char *filename, char *username, unsigned long filesize,
	     time_t time)
{
  struct dupe_node newdupe;
  DBT key, data;
  int ret;


  strncpy(newdupe.filename, filename, DUPE_MAX_FILENAME);
  newdupe.filename[DUPE_MAX_FILENAME] = 0;
  strlower(newdupe.filename);

  strncpy(newdupe.username, username, MAXUSERNAME);
  newdupe.username[MAXUSERNAME] = 0;

  newdupe.filesize = filesize;
  newdupe.time = time;


  key.data = newdupe.filename;
  key.size = strlen(newdupe.filename) + 1;

  data.data = (void *)&newdupe;
  data.size = sizeof(newdupe);


  file_goroot();


#ifdef USE_NDBM_H
  ret = dbm_store(dupe_db, key, data, DBM_INSERT);
#elif USE_GDBM_H
  ret = gdbm_store(dupe_db, key, data, GDBM_INSERT);
#else
  ret = dupe_db->put(dupe_db, &key, &data, R_NOOVERWRITE);
#endif

  file_gononroot();



  if (!ret) {

    consolef("dupe: %s added - %s\n", newdupe.filename, ctime(&time));
    return 0;

  }

  perror("dupe: dupe->dbput");
  consolef("dupe: %s add failed %d\n", newdupe.filename, ret);

  return ret;
}


int dupe_del(char *filename)
{
  DBT key;
  int ret;

  key.data = filename;
  key.size = strlen(filename) + 1;


  file_goroot();

#if USE_NDBM_H
  ret = dbm_delete(dupe_db, key);
#elif USE_GDBM_H
  ret = gdbm_delete(dupe_db, key);
#else
  ret = dupe_db->del(dupe_db, &key, 0);
#endif

  file_gononroot();


  if (!ret) {

    consolef("dupe: removed %s\n", filename);
    return 0;
  }

  perror("dupe: dupe_db->del");
  consolef("dupe: %s removal failed\n", filename);

  return ret;

}


#if USE_NDBM_H
struct dupe_node *dupe_seq(char *glob)
{
  DBT data;
  struct dupe_node *retnode;


  /* NDBM only returns key's */
  if (!dupe_db) return NULL;



  file_goroot();

  if (glob) {
    data = dbm_firstkey(dupe_db);
  } else {
    data = dbm_nextkey(dupe_db);
  }

  file_gononroot();

  if (!data.size) {


	  return NULL;
  }


  retnode = dupe_find(data.data);

  return retnode;

}

#elif USE_GDBM_H
struct dupe_node *dupe_seq(char *glob)
{
  DBT data;
  struct dupe_node *retnode;
  static DBT last_key;
  static int last = 0;

  if (!dupe_db) return NULL;



  file_goroot();

  if (glob) {

    data = gdbm_firstkey(dupe_db);

  } else {

    data = gdbm_nextkey(dupe_db, last_key );

    if (last) free(last_key.data);

	last = 0;

  }

  file_gononroot();


  if (!data.size || !data.data) {

    if (last) free(last_key.data);

    last = 0;

    return NULL;
  }


  retnode = dupe_find(data.data);

  last_key.data = data.data;
  last_key.size = data.size;

  last = 1;

  return retnode;
}

#endif

struct dupe_node *dupe_fnmatch(char *glob)
{
  DBT key, data;
  static struct dupe_node retnode;
  static char globbuf[256];
  static int count, searched;
  int r;
#if ( USE_NDBM_H ) || ( USE_GDBM_H )
  struct dupe_node *rnode;
#endif

  if (glob) { /* First call, load... */



    strcpy(globbuf, glob);
    count = 0;
    searched = 0;

  }

#if USE_NDBM_H

  while( ( rnode = dupe_seq(glob)) ) {
     memcpy(&retnode, rnode, sizeof(retnode));

#elif USE_GDBM_H

  while( ( rnode = dupe_seq(glob)) ) {

     memcpy(&retnode, rnode, sizeof(retnode));

#else


	 while(1) {


		 file_goroot();

		 r = (dupe_db->seq)(dupe_db, &key, &data, glob ? R_FIRST
							: R_NEXT);

		 file_gononroot();

		 if (r < 0) {
			 consolef("Error!!");
			 return NULL;
		 }

		 if (r > 0) {
			 consolef("No more entries \n");
			 return NULL;
		 }

#endif



    /* For some reason, R_NEXT loops continuously, so we put this hack in
       to stop it
       */
    searched++;



#if (USE_NDBM_H) || (USE_GDBM_H)
  if (!fnmatch(globbuf, retnode.filename, FNM_CASEFOLD)) {
#else
  if (!fnmatch(globbuf, data.data, FNM_CASEFOLD)) {
  memcpy(&retnode, data.data, sizeof(retnode));
#endif


      return &retnode;

    }

    glob = NULL;



  }

  consolef("dupe_fnmatch(): done.\n");
  return NULL; /* No more... */

}


void dupe_pathadd(char *path)
{

  if (dupe_num_path >= DUPE_MAX_PATH) {
    consolef("dupepath: DUPE_MAX_PATH hit, ignoring dupepath '%s'\n", path);
    return;
  }

  dupe_path_list[dupe_num_path++] = path;

}



int dupe_isdupepath(char *path)
{
  int i;

  consolef("dupe_isdupepath: checking %s\n", path);

  if (!strncmp(path, server_usechroot, strlen(server_usechroot)))
    path += strlen(server_usechroot);

  for (i = 0; i < dupe_num_path; i++) {

	  //    consolef("comparing %s and %s\n", dupe_path_list[i], path);

    if (!fnmatch(dupe_path_list[i], path, FNM_CASEFOLD))
      return 1;

  }

  return 0;

}



int dupe_isdupepathtop(char *path)
{
  int i;

  consolef("dupe_isdupepathtop: checking %s\n", path);

  if (!strncmp(path, server_usechroot, strlen(server_usechroot)))
    path += strlen(server_usechroot);

  for (i = 0; i < dupe_num_path; i++) {

	  //    consolef("comparing %s and %s\n", dupe_path_list[i], path);

    if (!fnmatch(dupe_path_list[i], path, FNM_CASEFOLD)) {

		//	consolef("Match - is it top level?\n");


		return 1;
	}

  }

  return 0;

}








































struct dupe_node *dupe_purge(time_t age)
{
  DBT key, data;
  static struct dupe_node retnode;
  static int count, searched;
  int r, purged = 0;
  char *glob = 0;
  time_t now, oldest = 0L;
#if defined ( USE_NDBM_H ) || defined ( USE_GDBM_H )
  struct dupe_node *rnode;
#endif


  time(&now);


  now -= age;  // remove the age from now time.


  if (glob) { /* First call, load... */

	  glob = (char *)1;
	  count = 0;
	  searched = 0;

  }

#ifdef USE_NDBM_H

  while( ( rnode = dupe_seq(glob)) ) {
	  memcpy(&retnode, rnode, sizeof(retnode));

#elif defined USE_GDBM_H

   while( ( rnode = dupe_seq(glob)) ) {

	   memcpy(&retnode, rnode, sizeof(retnode));

#else


	while(1) {


		r = (dupe_db->seq)(dupe_db, &key, &data, glob ? R_FIRST
						   : R_NEXT);


		if (r < 0) {
			break;
		}

		if (r > 0) {
			break;
		}

		memcpy(&retnode, data.data, sizeof(retnode));

#endif



		/* For some reason, R_NEXT loops continuously, so we put this hack in
		   to stop it
		*/
		searched++;



		// If this node old enough?
	if (now > retnode.time) {

		dupe_del(retnode.filename);

		purged++;

		if (!oldest || (retnode.time < oldest))
			oldest = retnode.time;

	}


    glob = NULL;



	} // while 1


	time(&now);


	consolef("dupe_purge: done - purged %d entries, oldest %s - %s",
			 purged, idletime(now - oldest), ctime(&oldest));



	return NULL; /* No more... */

}











#endif

