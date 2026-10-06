#if HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <time.h>

#if defined (POSIX) || defined (__FreeBSD__) || defined (__linux__)
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <string.h>

#include "lion.h"

#include "log.h"

#include "global.h"
#include "login.h"
#include "data.h"
#include "checkchild.h"
#include "check.h"
#include "fnmatch.h"
#include "sfv.h"

// We have a name space clash. Just misc would open lftpd misc, which 
// is no longer really in used, but we've not completely moved over so
// until that day, specify the path.
#include "../lion/src/misc.h"


//#define STAND_ALONE

#ifdef STAND_ALONE
#define consolef printf
#define checkchild_handler lion_userinput
struct check_node *check_node_head = NULL;
#define SFV_GetFileCRC(X) (unsigned long) (X)
#endif


// #define LOG_TO_FILE "/var/tmp/checkchild.log"

static THREAD_SAFE int checkchild_quit = 0;
static THREAD_SAFE lion_t *parent_handle = NULL;


// Do we only fork one tester at a time, or no limit...
// No longer used!
//#define CHECK_ONLYONE


struct checkchild_struct {
	char *name;
	unsigned int id;
	int type;     // SFV_TYPE_SFV, SFV_TYPE_MD5, ...
};

typedef struct checkchild_struct checkchild_t;


// We need to build requests up in a queue like thingy here.
// We will have a circular list, the size of list is in
// _allocated. Current index start is _start, and of course _end.
// We re-allocate bigger array if needed.
static THREAD_SAFE checkchild_t *checkchild_circular_list = NULL;
static THREAD_SAFE int checkchild_allocated = 0;
static THREAD_SAFE int checkchild_start = 0;
static THREAD_SAFE int checkchild_end = 0;
#define CHECKCHILD_CHUNKSIZE 100

// This one is assigned the current node testing.
static THREAD_SAFE checkchild_t *checkchild_current = NULL;



checkchild_t *checkchild_addnode(unsigned int id, char *file, int type)
{
	checkchild_t *newd;

	// Do we need to allocate more space ?
	if (!checkchild_circular_list ||
	    (checkchild_end + 1 == checkchild_start) ||
	    ((!checkchild_start) && (checkchild_end + 1 == checkchild_allocated))){
		
	  // Yep

	  printf("[checkchild %d] allocating space %d nodes\n",
		 getpid(),
		 checkchild_allocated);

	  newd = (checkchild_t *) realloc(checkchild_circular_list,
					  (checkchild_allocated + 
					   CHECKCHILD_CHUNKSIZE) * 
					  sizeof(checkchild_t));

	  if (!newd) return NULL;

	  checkchild_circular_list = newd;


	  // We also need to clean things up here...
	  // if Start is > End (ie, the list wraps) we need to copy
	  // the end chunk to the new end, and update Start.
	  if (checkchild_start > checkchild_end) {

	    printf("[checkchild %d] relocating..\n",
		   getpid());

	    // The ranges overlap when the tail is longer than CHUNKSIZE,
	    // so this must be memmove(), not memcpy().
	    memmove( &checkchild_circular_list[ checkchild_start +
					       CHECKCHILD_CHUNKSIZE ],
		    &checkchild_circular_list[ checkchild_start ],
		    sizeof(checkchild_t) *
		    (checkchild_allocated - checkchild_start));

	    checkchild_start += CHECKCHILD_CHUNKSIZE;

	  }

	  // realloc() may have moved the list, and current always points
	  // at the start node, so re-point it whether we relocated or not.
	  if (checkchild_current)
	    checkchild_current = &checkchild_circular_list[ checkchild_start ];

	  // Update allocated size.
	  checkchild_allocated += CHECKCHILD_CHUNKSIZE;
	  

	}

	// Insert us at _end

	newd = &checkchild_circular_list[ checkchild_end ];

	newd->name = strdup(file);
	newd->id = id;
	newd->type = type;

	printf("[checkchild %d] adding %d -> %u : %u : %s\n",
	       getpid(),
	       checkchild_end,
	       id,
	       type,
	       file);


	checkchild_end++;
	if (checkchild_end >= checkchild_allocated)
		checkchild_end = 0;


	return newd;


}



// Return the node start points to (unless we are empty).
void checkchild_assign_current(void)
{
  checkchild_t *result;

  // Empty?
  if (checkchild_start == checkchild_end) {

    checkchild_current = NULL;
    return; 
  }

  result = &checkchild_circular_list[checkchild_start];

  printf("[checkchild] popped %d -> %u : %u : %s\n",
	 checkchild_start,
	 result->id,
	 result->type,
	 result->name);
  
  
  checkchild_current = result;
  
}


// Release the node start points to (as we are done with it)
void checkchild_releasenode(void)
{
  checkchild_t *node;

  node = &checkchild_circular_list[checkchild_start];

  if (checkchild_current &&
      (node != checkchild_current))
    printf("[checkchild %d] internal error (node != current)!?\n",
	   getpid());


  if (checkchild_start == checkchild_end) {
    printf("[checkchild %d] error, list is empty when we've called releasenode\n",
	   getpid());
  }


  printf("[checkchild %d] Releasing %d -> %u : %u : %s\n",
	 getpid(),
	 checkchild_start,
	 node->id,
	 node->type,
	 node->name);

  SAFE_FREE(node->name);
  node->id = 0;



  checkchild_start++;

  if (checkchild_start >= checkchild_allocated)
    checkchild_start = 0;


  checkchild_current = NULL;

  printf("[checkchild] current NULL\n");

  // Attempt to re-assign it if we can...
  // AH! This calls assign, which calls process, which calls int/ext process
  // that does the task, that calls release, which calls assign, which...
  // So, lets make it NULL here, and sleep for one second. 
  // io_force_loop = 1; // No sleep.
  //	checkchild_assign_current();

}




void checkchild_process( char *line )
{
	char *token, *ar, *file;
	unsigned int id;
	int type;


	// file should be:
	// "id:type:/path/to/file name.txt"
	ar = line;

	token = misc_digtoken(&ar, ":\r\n");

	if (!token) {
		printf("[checkchild %d] parse error '%s'\n", 
		       getpid(), line);
		return;
	}

	id = atoi( token );


	token = misc_digtoken(&ar, ":\r\n");

	if (!token) {
		printf("[checkchild %d] parse error '%s'\n", 
		       getpid(), line);
		return;
	}

	type = atoi( token );


	file = ar;

	printf("[checkchild %d] parsed '%u:%u:%s'\n", 
	       getpid(), id, type, file);




	// Add this to testing queue.
	if (!checkchild_addnode(id, file, type)) {

		// failed with unknown
		lion_printf(parent_handle, "%u!12/Cannot allocate memory\n", id);
		return;

	}


}





void checkchild_processnode( void )
{
	struct check_node *c;

	if (!checkchild_current)
		return;

	printf("[checkchild %d] current set to %u : %s\n",
	       getpid(),
	       checkchild_current->id,
	       checkchild_current->name);



	// Check if it is an internal test, or, external.
	// Since we've forked we should only ever read this list, it could
	// change from under our feet. But, we never re-arrange the check list
	// in lftpd. (And we'd better not)
	for (c = check_node_head; c; c=c->next) {
			   
	  if (!fnmatch(c->ext, checkchild_current->name, FNM_CASEFOLD)) {
			
	    if ((c->exe == (char *)CHECK_INTERNAL)) {

	      checkchild_test_int( checkchild_current->id, 
				   checkchild_current->name,
				   checkchild_current->type);
	      return;

	    } else { // Do external...

	      checkchild_test_ext( checkchild_current->id, 
				   checkchild_current->name,
				   c->exe);
	      return;

	    } // external

	  } // casefold

	} // for

	// We usually do not get here, so assume it is processed
	printf("[checkchild %d] no matching test, skipping\n",
	       getpid());
	lion_printf(parent_handle, "%u!13/no match\n", 
		    checkchild_current->id);
	checkchild_releasenode();
}





//
// handler for the main parent pipe.
//
int checkchild_handler( lion_t *handle,
						void *user_data, int status, int size, char *line)
{

	switch( status ) {

	case LION_PIPE_RUNNING:  // We get this, tells us parent is alive
		break;

	case LION_PIPE_EXIT:     // parent died, or quit. So shall we.
	case LION_PIPE_FAILED:
		checkchild_quit = 1;
		break;

	case LION_INPUT:
		// New request from parent.
		printf("[checkchild %d] request '%s'\n", 
		       getpid(), line);

		checkchild_process( line );
		break;

	}

	return 0;

}



int checkchild_init( lion_t *parent, void *user_data, void *arg )
{

	consolef("[checkchild %d] running...\n", getpid());

#ifdef LOG_TO_FILE
	FILE *i;

	i = fopen(LOG_TO_FILE, "a");

	if (i) {

		dup2(fileno(i), 0);
		dup2(fileno(i), 1);
		dup2(fileno(i), 2);

		setvbuf(stdin,  (char *)NULL, _IOLBF, 0);
		setvbuf(stdout, (char *)NULL, _IOLBF, 0);
		setvbuf(stderr, (char *)NULL, _IOLBF, 0);

		fclose(i);
	}

#endif
	checkchild_allocated = 0;
	checkchild_start = 0;
	checkchild_end = 0;
	checkchild_current = NULL;

	parent_handle = parent;

	// We set our own handler for the child
	lion_set_handler(parent, checkchild_handler);

	// Lower our priority
	// FIXME! conf option
#ifdef POSIX
	consolef("Set priority 19: %d\n", 
			 setpriority(PRIO_PROCESS, 0, 10));
	setpriority(PRIO_PROCESS, 0, 10);
#endif


	

	while(!checkchild_quit) {

	  // Don't sleep if there is work queued and nothing in progress.
	  lion_poll(0, (!checkchild_current &&
			(checkchild_start != checkchild_end)) ? 0 : 1);

	  // Start the next node only when nothing is in progress. Internal
	  // tests finish (and release) inside processnode, external tests
	  // release when the tester exits. Calling processnode again while
	  // an external test runs would spawn the tester again.
	  if (!checkchild_current) {
	    checkchild_assign_current();
	    if (checkchild_current)
	      checkchild_processnode();
	  }

	}

	if (checkchild_start == checkchild_end)
	  return 0;

	// Save queue
	FILE *fp;
	char name[256];
	snprintf(name, sizeof (name), "/var/tmp/checklist%d.txt",
		 getpid());
	fp = fopen(name, "w");
	if (!fp)
	  return 0;
	printf("Dumping list to '%s'\n", name);

	do {
	  checkchild_assign_current();
	  if (checkchild_current == NULL)
	    break;
	  fprintf(fp, "%s\r\n", checkchild_current->name);
	  checkchild_releasenode();
	} while (1); 
	fclose(fp);
	return 0;

}


//
// *********************************************************************
//
// E X T E R N A L
//
// *********************************************************************
//
// External testing. This is accomplished by spawning a new program that
// does the actual testing. We then take the return value from the program
// and return it to parent.
// We have one option here. Do we enforce that these testers are serial, ie
// only one at a time (like that of internal) or do we spawn however many
// as needed. Only if they have SMP would it make sense to spawn more than
// one. 3 CPUs, and you could run two etc.
//


//
// Release the current node, but only if it is the one this tester was
// started for. Guards against a tester reporting twice (FAILED and EXIT),
// which would otherwise release (skip) the next queued file untested.
//
static void checkchild_release_if_current(unsigned long id)
{
	if (!checkchild_current || (checkchild_current->id != id)) {
		printf("[checkchild %d] tester %lu finished but is not current, ignoring\n",
		       getpid(), id);
		return;
	}
	checkchild_releasenode();
}


//
// Handler for all spawned tester programs.
//
int checkchild_ext_handler( lion_t *handle,
							void *user_data, int status, int size, char *line)
{
	unsigned long id = (unsigned long) user_data;
	static time_t start, end;

	switch( status ) {
		
	case LION_PIPE_RUNNING:  // We get this, tells us parent is alive
		printf("[checkchild] %lu tester running. \n", id);
		time(&start);
		break;

	case LION_PIPE_FAILED:
		printf("[checkchild] %lu failed. \n", id);

		// Use "!" to signify failure.
		lion_printf(parent_handle, "%u!%d/%s\n", id, size, line);
#ifdef CHECK_ONLYONE
		// Turn parent reading back on.
		lion_enable_read(parent_handle);
#endif
		checkchild_release_if_current(id);

		break;

	case LION_PIPE_EXIT:
		// make sure we have a received the return code.
		printf("[checkchild] %lu exit\n", id);

		if (size == -1) { 
			lion_want_returncode(handle);
			break;
		}

		printf("[checkchild] %lu exit -> %d. \n", id, size);

		// We have a return code.
		// return code
		// Use ":" to show success
		lion_printf(parent_handle, "%u:%d\n", id, size);

#ifdef CHECK_ONLYONE
		// Turn parent reading back on.
		lion_enable_read(parent_handle);
#endif
		time(&end);
		printf("[checkchild] test %lu, duration %lu.\n",
			   id, end - start);

		checkchild_release_if_current(id);
		break;


	case LION_INPUT:
		// Log text from programs?
		printf("[checkchild] %lu -> '%s'\n", id, line);
		break;

	}

	return 0;

}



void checkchild_test_ext( unsigned int id, char *file, char *program )
{
	char buf[1024];

	// Build the command to execute.
	snprintf(buf, sizeof(buf), "%s \"%s\"", program, file);


	//lion_printf(parent_handle, "%u:not implemented\n", id);

	consolef("executing '%s'\n", buf);

	// We can do this because we have FULFILL. Never returns NULL.
	lion_set_handler(
					 lion_system( buf, 1, LION_FLAG_FULFILL, 
								  (void *)id),
					 checkchild_ext_handler);

#ifdef CHECK_ONLYONE
	// If we deal only with one at a time, pause parent reading.
	// Alas, it is a while(getline) that calls us, so we will get more...
	// but we need lion alive too...
	printf("[checkchild] pausing parent\n");

	lion_disable_read( parent_handle );

#endif

}



void checkchild_test_int( unsigned int id, char *file, int type )
{
	int fd;
	unsigned long crc;
	char *md5;
	time_t before, after;

	fd = open(file, O_RDONLY
#ifdef O_BINARY
			  |O_BINARY
#endif
			  , 0400);

	if (fd < 0) {
	  int fail = errno;
	  printf("Failed to open %s %d\n",
		 file, fail);
		// Return failure
		lion_printf(parent_handle, "%u!%d:%s\n", id,
					fail, strerror(fail));

		checkchild_releasenode();
		return;
	}

	time(&before);
	switch(type) {
	case SFV_TYPE_SFV:
		crc = SFV_GetFileCRC(fd);
		lion_printf(parent_handle, "%u:%08lX\n", id, crc);
		break;
	case SFV_TYPE_MD5:
		md5 = SFV_GetFileMD5(fd);
		lion_printf(parent_handle, "%u:%s\n", id, md5);
		break;
	}
	time(&after);

	close(fd);


	printf("[checkchild] test %u, duration %lu\n", id, after - before);

	checkchild_releasenode();

}























#ifdef STAND_ALONE

unsigned int rond()
{
	static int fd = -2;
	unsigned long poo;

	if (fd == -2) {
		fd = open("/dev/urandom", O_RDONLY);
	}

	if (fd >= 0)
		read(fd, &poo, sizeof(poo));

	return poo;
}


int main(int argc, char **argv)
{
	int i, j;

	printf("[checkchild] exercise\n");

	for (j = 0; j < 1000; j++) {

	for (i = 0; i < 300; i++) {
		
		if ((rond()&512) == 512)
			checkchild_addnode(i, "nomatter");
		
		if ((i > 50) && (i < 166))
			checkchild_addnode(i, "nomatter");

		if ((rond()&128) == 128) 
			checkchild_releasenode();

		if (checkchild_current &&
			(checkchild_current != 
			&checkchild_circular_list[ checkchild_start ]))
			printf("CURRENT IS NOT START\n");

		if (!checkchild_current &&
			(checkchild_start != checkchild_end))
			printf("Current NULL BUT LIST NOT EMPTY\n");
		
	}

	printf("Releasing...\n");

	while(checkchild_start != checkchild_end)
		checkchild_releasenode();

	printf("bigloop\n");

	}
}

#endif



