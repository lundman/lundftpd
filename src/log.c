/*
 *
 * Log-file maintenence
 *
 * Jorgen Lundman, 1996.
 *
 */


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#define HAVE_STDARG_H 1
#endif

#if HAVE_STDARG_H
#  include <stdarg.h>
#  define VA_START(a, f)        va_start(a, f)
#else
#  if HAVE_VARARGS_H
#    include <varargs.h>
#    define VA_START(a, f)      va_start(a)
#  endif
#endif
#ifndef VA_START
#  warning "no variadic api"
#endif

#include <fcntl.h>

#include "lion.h"

#include "global.h"
#include "log.h"
#include "misc.h"


static FILE *log_filexfer = NULL;
static FILE *log_fileuser = NULL;
static FILE *log_fileadmin = NULL;

static int console_on = 0;


//#define DEBUG_LOG


#ifdef DEBUG
#define DEBUG_LOG
#endif

static char *log_cwd = NULL;




void log_load(void)
{

	// If we are to be able to rotate logs, we need to remember where we 
	// are/were incase of relative paths.

	// Windows do not have a fchdir function.
	if (!log_cwd) 
		log_cwd = getcwd(NULL, 1024); // this is never freed.

  if (server_logxfer)
    if (!(log_filexfer = fopen(server_logxfer, "a")))
      perror("Couldn't open logxfer, logging disabled: ");

  if (server_loguser)
    if (!(log_fileuser = fopen(server_loguser, "a")))
      perror("Couldn't open loguser, logging disabled: ");

  if (server_logadmin)
    if (!(log_fileadmin = fopen(server_logadmin, "a")))
      perror("Couldn't open logadmin, logging disabled: ");

#ifndef WIN33
  if (log_fileuser)
    setvbuf(log_fileuser, (char *)NULL, _IOLBF, 64);

  if (log_fileadmin)
    setvbuf(log_fileadmin, (char *)NULL, _IOLBF, 64);

  if (log_filexfer)
    setvbuf(log_filexfer, (char *)NULL, _IOLBF, 64);
#endif

}




void log_free(void)
{
	//  time_t tt = time(NULL);

  if (log_fileuser) 
    fclose(log_fileuser);

  if (log_fileadmin) 
    fclose(log_fileadmin);

  if (log_filexfer) 
    fclose(log_filexfer);

}


void log_rotate(void)
{
	struct tm *now;

	if (log_cwd)
		chdir(log_cwd);

	log_user("Rotating log file...\n");
	log_xfer("Rotating log file...\n");
	log_admin("Rotating log file...\n");

	log_free();

	now = localtime(&lion_global_time);

	if (server_logxfer) {
		snprintf(genbuf, sizeof(genbuf), "%s-%04u%02u%02u",
				 server_logxfer, 
				 now->tm_year + 1900,
				 now->tm_mon + 1,
				 now->tm_mday);
		rename(server_logxfer, genbuf);
	}
	if (server_loguser) {
		snprintf(genbuf, sizeof(genbuf), "%s-%04u%02u%02u",
				 server_loguser, 
				 now->tm_year + 1900,
				 now->tm_mon + 1,
				 now->tm_mday);
		rename(server_loguser, genbuf);
	}

	if (server_logadmin) {
		snprintf(genbuf, sizeof(genbuf), "%s-%04u%02u%02u",
				 server_logadmin, 
				 now->tm_year + 1900,
				 now->tm_mon + 1,
				 now->tm_mday);
		rename(server_logadmin, genbuf);
	}

	log_load();

}



#if HAVE_STDARG_H
void log_user(char const *fmt, ...)
#else
void log_user(fmt, va_alist)
     char const *fmt;
     va_dcl
#endif
{
  va_list ap;
  char msg[1024];

  if (!log_fileuser)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  fputs(msg, log_fileuser);
}

#if HAVE_STDARG_H
void log_admin(char const *fmt, ...)
#else
void log_admin(fmt, va_alist)
     char const *fmt;
     va_dcl
#endif
{
  va_list ap;
  char msg[1024];

  if (!log_fileadmin)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  fputs(msg, log_fileadmin);
}

#if HAVE_STDARG_H
void log_xfer(char const *fmt, ...)
#else
void log_xfer(fmt, va_alist)
     char const *fmt;
     va_dcl
#endif
{
  va_list ap;
  static char msg[1024];

  if (!log_filexfer)
    return;

  // This call writes 26 bytes to msg, including trailing \n and 0
  ctime_r(&lion_global_time, msg);

  // So we wipe out the \n when we...
  msg[24] = ' ';

  VA_START(ap, fmt);

  vsnprintf(&msg[25], sizeof(msg) - 26, fmt, ap);
  va_end(ap);

  fputs(msg, log_filexfer);
}


void log_console(int state)
{
  FILE *i;

#ifdef WIN32

   console_on = state;

   if (!state) return;


#ifdef DEBUG_LOOG
   i = freopen("debug.log", "w", stdout);
   i = freopen("debug.log", "w", stderr);
   i = freopen("debug.log", "r", stdin);
 #else
    //i = freopen("NUL", "w", stdout);
    //i = freopen("NUL", "w", stderr);
 //   i = freopen("NUL", "r", stdin);
#endif

	printf("Windows console redirected\n");

 
	return;

#else




  if (!state || (state == 69)) { /* No output, turn debugging logs off */


    console_on = 1;

#ifndef DEBUG_LOG

	i = fopen("/dev/null", "a");

#else

   // The sfv tester re-calls log_console with the secret number 69
	// so that we can seperate the logs, if logs are wanted.
    if (state != 69)
		i = fopen("debuglog", "a");
	else
		i = fopen("sfvdebuglog", "a");
    setvbuf(i, (char *)NULL, _IONBF, 0);

#endif

    if (!i) {
      fprintf(stderr, "Couldn't open /dev/null! (or NUL: on Windows)\n");
      exit(1);
    }

    /* Go daemon */
    if (state != 69) 
      if (fork()) exit(0);


    dup2(fileno(i), 0);
    dup2(fileno(i), 1);
    dup2(fileno(i), 2);



#ifdef DEBUG_LOG
    setvbuf(stdin,  (char *)NULL, _IOLBF, 0);
    setvbuf(stdout, (char *)NULL, _IOLBF, 0);
    setvbuf(stderr, (char *)NULL, _IOLBF, 0);
#endif

    if (state == 69) {
      setvbuf(stdin,  (char *)NULL, _IOLBF, 0);
      setvbuf(stdout, (char *)NULL, _IOLBF, 0);
      setvbuf(stderr, (char *)NULL, _IOLBF, 0);
    }


   fclose(i);

   setsid();

  } else { /* make out go to stdout... */

    if (!stdout) return ; /* no can do */

    console_on = 1;

  }

#endif

}


#if HAVE_STDARG_H
void consolef(char const *fmt, ...)
#else
void consolef(fmt, va_alist)
     char const *fmt;
     va_dcl
#endif
{
  va_list ap;
  char msg[1024];

  if (!console_on)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  fputs(msg, stdout);
 
#ifdef WIN32
  {
	  static last = NULL;

	  if (lion_global_time > last) {
		  last = lion_global_time;
	  }
		  fflush(stdout);
  }
#endif

}





