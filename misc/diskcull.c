#include <stdio.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#if __sun__
#include <sys/types.h>
#include <sys/statvfs.h>
extern __const char *__const sys_errlist[];
#endif

#include <inttypes.h>

#ifdef HPUX
#include <sys/vfs.h>

extern __const char *__const sys_errlist[];

#define seteuid(XX) setreuid( -1, (XX))

#endif


#ifndef IRIX
#include <sys/param.h>
#include <sys/mount.h>
#else
#include <sys/statfs.h>
#endif

#if defined ( LINUX ) || ( HPUX )
#include <sys/vfs.h>
#endif

#ifdef WIN32
#include <sys/vfs.h>
#define sys_errlist _sys_errlist 
#endif
  
#ifdef _SGI_SOURCE
#include <sys/statfs.h>
#endif



/*

Take a list of paths, for example, "/files/" which might have subdirectories
called "games" and "apps" each containing their own subdirectories and files.

This program will take a list of paths (Like "/files/") and a setting
which can be either in megs or % of total disk size.

It will check if the path has atleast X bytes free, or less than % full, of
disk space, and if not, recursively retrieve all directories under
"/files/", making sure it doesn't skip onto a different physical disk,
sorts in mtime order and delete's the oldest *leaf* directory.

Iterates until sufficient disk space is available.


This code was written by Jorgen Lundman <lundman@cranberry.co.uk>

device /ace 
  full 100M
; full 90%
  delete-log /ace/deletelog.txt
    delete-path pc/games/
    delete-path pc/apps/
    delete-path requests/
    delete-path unix/games/
    delete-path unix/apps/

    move-path incomming/games/ ../pc/games/
    move-path incomming/apps/ ../pc/apps/
*/





// READ THIS!
// If you just compile this program AS IS, you'll find it doesn't actually
// do ANYTHING. That's because the default is to be harmless, and not 
// wipe someone's entire system because they have not setup the right
// configuration.
//
//#define HARMLESS      /* don't actually call rm ! */

//#define PAUSE_B4_RM






// Leave the rest alone..


#define VERBOSE

#define TOPLEVEL  
/* Delete directories from top level only. The reason for this is before
 * it would go on sub-directories as well, and we got situation where it
 * wiped CD1 (and then it'd have enough diskspace again and stop. So
 * The directory and CD2 would remain, and as it'd just been modified - 
 * it would be considered new again. Not desirable.
 * This define therefore makes sure to delete from top level directories 
 * only.
 * As well as wipe files in root directory.
 */

/* #define DEBUG    / * Verbose debugging information, also, use -v */


#define MAX_FILESYSTEMS 16    /* Some max */
#define MAXDIRENT 10240

#define DEFAULT_CONF "diskcull.conf"



#define FLAG_FULL      1    /* Does it use a device-full trigger */
#define FLAG_PARTITION 2    /* or diskusage partition trigger... */
#define FLAG_TIMEDATE  4    /* or or time/date age...            */
#define FLAG_DIRCOUNT  8    /* or # of entries in dir trigger... */

#define METHOD_DELETE  1    /* Delete itmes */
#define METHOD_MOVE    2    /* Move items */


struct delete_entry_struct {
  unsigned char *device;

  unsigned int flags;

  unsigned int method;
  
  unsigned long long full_bytes;/* either this triggers full in bytes or... */
  unsigned int  full_percent; /* or when this % full */
  unsigned long long partition_bytes; /* Partition "full_bytes" if used */
  unsigned int  partition_percent; /* or when this % full */

  /* "full" variables */
  unsigned long long full_total;   /* Total diskspace on device */
  unsigned long long full_used;    /* disk used on device */
  unsigned long long full_free;    /* total - used */

  /* "partition" variables */
  unsigned long long usage_bytes; /* Bytes used in partition, IFF required */


  /* timedate trigger cariables */
  time_t time;                     /* Age trigger (in seconds) */

	unsigned int dircount;         /* Max entries in the directory */

  unsigned char *log_path; /* if any */

  struct delete_path_struct *paths;

  struct delete_entry_struct *next; /* if any */
};

struct delete_path_struct {
  unsigned char *path;

  unsigned char *destination;    /* For the move command */
  
  struct delete_path_struct *next;
};




struct delete_entry_struct *delete_entry_head = NULL;

static int num_entries;
static int start_entry;
static struct entry_struct *list_entries[MAXDIRENT];

static unsigned char conf_path[MAXPATHLEN];

static int verbose = 0;

static struct stat root_stat;

static unsigned long long diskusage = 0;


static time_t global_time;

char genbuf[1024];

struct entry_struct {
  char *name;
  char *root;
  int leaf;
  int toplevel;
  int file;
  int processed;
  struct stat sb;
  unsigned long long bytes;
};



void check(struct delete_entry_struct *);
void build_dir_tree(char *, char *);
void build_dir(char *, int);
void conf_read(void);
int deltree(const char *tree);
int dutree(const char *tree);


int main(int argc, char **argv)
{
  struct delete_entry_struct *runner;
  int i;


  time(&global_time);


  strcpy(conf_path, DEFAULT_CONF);

  /* I would use getopt() except not all system have it */

  for (i = 2; i <= argc; i++) {

    if (!strcasecmp(argv[i - 1], "-v"))
      verbose ^= 1;

    if (!strcasecmp(argv[i - 1], "-f")) {
      if (i >= argc) { 
	fprintf(stderr, "You must supply a filename with -f!\r\n");
	exit(-1);
      }

      strcpy(conf_path, argv[i]);
      i++;
    }
  }


  conf_read();



  for (runner = delete_entry_head; runner; runner = runner->next) {

    if (verbose)
#if defined ( IRIX ) || defined ( HPUX ) || defined (_SGI_SOURCE) || defined(__sun__)
      printf("Processing device '%s' : %llu %u\n", runner->device,
	     runner->full_bytes, runner->full_percent);
#elif defined (_OSF_SOURCE)
      printf("Processing device '%s' : %lu %u\n", runner->device,
	     runner->full_bytes, runner->full_percent);
#else
      printf("Processing device '%s' : %qu %u\n", runner->device,
	     runner->full_bytes, runner->full_percent);
#endif

    check(runner);
    
  }

  return 0;
}


#if __STDC__
void log(char *path, char const *fmt, ...)
#else
void log(path, fmt, va_alist)
  char *path;
  char *fmt;
  va_dcl
#endif
{
  va_list ap;
  char msg[1024];
  FILE *fd;
  
#if __STDC__
  va_start(ap, fmt);
#else
  va_start(ap);
#endif
  
  vsprintf(msg, fmt, ap);
  va_end(ap);

  if ((fd = fopen(path, "a"))) {

    fputs(msg, fd);

    fclose(fd);

    if (verbose) puts(msg);

  }

}


char *mystrcpy(char *s)
{
  char *r;

  r = malloc(strlen(s) + 1);
  if (!r) exit(1);

  strcpy(r, s);

  return r;

}

int sort_fn(const void *part_a, const void *part_b)
{
  /* This sorts in reverse time order... */

  if (
      (*((struct entry_struct **)part_a))->sb.st_mtime == 
      (*((struct entry_struct **)part_b))->sb.st_mtime 
      )
    return strcmp((*((struct entry_struct **)part_a))->name,
		  (*((struct entry_struct **)part_b))->name);
  else
    if (
        (*((struct entry_struct **)part_a))->sb.st_mtime <
        (*((struct entry_struct **)part_b))->sb.st_mtime 
        )
      return -1;
    else
      return 1;
}



char *age(time_t time)
{ /* This support functionw as written by George Shearer (Dr_Delete) */
  
  static char workstr[100];
  unsigned short int days=(time/86400),hours,mins,secs;
  hours=((time-(days*86400))/3600);
  mins=((time-(days*86400)-(hours*3600))/60);
  secs=(time-(days*86400)-(hours*3600)-(mins*60));
  
  workstr[0]=(char)0;
  if(days)
    sprintf(workstr,"%dd",days);
  if(hours)
    sprintf(workstr,"%s%s%dh",workstr,(workstr[0])?", ":"",hours);
  if(mins)
    sprintf(workstr,"%s%s%dm",workstr,(workstr[0])?", ":"",mins);
  if(secs)
    sprintf(workstr,"%s%s%ds",workstr,(workstr[0])?", ":"",secs);
  if (!days && !hours && !mins && !secs)
    sprintf(workstr,"0 seconds");
  
  return(workstr);
}

#if defined(__sun__)
#include <dlfcn.h>
#include <libzfs.h>
static libzfs_handle_t *(*_libzfs_init)(void);
static zfs_handle_t *(*_zfs_open)(libzfs_handle_t *, const char *, int);
static void (*_zfs_close)(zfs_handle_t *);
static uint64_t (*_zfs_prop_get_int)(zfs_handle_t *, zfs_prop_t);
static libzfs_handle_t *g_zfs;
static boolean_t

load_libzfs(void)
{
  void *hdl;
  
  if (_libzfs_init != NULL)
    return (g_zfs != NULL);
  
  if ((hdl = dlopen("libzfs.so", RTLD_LAZY)) != NULL) {
    _libzfs_init = (libzfs_handle_t *(*)(void))dlsym(hdl,
						     "libzfs_init");
    _zfs_open = (zfs_handle_t *(*)())dlsym(hdl, "zfs_open");
    _zfs_close = (void (*)())dlsym(hdl, "zfs_close");
    _zfs_prop_get_int = (uint64_t (*)())
      dlsym(hdl, "zfs_prop_get_int");
    
    if (_libzfs_init != NULL) {
      assert(_zfs_open != NULL);
      assert(_zfs_close != NULL);
      assert(_zfs_prop_get_int != NULL);
      
      g_zfs = _libzfs_init();
    }
  }
  
  return (g_zfs != NULL);
}



#include <sys/mnttab.h>
char *path2dataset(char *path)
{
  struct mnttab mp;
  FILE *fd;
  int longest = 0;
  char *best = NULL;

  fd = fopen("/etc/mnttab", "r");
  if (!fd) return NULL;

  while (!getmntent(fd, &mp)) {

    if (!strncmp(mp.mnt_mountp, path, strlen(mp.mnt_mountp))) {

      if (strlen(mp.mnt_mountp) > longest) {
	longest = strlen(mp.mnt_mountp);
	if (best) free(best);
	best = strdup(mp.mnt_special);
      }
    }
  }

  fclose(fd);
  
  return best;

}



static void
adjust_total_blocks(char *path, fsblkcnt64_t *total,
		    fsblkcnt64_t *avail,
		    uint64_t blocksize)
{
  char *dataset, *slash;
  int first = 1;
  uint64_t quota = 0;

  dataset = path2dataset(path);
  
  if (!load_libzfs())
    return;
  
  /*
   * We want to get the total size for this filesystem as bounded by any
   * quotas. In order to do this, we start at the current filesystem and
   * work upwards looking for the smallest quota.  When we reach the
   * pool itself, the quota is the amount used plus the amount
   * available.
   */
  if (!dataset)
    return;

  printf("mountpoint '%s' is at dataset '%s'\n",
	 path, dataset);
  
  slash = dataset + strlen(dataset);
  while (slash != NULL) {
    zfs_handle_t *zhp;
    uint64_t this_quota;
    
    *slash = '\0';
    
    zhp = _zfs_open(g_zfs, dataset, ZFS_TYPE_DATASET);
    if (zhp == NULL)
      break;

    /* true at first iteration of loop */
    if (first) {
      quota =  _zfs_prop_get_int(zhp, ZFS_PROP_USED);
      *avail = quota;
      quota = _zfs_prop_get_int(zhp, ZFS_PROP_REFQUOTA);
      if (quota == 0)
	quota = UINT64_MAX;
      first = 0;
    }
    
    this_quota = _zfs_prop_get_int(zhp, ZFS_PROP_QUOTA);
    if (this_quota && this_quota < quota)
      quota = this_quota;

    printf("this quota %llu: %llu\n", this_quota, quota);
    
    /* true at last iteration of loop */
    if ((slash = strrchr(dataset, '/')) == NULL) {
      uint64_t size;
      
      size = _zfs_prop_get_int(zhp, ZFS_PROP_USED) +
	_zfs_prop_get_int(zhp, ZFS_PROP_AVAILABLE);

      printf("and used + available is %llu + %llu\n", 
	     _zfs_prop_get_int(zhp, ZFS_PROP_USED),
	     _zfs_prop_get_int(zhp, ZFS_PROP_AVAILABLE));


      printf("new avail %llu\n", *avail);

      if (size < quota)
	quota = size;
    }
    
    _zfs_close(zhp);
  }
  
  /*
   * Modify total only if we managed to get some stats from libzfs.
   */
  if (quota != 0) {
    *total = quota / blocksize;
    printf("Adjusted to %llu / %llu\n", quota, blocksize);
  }
  free(dataset);
}
#endif





int enough_diskspace_full(struct delete_entry_struct *device)
{
#if defined __sun__
  struct statvfs sfsb;
#else
  struct statfs sfsb;
#endif
  unsigned long long free, avail, used, blksiz, total;
  
#ifdef _SGI_SOURCE
  if (statfs(device->device, &sfsb, sizeof(struct statfs), 0)) {
#elif defined _OSF_SOURCE
  if (statfs(device->device, &sfsb, sizeof(struct statfs))) {
#elif defined __sun__
  if (statvfs(device->device, &sfsb)) {
#else
  if (statfs(device->device, &sfsb)) {
#endif
    perror("stat()");
    printf("FATAL: Failed to stat root-device '%s'!\n", device->device);
    return -1;
  }


  used = (unsigned long long) sfsb.f_blocks - sfsb.f_bfree;
#if defined (_SGI_SOURCE)
  blksiz = sfsb.f_bsize;
  free = (unsigned long long) sfsb.f_bfree * blksiz;
  avail = used + (unsigned long long) sfsb.f_bfree;
#elif defined (__sun__)
  blksiz = sfsb.f_bsize;

  // avail is available block in totoal, ie, the TOTAL size
  total = sfsb.f_blocks * (unsigned long long) blksiz;
  //free  = sfsb.f_bfree  * (unsigned long long) blksiz;
  free  = sfsb.f_bavail * (unsigned long long) blksiz;
  avail = sfsb.f_blocks * (unsigned long long) blksiz;

  {
    fsblkcnt64_t atotal, aused;
    adjust_total_blocks(device->device, &atotal, &aused, 1);
    total = atotal;
    avail = atotal;
    printf("atotal %llu, aused %llu\n", atotal, aused);
    used = aused;
    free = total - used;
  }

  //used  = avail - free; 
  //rpool/disk02    available       1693308928      -
  //rpool/disk02    used    553431214080    -
  //rpool/disk02    quota   555124523008    local
  //root@backup:~/lftpd# 555124523008 - 553431214080 = 1693308928
  // atotal 555124523008, aused 1927653861376



  printf("total %llu: free %llu: avail %llu: used %llu. percent used %u\n",
	 total, free, avail, used,
	 (unsigned int)((used * 100) / avail));


#elif defined (_OSF_SOURCE)
  blksiz = 1024;
  free = sfsb.f_bavail * (unsigned long long) blksiz;
  avail = sfsb.f_blocks;
#else
  blksiz = sfsb.f_bsize;
  free = (unsigned long long) sfsb.f_bavail * blksiz;
  avail = used + (unsigned long long) sfsb.f_bavail;
  avail = avail * sfsb.f_bsize / blksiz;  // block size isn't always 1024.
#endif

#if 1
  printf("bavail %lu and bsize %lu\n",
		 (unsigned long) sfsb.f_bavail,
		 (unsigned long) sfsb.f_bsize);
#endif



  /* Fill in our variables if this is first time */
  if (!device->full_total) {
    device->full_total = total;
    device->full_used = used;
    device->full_free = avail;
  }

  if (verbose)
#if defined ( IRIX ) || defined ( HPUX ) || defined (_SGI_SOURCE) || defined(__sun__)
    printf("'%s' %llu free %llu total, percent %ld%%\n", 
	   device->device, free/blksiz, avail,
	   (unsigned int)((used * 100) / avail));
#elif defined (_OSF_SOURCE)
    printf("'%s' %lu free %lu total, percent %u%%\n", 
	   device->device, free/blksiz, avail,
	   (unsigned int)used * 100 / avail);
#else
    printf("'%s' %qu free %qu total, percent %lu%%\n", 
	   device->device, free/blksiz, avail,
	   (unsigned long)(used * 100 / avail));
#endif

    printf("if ((%llu full_bytes && (%llu free >= %llu full_bytes)) &&" 
	   "((%llu free) <= %llu avail)) return 1;\n",
	   device->full_bytes,free,device->full_bytes,free/blksiz,avail);

  if ((device->full_bytes && (free >= device->full_bytes)) && 
      ((free) <= avail)) return 1;

  if (device->full_percent && (((used * 100) / avail) <= device->full_percent))
    return 1;

  if (!device->full_bytes && !device->full_percent) 
    return 1; /*Just incase someone does something dumb*/
  printf("returning 0 (should delete)\n");
  return 0;

}

int enough_diskspace_partition(struct delete_entry_struct *device)
{


  /* Right, if we have not run before, check if used disk space on device
     is less than partition trigger, ie, it's impossible for partition to
     be too big, this is just an early out optimization. 
   * 
   * However, is this is not true, say there isn't enough space to force a
   * directory tree build to count disk usage
   *
   */

  if (!device->usage_bytes) {
    /* No scan performed yet */

    /* Call full check to get device information */
    enough_diskspace_full(device);

    if (!device->partition_bytes) { /* If bytes is not set, it must be a % */

      if (!device->partition_percent) {

	printf("MALFORMED 'partition' config. Bytes and %% both 0.\n");
	return 1;

      }

      device->partition_bytes = device->full_total * device->partition_percent
	/ 100;

      if (verbose)
#if defined ( IRIX ) || defined ( HPUX ) || defined (_SGI_SOURCE) || defined(__sun__)
	printf("partition: %d%% of %llu is %llu\n", device->partition_percent,
	       device->full_total, device->partition_bytes);
#elif defined (_OSF_SOURCE)
	printf("partition: %d%% of %lu is %lu\n", device->partition_percent,
	       device->full_total, device->partition_bytes);
#else
	printf("partition: %d%% of %qu is %qu\n", device->partition_percent,
	       device->full_total, device->partition_bytes);
#endif
      
    }


    if (device->full_used < device->partition_bytes) {

      if (verbose) {

#if defined ( IRIX ) || defined ( HPUX ) || defined (_SGI_SOURCE) || defined(__sun__)
	printf("full_used %llu and partition %llu\n", device->full_used,
	       device->partition_bytes);
#elif defined (_OSF_SOURCE)
	printf("full_used %lu and partition %lu\n", device->full_used,
	       device->partition_bytes);
#else
	printf("full_used %qu and partition %qu\n", device->full_used,
	       device->partition_bytes);
#endif


	printf("partition: early exit, device usage < partition trigger\n");

      }

      return 1; /* No scan required */

    }

    if (verbose)
      printf("partition: forcing scan\n");

    return 0;  /*force scan */
  }



  if (diskusage) { /* Amount deleted in last run */

    device->usage_bytes -= diskusage;

  }


  if (verbose)
#if defined ( IRIX ) || defined ( HPUX ) || defined (_SGI_SOURCE) || defined(__sun__)
    printf("partition: checking that %llu < %llu (disk_used < partition_trigger)\n",
#elif defined (_OSF_SOURCE)
    printf("partition: checking that %lu < %lu (disk_used < partition_trigger)\n",
#else
    printf("partition: checking that %qu < %qu (disk_used < partition_trigger)\n",
#endif  
	   device->usage_bytes, 
	   device->partition_bytes ? device->partition_bytes : 
	   device->full_total * device->partition_percent / 100);
  

  if (device->usage_bytes < device->partition_bytes)
    return 1;  /* No need to delete anymore */

  return 0; /* Delete */
}




/* 
 * Check the age of a directory entry. If it is older, or the same, as time
 * specified for device, return 1, else return 0.
 */

int old_enough(struct delete_entry_struct *device, 
	       struct entry_struct *dir_entry)
{
  
  if (verbose) printf("TimeDate check: %30s -> %s",
		      dir_entry->name, ctime(&(dir_entry->sb.st_mtime)));

  
  if (dir_entry->sb.st_mtime > global_time) {

    /* The dir is newer that when this program started, exit. */
    if (verbose) printf("Newer than now (eh? :) )\n");
    return 0;

  }


  if ( (global_time - dir_entry->sb.st_mtime) >= device->time) {

    printf("Too old (by %s)\n", age((global_time - dir_entry->sb.st_mtime)));
    return 1;

  }


  if (verbose) printf("New enough - exit.\n");

  return 0;

}


int enough_diskspace_dircount(struct delete_entry_struct *device)
{
	int count = 0;
	int i; 

    for (i = 0; i < num_entries; i++) {

		if (list_entries[i]->processed) continue;

#ifdef TOPLEVEL
		if (list_entries[i]->toplevel) count++;
#else
	    if (list_entries[i]->leaf) count++;
#endif
	}


	printf("Apparantly there are %d entires (%d) ...\n", count, 
		   device->dircount);
	
	if (count > device->dircount)
		return 0;
	
	
	return 1;
}




int enough_diskspace(struct delete_entry_struct *device)
{
  /* Tricky code... 
   *
   * If its "full"      trigger, check it
   * If its "partition" trigger, check it
   * If its "time/date" trigger, check it
   * If its "dircount"  trigger, check it
   *
   * if any are true, return false, otherwise, trickle down to true.
   *
   */

  if (device->flags & FLAG_FULL) {   /* Full trigger */

    if (!enough_diskspace_full(device)) 
      return 0; /* Not enough? delete... */

  }

  if (device->flags & FLAG_PARTITION) {   /* Full trigger */

    if (!enough_diskspace_partition(device)) 
      return 0; /* Not enough? delete... */

  }

  if (device->flags & FLAG_TIMEDATE) {   /* Full trigger */


    return 0; /* Always returns 0, checked later on. */

  }

  if (device->flags & FLAG_DIRCOUNT) {   /* Full trigger */

    if (!enough_diskspace_dircount(device)) 
      return 0; /* Not enough? delete... */

  }

  /* Kewl, no work required. */

  return 1;

}


void check(struct delete_entry_struct *device)
{
  int i, del, ret;
  struct delete_path_struct *runner;
  unsigned char rmpath[MAXPATHLEN], *rmpathl;
  time_t now;

  
  
  /* Check if any work needs to be done. -
	 This is an early-out situations, where we do a quick
	 test, this isn't approriate for some situations
  */

  if (!(device->flags & FLAG_DIRCOUNT) && enough_diskspace(device)) {

    return;

  } 


  /* grab top level stat */
  if (stat(device->device, &root_stat)) {
    perror("stat()");
    return;
  }

  if (verbose) 
    printf("'%s' is on device %08x.\n", device->device, root_stat.st_dev);

  if (verbose)
    printf("Building directory tree image...\n");


  /* recursive - build a tree of all paths */

  /* Zero all entries.. */
  start_entry = 0;
  num_entries = 0;
  diskusage = 0;

  for (runner = device->paths; runner; runner = runner->next) {

    if (verbose)
      printf("%s Building Delete-path '%s'\n", device->device, runner->path);

    sprintf(rmpath, "%s/%s", device->device, runner->path);

	// As it happens, if we are a dircount, or timestamp we really
	// dont need to recurse.

	if ((device->flags & FLAG_DIRCOUNT) || (device->flags & FLAG_TIMEDATE)) { 

		start_entry = num_entries;
		build_dir(rmpath, 1);

	} else {

		build_dir_tree(rmpath, NULL);

	}

  }

  device->usage_bytes = diskusage;

  if (verbose) {

    printf("Alright, I found %d directories.\n", num_entries);
    
#if defined ( IRIX ) || ( HPUX ) || (_SGI_SOURCE) || defined(__sun__)
    printf("%llu bytes %llu Mb\n", diskusage, diskusage / 1024);
#elif defined (_OSF_SOURCE)
    printf("%lu bytes %llu Mb\n", diskusage, diskusage / 1024);
#else
    printf("%qu bytes %qu Mb\n", diskusage, diskusage / 1024);
#endif
    
    printf("sorting...oldest first\n");
  }


  if (!diskusage) {

    if (verbose)
      printf("No diskusage found? - skipping..\n");

    return;
  }



  qsort(list_entries, num_entries, sizeof(struct entry_struct *), sort_fn);


#if 0
  del = 0;

  while( del < num_entries) {

    while((del < num_entries) && (strcmp(list_entries[0]->root,
					 list_entries[del]->root))) del++;
    
    if (del >= num_entries) break;

    printf("inroot %s %d\n", list_entries[del]->name, list_entries[del]->file);
    if (list_entries[del]->root) {
      printf("root is %s\n", list_entries[del]->root);
    }
    del++;
  }
  exit(0);
#endif



#if 1
  if (verbose)
    for (i = 0; i < num_entries; i++)
      printf(" %s/%s %s\n", list_entries[i]->root, list_entries[i]->name,
	     list_entries[i]->leaf?"(leaf)": 
	     list_entries[i]->toplevel?"(top)":"");
#endif

  if (verbose)
    printf("Processing device...\n");

  del = 0;



  /* Now, while there isn't enough diskspace, delete... */
  diskusage = 0; /* Zero diskusage for amount deleted */

  while(!enough_diskspace(device)) {

    sleep(1); /* don't chew 100% cpu */


    /* Advance to next delete-item. */
#ifdef TOPLEVEL
    while((del < num_entries) && (!list_entries[del]->toplevel)) del++;
#else
    while((del < num_entries) && (!list_entries[del]->leaf)) del++;
#endif


    if (del >= num_entries) break; /* no more to delete */



    /* If this is a timedate thing, check it's date. We only need to go
     * until first "new" file as it's sorted oldest first. */
    if (device->flags & FLAG_TIMEDATE) {
    
      if (!old_enough(device, list_entries[del])) {  

	/* This file is too new. */
	break;

      }

    }



    sprintf(rmpath, "%s/%s", list_entries[del]->root, list_entries[del]->name);




    /* DELETE DELETE DELETE */

    if (device->method == METHOD_DELETE) {
      
      /*    sprintf(genbuf, "rm -rf '%s'", rmpath);*/
      strcpy(genbuf, rmpath);
      
      if (verbose)
	printf("Deleting '%s'\n", rmpath);
      
      
      rmpathl = &rmpath[strlen(device->device)]; /* skip device path */
      
      if (strlen(rmpathl) >= 40) { /* truncate if it's too long */
	rmpathl[39] = '~';
	rmpathl[40] = 0;
      }
      
      diskusage = 0; /* Zero diskusage for amount deleted */



      if (list_entries[del]->file) {    /* it's a file */

#ifdef PAUSE_B4_RM
		  printf("PAUSE! Push return to delete:\n");
		  getchar();
#endif

	diskusage = list_entries[del]->sb.st_size;
	ret = unlink(genbuf);

      } else {

#ifdef PAUSE_B4_RM
		  printf("PAUSE! Push return to delete:\n");
		  getchar();
#endif



	ret = deltree(genbuf);

      }


      if (ret) {
	
#ifdef DEBUG
	printf("Failed to execute '%s'.\n", genbuf);
#endif
	
	if (device->log_path) log(device->log_path,
				  "%-40s : FAILED - %s\n", 
				  rmpathl,
				  sys_errlist[errno]);
	
      } else {
	
	time(&now);
	
	if (device->log_path) log(device->log_path, 
				  "%-40s : %s", 
				  rmpathl, ctime(&now));
	
	
      }
      
      
      if (verbose)
#if defined ( IRIX ) || ( HPUX ) || (_SGI_SOURCE) || defined(__sun__)
	printf("deltree removed %llu bytes\n", diskusage);
#elif defined (_OSF_SOURCE)
	printf("deltree removed %lu bytes\n", diskusage);
#else
        printf("deltree removed %qu bytes\n", diskusage);
#endif
    
    } else {
      
      
      /* MOVE MOVE MOVE */


      /* We need to find which path this move is refering to so we can
	 use the destination value.
	 */


		for (runner = device->paths; runner; runner = runner->next) {
	if (strstr(rmpath, runner->path)) break;
      }

      if (!runner) {
	if (verbose)
	  printf("Failed to find matching move-path for '%s' :(\n", rmpath);

	break; /* Stop processing */

      }

      
      sprintf(genbuf, "mv -f '%s' '%s'", rmpath, runner->destination);
      
      
      if (verbose)
	printf("Moving '%s'->'%s'\n", rmpath, runner->destination);
      
      
      rmpathl = &rmpath[strlen(device->device)]; /* skip device path */
      
      if (strlen(rmpathl) >= 40) { /* truncate if it's too long */
	rmpathl[39] = '~';
	rmpathl[40] = 0;
      }
      
      diskusage = 0; /* Zero diskusage for amount deleted */

#ifndef HARMLESS      
      if (system(genbuf)) {
	
#ifdef DEBUG
	printf("Failed to execute '%s'.\n", genbuf);
#endif
	
	if (device->log_path) log(device->log_path,
				  "%-40s : FAILED - %s\n", 
				  rmpathl,
				  sys_errlist[errno]);
	
      } else {
	
	time(&now);
	
	if (device->log_path) log(device->log_path, 
				  "%-40s : %s", 
				  rmpathl, ctime(&now));
	
	
      }
#endif
      
      sprintf(genbuf, "%s/%s", runner->destination, list_entries[del]->name);
      
      
      /* Build disk-usage of tree tree we moved, if required */
      if (device->flags & FLAG_PARTITION)
		  dutree(genbuf);
      
      
      if (verbose)
#if defined ( IRIX ) || ( HPUX ) || (_SGI_SOURCE) || defined(__sun__)
	printf("mvtree moved %llu bytes\n", diskusage);
#elif defined (_OSF_SOURCE)
	printf("mvtree moved %lu bytes\n", diskusage);
#else
      printf("mvtree moved %qu bytes\n", diskusage);
#endif
      
      
      
      
    }
    
    
	// We now remove this entry from the list by 'spacing' it out
	// ie - setting it to NULL.
	
	list_entries[del]->processed = 1;
    
    del++;
    
  }
  
  /* All done, free all! */
    for (i = 0; i < num_entries; i++) {
      free(list_entries[i]->name);
      free(list_entries[i]->root);
      free(list_entries[i]);
    }
    
    num_entries = 0; /* safety */
    
}


void build_dir_tree(char *root, char *path)
{
  char rpath[MAXPATHLEN];
  int end_entry, i, before_call;

  if (!path) { /* root of recursion */

    /* Build entries */
    start_entry = num_entries;

#if 0
    num_entries = 0;
#endif

    build_dir(root, 1);

    end_entry = num_entries;

  } else { /* recursion */

    start_entry = num_entries;

    build_dir(path, 0);

    end_entry = num_entries;

  }

  /* loop through all new entries add call each subdir */
  for (i = start_entry ; i < end_entry; i++) {

    if (list_entries[i]->file) continue; /* Skip files */
    
    if (!path) {
      sprintf(rpath, "%s/%s", root, list_entries[i]->name);
    } else {
      sprintf(rpath, "%s/%s", path, list_entries[i]->name);
    }

    /* if num_entries change after we call this, then it's not a leaf node */
    before_call = num_entries;

    build_dir_tree(root, rpath);

    list_entries[i]->leaf = (num_entries == before_call);

  }

}


void build_dir(char *path, int root)
{
  /* adds entries to list_entries and increments num_entries.
     only adds dirs and dirs with same dev as root_stat. */
  DIR *dirr;
  struct dirent *dp;
  struct entry_struct *new_dir;
  struct stat sb;


  if (!(dirr = opendir(path))) {
    printf("Failed opendir() on '%s' -- skipping\n", path);
    return;
  }


  while ((dp = readdir(dirr)) != NULL) {

    if (!strcmp(dp->d_name, "."))
      continue;
    
    if (!strcmp(dp->d_name, ".."))
      continue;

    if (num_entries >= MAXDIRENT) break; /* no more slots, don't crash.. */

    sprintf(genbuf, "%s/%s", path, dp->d_name);

    if (lstat(genbuf, &sb)) { /* failed? */ 
      printf("Couldn't stat %s?\n", genbuf);
      continue;
    }

    if ((sb.st_mode & S_IFREG) ||
	(sb.st_mode & S_IFDIR)) { /* File or dir, count bytes */

      diskusage += sb.st_size;

    }

    /* If it is top level directory we want to count files too. */
    if (root && (!(sb.st_mode & S_IFREG) && !(sb.st_mode & S_IFDIR))) continue;

    if (!root && !(sb.st_mode & S_IFDIR)) continue; /* not a dir */

    /* File and starts with a .? then skip */
    if ((sb.st_mode & S_IFREG) && *dp->d_name == '.') continue;

    if (sb.st_dev != root_stat.st_dev) {
      printf("Different physical device %s (%08x) -- skipping\n", genbuf,
	     sb.st_dev);
      continue;
    }

    /* ok, now we add... */
    if (!(new_dir = (struct entry_struct *) 
	  malloc(sizeof(struct entry_struct)))) {
      perror("malloc()");
      break;
    }

    memcpy(&new_dir->sb, &sb, sizeof(sb)); /* copy stat over */

    new_dir->name = mystrcpy(dp->d_name);
    new_dir->root = mystrcpy(path);
    new_dir->file = (sb.st_mode & S_IFREG) ? 1 : 0;
    new_dir->toplevel = root;
	new_dir->processed = 0;

    list_entries[num_entries++] = new_dir;

  } /* while */

  closedir(dirr);

}





/*
 *
 *
 *
 * Conf Reading Code  - Plain
 * 
 *
 *
 */


static int get_char(FILE *f)
{
  int c;
  c = getc(f);
  if (c=='#')
    while ((c = getc(f)) != '\n') ;
  if (c == EOF)
    return EOF;
  if (c < ' ' && (c != 1))
    c = ' ';
  return c;
}


int get_word(FILE *f, char *w)
{
  int p = 0;
  int c;
  c = get_char(f);
  while (c == ' ')
    c = get_char(f);
  if (c == EOF)
    return 0;
  while (p < 256 && c != ' ') {
    w[p++] = c;
    c = get_char(f);
    if (c == EOF)
      break;
  }
  w[p] = 0;
  return 1;
}

int get_keyword(FILE *in, char *buf)
{
  int c;
  int pos = 0;

  while ((c = get_char(in)) != EOF) {
    if (c=='"') {
      while ((c = get_char(in)) != EOF) {
        if (c == '\\') {
          buf[pos++] = get_char(in);
          continue;
        }
        if (c == '"') {
          c = get_char(in);
          break;
        }
        buf[pos++] = c;
      }
    }
    if (c=='{' || c=='}' || c==',' || c==';') {
      if (!pos) {
        buf[pos++] = c;
        buf[pos] = 0;
        return 0;
      }
      ungetc(c, in);
    }
    if (!isalnum(c) && c != ':' && c != '_' && c != '.' && 
	c != '/' && c != '|' && c!= '~' && c!='-' && c!='+') {
      buf[pos] = 0;
      return 0;
    }
    buf[pos++] = c;
  }
  return EOF;
}



char *get_string(FILE *f)
{
  char word[256], *w;

  if (get_keyword(f, word) != EOF)
    if ((w = (char *)malloc(strlen(word)+1))) {
      strcpy(w, word);
      return w;
    }

  return NULL;
}



void assign_full(struct delete_entry_struct *cd, char *word)
{

  /* Handle strings like "100M  100K  90% 2G 1T 10240000 */
  unsigned char *end;
  unsigned long long value;

  value = (unsigned long long) strtol(word, (char **)&end, 10);

  if (value <= 0) {
    printf("Invalid integer for 'full' keyword\n");

    cd->full_percent = 100; /* Don't wipe their entire system */

    return;
  }



  switch(tolower(*end)) {

  case '%':
    cd->full_percent = (unsigned int) value;
    return;

  case 0: /* that means bytes */
  case 'b':
    cd->full_bytes = value;
    return;

  case 'k':
    cd->full_bytes = value * 1024;
    return;

  case 'm':
    cd->full_bytes = value * 1024 * 1024;
    return;

  case 'g':
    cd->full_bytes = value * 1024 * 1024 * 1024;
    return;

  case 't':
    cd->full_bytes = value * 1024 * 1024 * 1024 * 1024; 
    return;

  default:
    printf("Unknown quantifier '%s'\n", end);

  }

}


void assign_partition(struct delete_entry_struct *cd, char *word)
{

  /* Handle strings like "100M  100K  90% 2G 1T 10240000 */
  unsigned char *end;
  unsigned long long value;

  value = (unsigned long long) strtol(word, (char **)&end, 10);

  if (value <= 0) {
    printf("Invalid integer for 'partition' keyword\n");

    cd->partition_percent = 100; /* Don't wipe their entire system */

    return;
  }



  switch(tolower(*end)) {

  case '%':
    cd->partition_percent = value;
    return;

  case 0: /* that means bytes */
  case 'b':
    cd->partition_bytes = value;
    return;

  case 'k':
    cd->partition_bytes = value * 1024;
    return;

  case 'm':
    cd->partition_bytes = value * 1024 * 1024;
    return;

  case 'g':
    cd->partition_bytes = value * 1024 * 1024 * 1024;
    return;

  case 't':
    cd->partition_bytes = value * 1024 * 1024 * 1024 * 1024; 
    return;

  default:
    printf("Unknown quantifier '%s'\n", end);

  }

}




void assign_timedate(struct delete_entry_struct *cd, char *word)
{

  /* Handle strings like "100M  100K  90% 2G 1T 10240000 */
  unsigned char *end;
  unsigned long value;

  value = (unsigned long) strtol(word, (char **)&end, 10);

  if (value <= 0) {
    printf("Invalid integer for 'timedate' keyword\n");

    cd->time = -1; /* Don't wipe their entire system */

    return;
  }



  switch(tolower(*end)) {

  case 0: /* that means seconds */
  case 's':
    cd->time = value;
    return;

  case 'm':
    cd->time = value * 60;
    return;

  case 'h':
    cd->time = value * 60 * 60;
    return;

  case 'd':
    cd->time = value * 24 * 60 * 60;
    return;

  case 'w':
    cd->time = value * 7 * 24 * 60 * 60;
    return;

  default:
    printf("Unknown quantifier '%s'\n", end);

  }

}




void conf_read(void) 
{
  FILE *fd;
  int save_id = -1;
  char keyword[256];
  struct delete_entry_struct *current_device = NULL;
  struct delete_path_struct *current_path = NULL;


  if (verbose) printf("Reading conf '%s'\n", conf_path);

  if (getuid() != geteuid()) {
    save_id = geteuid();
    seteuid(getuid());
  }


  fd = fopen(conf_path, "r");

  if (!fd) {
    perror("Failed to open conf file");
    exit(-1);
  }



#ifdef LOCK_EX
  // Lock the file to ensure we only have one process working on this cfg.

  if ( flock( fileno( fd ), LOCK_EX | LOCK_NB ) ) {

	  printf("Config file '%s' is locked.\nPossibly another mirror process is already running?\n\n", conf_path);

	  exit(0);
  }

#endif




  /* Parse conf file */

  while(get_word(fd, keyword)) {
    int l = strlen(keyword);

    if (!strncasecmp(keyword, "device", l)) {

      current_device = (struct delete_entry_struct *) 
	malloc(sizeof(struct delete_entry_struct));

      if (!current_device) {
	fprintf(stderr, "Out of memory!\r\n");
	exit(-1);
      }

      memset(current_device, 0, sizeof(*current_device));

      current_device->next = delete_entry_head;
      delete_entry_head = current_device;

      /* Read path */
      current_device->device = get_string(fd);

      if (verbose) printf("Defining device '%s'\n", current_device->device);


    } else if (!strncasecmp(keyword, "full", l)) {

      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }
      
      get_word(fd, keyword); /* read in quantity */
      assign_full(current_device, keyword);

      current_device->flags |= FLAG_FULL;


#if defined ( IRIX ) || ( HPUX ) || (_SGI_SOURCE) || defined(__sun__)
      if (verbose) printf("Full-trigger set to %llu bytes or %u%%\n", 
			  current_device->full_bytes, 
			  current_device->full_percent);
#elif defined (_OSF_SOURCE)
      if (verbose) printf("Full-trigger set to %lu bytes or %u%%\n", 
			  current_device->full_bytes, 
			  current_device->full_percent);
#else
      if (verbose) printf("Full-trigger set to %qu bytes or %u%%\n", 
			  current_device->full_bytes, 
			  current_device->full_percent);
#endif

    } else if (!strncasecmp(keyword, "dircount", l)) {

      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }
      
      get_word(fd, keyword); /* read in quantity */
      assign_full(current_device, keyword);

	  if (!current_device->full_bytes) {
		  printf("Dircount-trigger with no count given?\n");
		  continue;
	  }

	  current_device->dircount = current_device->full_bytes;
	  current_device->full_bytes = 0;


      current_device->flags |= FLAG_DIRCOUNT;

      if (verbose) printf("Dircount-trigger set to %u items\n", 
			  current_device->dircount);




    } else if (!strncasecmp(keyword, "dircount", l)) {

      if (!current_device) {
		  printf("Keyword without defining 'device' parse error\n");
		  continue;
      }
      
      get_word(fd, keyword); /* read in quantity */
      assign_timedate(current_device, keyword);

      current_device->flags |= FLAG_TIMEDATE;

      if (verbose) printf("TimeDate-trigger set to age %s\n", 
			  age(current_device->time));




    } else if (!strncasecmp(keyword, "partition", l)) {

      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }
      
      get_word(fd, keyword); /* read in quantity */
      assign_partition(current_device, keyword);

      current_device->flags |= FLAG_PARTITION;

#if defined ( IRIX ) || ( HPUX ) || (_SGI_SOURCE) || defined(__sun__)
      if (verbose) printf("Partition-trigger set to %llu bytes or %u%%\n", 
			  current_device->partition_bytes,
			  current_device->partition_percent);
#elif defined (_OSF_SOURCE)
      if (verbose) printf("Partition-trigger set to %lu bytes or %u%%\n", 
			  current_device->partition_bytes,
			  current_device->partition_percent);
#else
      if (verbose) printf("Partition-trigger set to %qu bytes or %u%%\n", 
			  current_device->partition_bytes,
			  current_device->partition_percent);
#endif

    } else if (!strncasecmp(keyword, "delete-log", l)) {


      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }

      current_device->log_path = get_string(fd);

      if (verbose) printf("Delete-log set to '%s'\n",current_device->log_path);



    } else if (!strncasecmp(keyword, "delete-path", l)) {

      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }

      
      if (!current_device->method)
	current_device->method = METHOD_DELETE;
      else if (current_device->method != METHOD_DELETE) {
	printf("Method 'delete' and 'move' defined both defined. stop.\n");
	exit(0);
      }


      current_path = (struct delete_path_struct *)
	malloc(sizeof(struct delete_path_struct));

      if (!current_path) {
	fprintf(stderr, "Out of memory\r\n");
	exit(-1);
      }

      current_path->path = get_string(fd);

      current_path->next = current_device->paths;
      current_device->paths = current_path;


      if (verbose) printf("Defining delete-path '%s'\n", current_path->path);



    } else if (!strncasecmp(keyword, "move-path", l)) {

      if (!current_device) {
	printf("Keyword without defining 'device' parse error\n");
	continue;
      }

      
      if (!current_device->method)
	current_device->method = METHOD_MOVE;
      else if (current_device->method != METHOD_MOVE) {
	printf("Method 'delete' and 'move' defined both defined. stop.\n");
	exit(0);
      }


      current_path = (struct delete_path_struct *)
	malloc(sizeof(struct delete_path_struct));

      if (!current_path) {
	fprintf(stderr, "Out of memory\r\n");
	exit(-1);
      }


      current_path->path = get_string(fd);
      current_path->destination = get_string(fd);


      current_path->next = current_device->paths;
      current_device->paths = current_path;


      if (verbose) printf("Defining move-path '%s'->'%s'\n", 
			  current_path->path, current_path->destination);


    } else {

      printf("Unknown keyword '%s'\n", keyword);

    }


  }


  fclose(fd);


  if (save_id >= 0) seteuid(save_id);

}






/*
 *
 * Lundy's recursive delete function
 *
 * rm -rf
 * DelTree
 *
 * int deltree(tree_name);
 * Deletes all files and directories starting at root "tree_name"
 *
 * Returns 0 for success, !0 otherwise.
 *
 * Because this leaves parentual DIR *'s open, this will fail if the depth
 * is large enough for maximum open filedescriptors.
 *
 */



int deltree(const char *tree)
{
  /* For all objects in this directory
   * ... if its a file, delete
   * ... if it's a directory, call deltree()
   */
  struct stat sb;
  DIR *dirp;
  struct dirent *dp;


  dirp = opendir(tree);

  if (!dirp) {
#ifdef VERBOSE
    perror("deltree: Failed to open directory");
#endif
    return -1;    /* ERROR_OPENING_DIR */
  }

  if (chdir(tree)) {
#ifdef VERBOSE
    perror("deltree: Couldn't CD to subdirectory");
#endif
    return -3;    /* COULD_NOT_CWD */
  }


  while ((dp = readdir(dirp)) != NULL) {

    if (!strcmp(dp->d_name, "."))   /* Skip this dir */
      continue;

    if (!strcmp(dp->d_name, ".."))   /* Skip this dir */
      continue;

    if (lstat(dp->d_name, &sb)) {
#ifdef VERBOSE
      perror("deltree: Couldn't stat directory object");
#endif
      closedir(dirp);
      return -2;   /* FAILED_STAT */
    }


    /* Count bytes removed */
    if ((sb.st_mode & S_IFDIR) || (sb.st_mode & S_IFREG)) {

      diskusage += sb.st_size;

    } 



    if (sb.st_mode & S_IFDIR) {  /* Shit, it's a dir */

      if (deltree(dp->d_name)) {
#ifdef VERBOSE
	perror("deltree: Deletion of subdirectory tree failed");
#endif
	closedir(dirp);
	return -6;  /* DIRTREE_DELETE_FAILED */
      }

    } else {  /* It's a file */

#ifndef HARMLESS
      if (remove(dp->d_name)) {
#ifdef VERBOSE
	perror("deltree: Failed to delete file");
#endif
	return -7;    /* FILE_DELETE_FAILED */
      }
#endif /* HARMLESS */
      
    }
  }

  /* Done with this tree */

  (void)closedir(dirp);

  if (chdir("..")) {
#ifdef VERBOSE
    perror("deltree: Couldn't CD to parent");
#endif
    return -4;    /* NO_PARENT_DIR */
  }

#ifndef HARMLESS
  if (rmdir(tree)) {
#ifdef VERBOSE
    perror("deltree: Could not delete directory");
#endif
    return -5; /* DELETE_ERROR */
  }
#endif

  return 0;    /* SUCCESS (fucken finally) */

}





/*
 *
 * Lundy's recursive du function
 *
 * du
 * DUTree
 *
 * int dutree(tree_name);
 * Counts disk usage 
 *
 * Returns 0 for success, !0 otherwise.
 *
 */



int dutree(const char *tree)
{
  /* For all objects in this directory
   * ... if its a file, delete
   * ... if it's a directory, call deltree()
   */
  struct stat sb;
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(tree);

  if (!dirp) {
#ifdef VERBOSE
    perror("dutree: Failed to open directory");
#endif
    return -1;    /* ERROR_OPENING_DIR */
  }

  if (chdir(tree)) {
#ifdef VERBOSE
    perror("dutree: Couldn't CD to subdirectory");
#endif
    return -3;    /* COULD_NOT_CWD */
  }


  while ((dp = readdir(dirp)) != NULL) {

    if (!strcmp(dp->d_name, "."))   /* Skip this dir */
      continue;

    if (!strcmp(dp->d_name, ".."))   /* Skip this dir */
      continue;

    if (lstat(dp->d_name, &sb)) {
#ifdef VERBOSE
      perror("dutree: Couldn't stat directory object");
#endif
      closedir(dirp);
      return -2;   /* FAILED_STAT */
    }


    /* Count bytes removed */
    if ((sb.st_mode & S_IFDIR) || (sb.st_mode & S_IFREG)) {

      diskusage += sb.st_size;

    } 



    if (sb.st_mode & S_IFDIR) {  /* Shit, it's a dir */
      
      if (dutree(dp->d_name)) {
#ifdef VERBOSE
	perror("dutree: Deletion of subdirectory tree failed");
#endif
	closedir(dirp);
	return -6;  /* DIRTREE_DELETE_FAILED */
      }
      
    }
    
  }
  
  /* Done with this tree */
  
  (void)closedir(dirp);
  
  if (chdir("..")) {
#ifdef VERBOSE
    perror("dutree: Couldn't CD to parent");
#endif
    return -4;    /* NO_PARENT_DIR */
  }

  return 0;    /* SUCCESS (fucken finally) */
  
}


