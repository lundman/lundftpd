#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef IRCBOT

#ifndef WITH_SSL
#error "IRCBOT is enabled but WITH_SSL is NOT. Either disable IRCBOT support, or enable WITH_SSL"
#endif



#include <stdio.h>
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

#include <string.h>

#include <time.h>
#include <stdlib.h>

#include <openssl/blowfish.h>

#include "global.h"
#include "log.h"
#include "irc.h"

#include "login.h"
#include "misc.h"

#include "data.h"

//static ircbot_t bot;
static int ircbot_enabled = 0;
extern time_t server_uptime;



//
// Technically, we could just have one lion_t handle, and send everything from
// it. But since I know SOMEONE has plans for two way communication, and
// lion_udp will eventually let you bind to multiple hosts from one udp port
// (but multiple handles) I thought I'd be nice...
//
// Open a lion_udp
// for each host/port, bind a new handle to it.
// close the original udp (optionally, unless we want to listed for anony data)
// then handler gets calls per host:port. Or we just use lion_printf.


struct ircbot_struct {
	lion_t *handle;
	char *host;
	int port;
	char *pass;
	int active;

	// We need a IDEA context.
	BF_KEY key;

	struct ircbot_struct *next;
};

typedef struct ircbot_struct ircbot_s;

static ircbot_s *ircbot_head = NULL;


void irc_input_cmd(ircbot_s *node, char *line);






int irc_handler( lion_t *handle, void *user_data,
					int status, int size, char *line)
{
	ircbot_s *node = (ircbot_s *)user_data;

	//	consolef("[irc] handler: %p->%p %d %d\n", handle, user_data,
	//   status, size);

	switch(status) {

	case LION_CONNECTION_LOST:

		// Master socket?
		if (!node) {
			consolef("[irc] failed to open UDP socket: %d:%s\n", status, line);
			return 0;
		}

		consolef("[irc] lost udp bind %p %d:%s\n", node, status, line);
		node->handle = NULL;
		break;

	case LION_CONNECTION_CLOSED:

		consolef("[irc] closed %p/%p\n", handle, node);
		if (node)
			node->handle = NULL;
		break;

	case LION_CONNECTION_CONNECTED:

		consolef("[irc] armed %p/%p\n", handle, node);
		break;


		// THESE ARE ONLY USED IF WE ALLOW BI-DIRECTIONAL COMMUNICATION
	case LION_INPUT:
		consolef("[irc] input '%s'\n", line);
	case LION_BINARY:
		consolef("[irc] input from %p/%p size %d\n", handle, node, size);

		if (server_irccmds) {
			unsigned char decoded[1500];
			unsigned char ivec[8] = { 0,0,0,0,0,0,0,0 };
			int num = 0;

			if ((size > 0) && (size < sizeof(decoded))) {

				//memset(decoded, 0, sizeof(decoded));
				BF_cfb64_encrypt((unsigned char *)line, decoded, size,
                                 &node->key, ivec, &num,
								 BF_DECRYPT);

				decoded[size] = 0;

				consolef("which is '%s'\n",
						 decoded);

				irc_input_cmd(node, (char *)decoded);
			}
		}

		break;

	case LION_BUFFER_USED:
	case LION_BUFFER_EMPTY:
		consolef("[irc] buffering events are ignored...\n");
		break;

	default:
		break;
	}

	return 0;

}















void irc_addbot(char *host, int port, char *pass)
{
	ircbot_s *newd;

	//	consolef("[irc] conf bot %s:%d\n", host, port);

	newd = (ircbot_s *) malloc(sizeof( *newd));
	if (!newd) return;

	memset(newd, 0, sizeof(*newd));

	newd->next = ircbot_head;
	ircbot_head = newd;

	newd->host = strdup(host);
	//newd->pass = strdup(pass); // we don't need it as we keep the BK_key
	newd->port = port;

	//	consolef("[irc] setting BF_KEY\n");
	// Set up the BF key
	BF_set_key(&newd->key, strlen(pass), (unsigned char *)pass);

}



void irc_init(void)
{
  ircbot_s *runner;
  lion_t *handle;
  int port;


  consolef("[irc] src_port set to %d\n", server_irc_src_port);

  // Do we want to bind a local port?
  port = server_irc_src_port; // pick one
  handle = lion_udp_new(&port,
						server_bindif ? lion_addr(server_bindif) : 0,
						LION_FLAG_FULFILL, NULL);

  // The udp_bind handles inherit the same handler.
  lion_set_handler(handle, irc_handler);

  // They also inherit binary setting.
  lion_setbinary(handle);


  for( runner = ircbot_head; runner; runner = runner->next ) {

	  if (!runner->host) continue;

	  consolef("[IrcBot] Readying host %s port %d (local %d)\n", runner->host,
			   runner->port, port);

	  runner->handle = lion_udp_bind( handle,
									  lion_addr(runner->host), runner->port,
									  (void *)runner);
	  runner->active = 1;
	  ircbot_enabled = 1;

  }

  // Release the anonymous udp socket, we don't need or care for it. Nor
  // do we want to receive anything (from anything except that which we confed)
  lion_close( handle );

}

void irc_free(void)
{
  ircbot_s *runner, *next;

  if (ircbot_enabled) {

	  for( runner = ircbot_head; runner; runner = next ) {

		  if (runner->handle) {
			  lion_close( runner->handle );
			  runner->handle = NULL;
		  }

		  next = runner->next;
		  free(runner);

	  }

	  ircbot_head = NULL;

  }

  ircbot_enabled = 0;

}


#ifdef WIN32
// The include login.h, includes winsock2.h which includes windows.h and this seems
// to break varargs, so we need to patch around it
#define __STDC__ 1
#endif




#if HAVE_STDARG_H
void irc_announce(char const *fmt, ...)
#else
void irc_announce(fmt, va_alist)
	char const *fmt;
	va_dcl
#endif
{
  ircbot_s *runner;
  static char msg[1500];
  static unsigned char encoded[1500];
  unsigned char ivec[8] = { 0,0,0,0,0,0,0,0 };
  int len, num;
  va_list ap;

  if (!ircbot_enabled)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  len = strlen(msg);

  /* Should check errorcodes */
  for( runner = ircbot_head; runner; runner = runner->next )  {

	  if (runner->active && runner->handle) {

		  //void BF_cfb64_encrypt(const unsigned char *in, unsigned char *out,
          //      long length, BF_KEY *schedule, unsigned char *ivec, int *num,
          //      int enc);
		  num = 0;
		  BF_cfb64_encrypt((unsigned char *)msg, encoded, len,
                           &runner->key, ivec, &num,
						   BF_ENCRYPT);

		  lion_output(runner->handle, (char *)encoded, len);

	  }
	}

}





#if HAVE_STDARG_H
void irc_announce_one(ircbot_s *node, char const *fmt, ...)
#else
void irc_announce(node, fmt, va_alist)
	 ircbot_s *node;
	char const *fmt;
	va_dcl
#endif
{
  static char msg[1500];
  static unsigned char encoded[1500];
  unsigned char ivec[8] = { 0,0,0,0,0,0,0,0 };
  int len, num;
  va_list ap;

  if (!ircbot_enabled)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  len = strlen(msg);

  /* Should check errorcodes */
  if (node->active && node->handle) {

	  num = 0;
	  BF_cfb64_encrypt((unsigned char *)msg, encoded, len, &node->key,
                       ivec, &num,
					   BF_ENCRYPT);

	  lion_output(node->handle, (char *)encoded, len);

	}

}






#if HAVE_STDARG_H
void ircbot_store_documentf(char *doc, char const *fmt, ...)
#else
void ircbot_store_documentf(doc, fmt, va_alist)
     char *doc;
     char const *fmt;
     va_dcl
#endif
{
  ircbot_s *runner;
  va_list ap;
  static char msg[1024];

  if (!ircbot_enabled)
    return;

  VA_START(ap, fmt);

  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);


  /* Should check errorcodes */
  for( runner = ircbot_head; runner; runner = runner->next )
	  if (runner->active)
		  //		  ircbot_store_document(&runner->bot, doc, msg);
		  ;
}


void irc_topup(void)
{
  if (!ircbot_enabled) return;
#if 0
  ircbot_store_documentf("topup",
                         "\x16     DAY        MB     WEEK        MB     MONTH       MB     ALLTIME      MB  \x16\r\n"
                         "\x16 01 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 02 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 03 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 04 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 05 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 06 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 07 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 08 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 09 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 10 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         , global_tubd[0] ? global_tuud[0] : (unsigned char *)"-", global_tubd[0] / 1048576.0, global_tubw[0] ? global_tuuw[0] : (unsigned char *)"-", global_tubw[0] / 1048576.0, global_tubm[0] ? global_tuum[0] : (unsigned char *)"-", global_tubm[0] / 1048576.0, global_tub[0] ? global_tuu[0] : (unsigned char *)"-", global_tub[0] / 1048576.0
                         , global_tubd[1] ? global_tuud[1] : (unsigned char *)"-", global_tubd[1] / 1048576.0, global_tubw[1] ? global_tuuw[1] : (unsigned char *)"-", global_tubw[1] / 1048576.0, global_tubm[1] ? global_tuum[1] : (unsigned char *)"-", global_tubm[1] / 1048576.0, global_tub[1] ? global_tuu[1] : (unsigned char *)"-", global_tub[1] / 1048576.0
                         , global_tubd[2] ? global_tuud[2] : (unsigned char *)"-", global_tubd[2] / 1048576.0, global_tubw[2] ? global_tuuw[2] : (unsigned char *)"-", global_tubw[2] / 1048576.0, global_tubm[2] ? global_tuum[2] : (unsigned char *)"-", global_tubm[2] / 1048576.0, global_tub[2] ? global_tuu[2] : (unsigned char *)"-", global_tub[2] / 1048576.0
                         , global_tubd[3] ? global_tuud[3] : (unsigned char *)"-", global_tubd[3] / 1048576.0, global_tubw[3] ? global_tuuw[3] : (unsigned char *)"-", global_tubw[3] / 1048576.0, global_tubm[3] ? global_tuum[3] : (unsigned char *)"-", global_tubm[3] / 1048576.0, global_tub[3] ? global_tuu[3] : (unsigned char *)"-", global_tub[3] / 1048576.0
                         , global_tubd[4] ? global_tuud[4] : (unsigned char *)"-", global_tubd[4] / 1048576.0, global_tubw[4] ? global_tuuw[4] : (unsigned char *)"-", global_tubw[4] / 1048576.0, global_tubm[4] ? global_tuum[4] : (unsigned char *)"-", global_tubm[4] / 1048576.0, global_tub[4] ? global_tuu[4] : (unsigned char *)"-", global_tub[4] / 1048576.0
                         , global_tubd[5] ? global_tuud[5] : (unsigned char *)"-", global_tubd[5] / 1048576.0, global_tubw[5] ? global_tuuw[5] : (unsigned char *)"-", global_tubw[5] / 1048576.0, global_tubm[5] ? global_tuum[5] : (unsigned char *)"-", global_tubm[5] / 1048576.0, global_tub[5] ? global_tuu[5] : (unsigned char *)"-", global_tub[5] / 1048576.0
                         , global_tubd[6] ? global_tuud[6] : (unsigned char *)"-", global_tubd[6] / 1048576.0, global_tubw[6] ? global_tuuw[6] : (unsigned char *)"-", global_tubw[6] / 1048576.0, global_tubm[6] ? global_tuum[6] : (unsigned char *)"-", global_tubm[6] / 1048576.0, global_tub[6] ? global_tuu[6] : (unsigned char *)"-", global_tub[6] / 1048576.0
                         , global_tubd[7] ? global_tuud[7] : (unsigned char *)"-", global_tubd[7] / 1048576.0, global_tubw[7] ? global_tuuw[7] : (unsigned char *)"-", global_tubw[7] / 1048576.0, global_tubm[7] ? global_tuum[7] : (unsigned char *)"-", global_tubm[7] / 1048576.0, global_tub[7] ? global_tuu[7] : (unsigned char *)"-", global_tub[7] / 1048576.0
                         , global_tubd[8] ? global_tuud[8] : (unsigned char *)"-", global_tubd[8] / 1048576.0, global_tubw[8] ? global_tuuw[8] : (unsigned char *)"-", global_tubw[8] / 1048576.0, global_tubm[8] ? global_tuum[8] : (unsigned char *)"-", global_tubm[8] / 1048576.0, global_tub[8] ? global_tuu[8] : (unsigned char *)"-", global_tub[8] / 1048576.0
                         , global_tubd[9] ? global_tuud[9] : (unsigned char *)"-", global_tubd[9] / 1048576.0, global_tubw[9] ? global_tuuw[9] : (unsigned char *)"-", global_tubw[9] / 1048576.0, global_tubm[9] ? global_tuum[9] : (unsigned char *)"-", global_tubm[9] / 1048576.0, global_tub[9] ? global_tuu[9] : (unsigned char *)"-", global_tub[9] / 1048576.0
                         );
#endif
}


void irc_topdn(void)
{
  if (!ircbot_enabled) return;
#if 0
  ircbot_store_documentf("topdn",
                         "\x16     DAY        MB     WEEK        MB     MONTH       MB     ALLTIME      MB  \x16\r\n"
                         "\x16 01 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 02 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 03 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 04 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 05 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 06 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 07 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 08 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 09 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         "\x16 10 \x16 %-8s %6.1f   %-8s %7.1f   %-8s %7.1f   %-8s %8.1f\r\n"
                         , global_tdbd[0] ? global_tdud[0] : (unsigned char *)"-", global_tdbd[0] / 1048576.0, global_tdbw[0] ? global_tduw[0] : (unsigned char *)"-", global_tdbw[0] / 1048576.0, global_tdbm[0] ? global_tdum[0] : (unsigned char *)"-", global_tdbm[0] / 1048576.0, global_tdb[0] ? global_tdu[0] : (unsigned char *)"-", global_tdb[0] / 1048576.0
                         , global_tdbd[1] ? global_tdud[1] : (unsigned char *)"-", global_tdbd[1] / 1048576.0, global_tdbw[1] ? global_tduw[1] : (unsigned char *)"-", global_tdbw[1] / 1048576.0, global_tdbm[1] ? global_tdum[1] : (unsigned char *)"-", global_tdbm[1] / 1048576.0, global_tdb[1] ? global_tdu[1] : (unsigned char *)"-", global_tdb[1] / 1048576.0
                         , global_tdbd[2] ? global_tdud[2] : (unsigned char *)"-", global_tdbd[2] / 1048576.0, global_tdbw[2] ? global_tduw[2] : (unsigned char *)"-", global_tdbw[2] / 1048576.0, global_tdbm[2] ? global_tdum[2] : (unsigned char *)"-", global_tdbm[2] / 1048576.0, global_tdb[2] ? global_tdu[2] : (unsigned char *)"-", global_tdb[2] / 1048576.0
                         , global_tdbd[3] ? global_tdud[3] : (unsigned char *)"-", global_tdbd[3] / 1048576.0, global_tdbw[3] ? global_tduw[3] : (unsigned char *)"-", global_tdbw[3] / 1048576.0, global_tdbm[3] ? global_tdum[3] : (unsigned char *)"-", global_tdbm[3] / 1048576.0, global_tdb[3] ? global_tdu[3] : (unsigned char *)"-", global_tdb[3] / 1048576.0
                         , global_tdbd[4] ? global_tdud[4] : (unsigned char *)"-", global_tdbd[4] / 1048576.0, global_tdbw[4] ? global_tduw[4] : (unsigned char *)"-", global_tdbw[4] / 1048576.0, global_tdbm[4] ? global_tdum[4] : (unsigned char *)"-", global_tdbm[4] / 1048576.0, global_tdb[4] ? global_tdu[4] : (unsigned char *)"-", global_tdb[4] / 1048576.0
                         , global_tdbd[5] ? global_tdud[5] : (unsigned char *)"-", global_tdbd[5] / 1048576.0, global_tdbw[5] ? global_tduw[5] : (unsigned char *)"-", global_tdbw[5] / 1048576.0, global_tdbm[5] ? global_tdum[5] : (unsigned char *)"-", global_tdbm[5] / 1048576.0, global_tdb[5] ? global_tdu[5] : (unsigned char *)"-", global_tdb[5] / 1048576.0
                         , global_tdbd[6] ? global_tdud[6] : (unsigned char *)"-", global_tdbd[6] / 1048576.0, global_tdbw[6] ? global_tduw[6] : (unsigned char *)"-", global_tdbw[6] / 1048576.0, global_tdbm[6] ? global_tdum[6] : (unsigned char *)"-", global_tdbm[6] / 1048576.0, global_tdb[6] ? global_tdu[6] : (unsigned char *)"-", global_tdb[6] / 1048576.0
                         , global_tdbd[7] ? global_tdud[7] : (unsigned char *)"-", global_tdbd[7] / 1048576.0, global_tdbw[7] ? global_tduw[7] : (unsigned char *)"-", global_tdbw[7] / 1048576.0, global_tdbm[7] ? global_tdum[7] : (unsigned char *)"-", global_tdbm[7] / 1048576.0, global_tdb[7] ? global_tdu[7] : (unsigned char *)"-", global_tdb[7] / 1048576.0
                         , global_tdbd[8] ? global_tdud[8] : (unsigned char *)"-", global_tdbd[8] / 1048576.0, global_tdbw[8] ? global_tduw[8] : (unsigned char *)"-", global_tdbw[8] / 1048576.0, global_tdbm[8] ? global_tdum[8] : (unsigned char *)"-", global_tdbm[8] / 1048576.0, global_tdb[8] ? global_tdu[8] : (unsigned char *)"-", global_tdb[8] / 1048576.0
                         , global_tdbd[9] ? global_tdud[9] : (unsigned char *)"-", global_tdbd[9] / 1048576.0, global_tdbw[9] ? global_tduw[9] : (unsigned char *)"-", global_tdbw[9] / 1048576.0, global_tdbm[9] ? global_tdum[9] : (unsigned char *)"-", global_tdbm[9] / 1048576.0, global_tdb[9] ? global_tdu[9] : (unsigned char *)"-", global_tdb[9] / 1048576.0
                         );
#endif
}


void irc_wall(void)
{
  ircbot_s *runner;
  int i;

  if (!ircbot_enabled) return;


  sprintf(genbuf, "\x16 ---===]   W A L L   [===---\x16\r\n");
#if 1
  for (i = 0; i < NUM_TOP; i++) {

    if (global_mwu[i] && global_mwm[i]) {

      /* Could this BE any more in efficient? */
      strcat(genbuf, "<");
      strcat(genbuf, (char *)global_mwu[i]);
      strcat(genbuf, "> ");
      strcat(genbuf, (char *)global_mwm[i]);
      strcat(genbuf, "\r\n");

    }

  }
  for( runner = ircbot_head; runner; runner = runner->next )
	  if (runner->active)
		  //		  ircbot_store_document(&runner->bot, "wall", genbuf);
		  ;
#endif
}


void irc_server(void)
{
  if (!ircbot_enabled) return;

#if 0
  ircbot_store_documentf( "server",
						  "\x16 ---===]   S E R V E R   S T A T i S T i C S   [===---\x16\r\nUptime: %s  Users: %d  Xfers: %d\r\n",
						  idletime(global_time_now - server_uptime),
						  server_num_logins,
						  server_num_data);
#endif
}




struct cps_struct {
	float up;
	float down;
	char *user;

    ircbot_s *ircbot;
    char *serv;
    char *chan;
};


//
// we need a lion get_total_cps call
//
int irc_cps_sub(lion_t *handle, void *arg1, void *arg2)
{
	float lup, ldown;
	struct login_node *t;
	struct data_node *d;
	struct cps_struct *data;

	data = (struct cps_struct *) arg1;

	if (!data) return 0;

	if (lion_get_handler(handle) != login_handler) return 1;

	t = (struct login_node *) lion_get_userdata(handle);

	if (!t) return 1;

	// If optional user, ignore nodes that aren't user.
	if (data->user && mystrccmp(data->user, t->user)) return 1;

	d = (struct data_node *)t->data;

	if (d && d->handle) {

		lup = 0;
		ldown = 0;

		lion_get_cps(d->handle, &lup, &ldown);

		data->up += lup;
		data->down += ldown;

	}

	return 1;

}



//
// we need a lion get_total_cps call
//
int irc_uploaders_sub(lion_t *handle, void *arg1, void *arg2)
{
	float lup, ldown;
	struct login_node *t;
	struct data_node *d;
	struct cps_struct *data;

	data = (struct cps_struct *) arg1;

	if (!data) return 0;

	if (lion_get_handler(handle) != login_handler) return 1;

	t = (struct login_node *) lion_get_userdata(handle);

	if (!t) return 1;

	// If optional user, ignore nodes that aren't user.
	if (data->user && mystrccmp(data->user, t->user)) return 1;

	d = (struct data_node *)t->data;

    // Uploaders only.
	if (d && d->handle && (d->type & DATA_STOR)) {
        bytes_t bup, bdown;
		lup = 0;
		ldown = 0;

		lion_get_cps(d->handle, &lup, &ldown);
		lion_get_bytes(d->handle, &bup, &bdown);

		data->up += (float)lup;
        // number of uploaders.
		data->down++;

		irc_announce_one(data->ircbot,
						 "UPLUSER|USER=%-*.*s|FILE=%-*.*s|UP=%7.2f|CURRENT=%7.2f|IRCSERV=%s|CHAN=%s%s%s\n",
                         12,12,
                         t->user,
                         20,20,
                         d->name ? path2file(d->name) : "",
                         lup,
                         (float)bup / 1048576.0,
						 data->serv,
						 data->chan,
						 data->user ? "|USER=" : "",
						 data->user ? data->user : "");


	}

	return 1;

}


//
// we need a lion get_total_cps call
//
int irc_downloaders_sub(lion_t *handle, void *arg1, void *arg2)
{
	float lup, ldown;
	struct login_node *t;
	struct data_node *d;
	struct cps_struct *data;

	data = (struct cps_struct *) arg1;

	if (!data) return 0;

	if (lion_get_handler(handle) != login_handler) return 1;

	t = (struct login_node *) lion_get_userdata(handle);

	if (!t) return 1;

	// If optional user, ignore nodes that aren't user.
	if (data->user && mystrccmp(data->user, t->user)) return 1;

	d = (struct data_node *)t->data;

    // Uploaders only.
	if (d && d->handle && (d->type & DATA_RETR)) {
        bytes_t bup, bdown;
		lup = 0;
		ldown = 0;

		lion_get_cps(d->handle, &lup, &ldown);
		lion_get_bytes(d->handle, &bup, &bdown);

		data->down += (float)ldown;
        // number of uploaders.
		data->up++;

		irc_announce_one(data->ircbot,
						 "DWNUSER|USER=%-*.*s|FILE=%-*.*s|DOWN=%7.2f|CURRENT=%7.2f|IRCSERV=%s|CHAN=%s%s%s\n",
                         12,12,
                         t->user,
                         20,20,
                         d->name ? path2file(d->name) : "",
                         ldown,
                         (float)bdown / 1048576.0,
						 data->serv,
						 data->chan,
						 data->user ? "|USER=" : "",
						 data->user ? data->user : "");


	}

	return 1;

}




void irc_logo(void)
{
  if (!ircbot_enabled) return;


}








//
// We have received a CMD from the IRC BOT. Process it, and reply.
//
void irc_input_cmd(ircbot_s *node, char *line)
{
	char *cmd, *ar, *user, *ircserv, *channel;
	struct cps_struct data;

	ar = line;

	// The command
	cmd = digtoken(&ar, " \r\n");
	// Irc server name
	ircserv = digtoken(&ar, " \r\n");
	// Channel name, or nick of user
	channel = digtoken(&ar, " \r\n");

	if (!cmd || !*cmd) return;
	if (!channel || !*channel) return;



	// Optional arguments
	user = digtoken(&ar, " \r\n");



	data.up   = 0.0;
	data.down = 0.0;
	data.user = NULL;
    data.ircbot = node;
    data.user = user;
    data.serv = ircserv;
    data.chan = channel;


	if (!mystrccmp("IRC_BW", cmd)) {

		user = NULL;
		// We received the BW command.
		lion_find(irc_cps_sub, (void *)&data, NULL);
		irc_announce_one(node,
						 "CPS|UP=%7.2f|DOWN=%7.2f|TOTAL=%7.2f|IRCSERV=%s|CHAN=%s%s%s\n",
						 data.up, data.down,
						 data.up + data.down,
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");

	} else if (!mystrccmp("IRC_SPEED", cmd)) {

		// Allow lookup of a user
		data.user = user;

		// We received the SPEED command.
		lion_find(irc_cps_sub, (void *)&data, NULL);
		irc_announce_one(node,
						 "CPS|UP=%7.2f|DOWN=%7.2f|TOTAL=%7.2f|IRCSERV=%s|CHAN=%s%s%s\n",
						 data.up, data.down,
						 data.up + data.down,
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");

	} else if (!mystrccmp("IRC_UPLOADERS", cmd)) {

		// Allow lookup of a user
		data.user = user;

		// We received the UPLOADERS command.
		irc_announce_one(node, "UPLTOP|IRCSERV=%s|CHAN=%s%s%s\n",
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");
#if 0
		irc_announce_one(node,
						 "UPLUSER|USER=%-*.*s|FILE=%-*.*s|UP=%7.2f|CURRENT=%7.2f|IRCSERV=%s|CHAN=%s%s%s\n",
                         12,12,
                         "fake",
                         20,20,
                         "xxx-porno-part05.rar",
                         1234.55,
                         (float)1500000,
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");
#endif
		lion_find(irc_uploaders_sub, (void *)&data, NULL);
		irc_announce_one(node,
						 "UPLBOT|UP=%7.2f|TOTAL=%7d|IRCSERV=%s|CHAN=%s%s%s\n",
                         data.up / 1048576.0,
                         (int)data.down,
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");


	} else if (!mystrccmp("IRC_DOWNLOADERS", cmd)) {

		// Allow lookup of a user
		data.user = user;

		// We received the UPLOADERS command.
		irc_announce_one(node, "DWNTOP|IRCSERV=%s|CHAN=%s%s%s\n",
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");

		lion_find(irc_downloaders_sub, (void *)&data, NULL);
		irc_announce_one(node,
						 "DWNBOT|DOWN=%7.2f|TOTAL=%7d|IRCSERV=%s|CHAN=%s%s%s\n",
                         data.down / 1048576.0,
                         (int)data.up,
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");



	} else if (!mystrccmp("IRC_REQUEST", cmd)) {
#if 0
		struct login_node t;

		// This stops socket_print / socket_number
		memset(&t, sizeof(t), 0);
		t.handle = NULL;

		// Call site command
		site_request(&t, user);

		// We received the SPEED command.
		irc_announce_one(node,
						 "REQUEST|DIR=%s|IRCSERV=%s|CHAN=%s%s%s",
						 user ? user : "",
						 ircserv,
						 channel,
						 user ? "|USER=" : "",
						 user ? user : "");

#endif
	}



}








#endif
