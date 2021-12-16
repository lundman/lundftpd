/*
 * Identd Prober (c) Jorgen Lundman
 *
 * Nonblocking identd lookup functions.
 *
 * Adds identd requests to an internal list, fires away requests and waits
 * for response or timeout.
 *
 * Shit, since when was this TCP :)
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
//#include <syslog.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include <time.h>  /* linux */

#include <sys/types.h>


#include "global.h"
#include "log.h"
#include "misc.h"
#include "socket.h"
#include "ident.h"
#include "login.h"

#include "lion.h"




static struct ident_node *ident_node_head = NULL;


/*
 * Ready the identd prober port, structures etc. 
 */

void ident_init(void) 
{

}


//
// LiON input handler for ident requests.
//
int ident_handler( lion_t *handle,
				   void *user_data, int status, int size, char *line)
{
	struct ident_node *ni = (struct ident_node *) user_data;

	//consolef("[ident] %p/%p handler event %d\n", handle, user_data, status);

	
	switch( status ) {

	case LION_CONNECTION_LOST:
		consolef("[ident] %p/%p connection failed %d:%s\n", 
				 handle, user_data, size, line);

		/* Fall-through */

	case LION_CONNECTION_CLOSED:
		//consolef("[ident] %p/%p connection closed\n", handle, user_data);

		if (ni) {

			ni->handle = NULL;
			ident_close( ni );
			
		}
		break;


	case LION_CONNECTION_CONNECTED:
		//consolef("[ident] %p/%p connected\n", handle, user_data);

		/* send request */
		lion_printf(handle, "%u,%u\r\n",
					ni->local_port,
					ni->remote_port);
		break;


	case LION_INPUT:

		if (ni && ni->reply) {
			unsigned int rmt_port, our_port;
			char reply[256];
			
			// Do a nice buffer check here. size is strlen line
			if ((size < 256) &&
				(sscanf(line, "%u , %u : USERID :%*[^:]:%9s",
						&rmt_port, &our_port, reply) == 3)  &&
				(ni->remote_port == our_port) &&
				(ni->local_port == rmt_port)) {
				
				chop(reply);
				
				strncpy(ni->reply, reply, MAXUSERNAME); 
				/* signal we have found it! */
				ni->reply[MAXUSERNAME - 1] = 0;

				consolef("[ident] reply '%s'\n", ni->reply);
				
			}
		}

		lion_close( handle );

		break;
	}

	return 0;
}


void ident_request(struct login_node *t, char *reply)
{
  unsigned long tmp;
  struct ident_node *ni;
  int len;


  if (!server_useident) { /* Are idents turned off ? */
    strcpy(reply, FROM_UNKNOWN);
    return;
  }

  consolef("[ident] %p request started.\n", t);

  ni = (struct ident_node *) malloc(sizeof(struct ident_node));

  if (!ni) {
    perror("ident_request(malloc())");
    strcpy(reply, FROM_UNKNOWN);
#if 0
    strcpy(reply, "NONE1");
#endif
    return;
  }
  
  memset(ni, 0, sizeof(*ni));
  ni->next = NULL;

  ni->reply = reply;

  len = sizeof(struct sockaddr);

#if 1

  lion_getpeername( t->handle, &ni->ident, &ni->local_port);

#endif

  len = sizeof(struct sockaddr_in);

  lion_getsockname( t->handle, &tmp, &ni->remote_port );
  ni->ident_port = RFC931_PORT;
  
  //  consolef("Firing off connection to %s:%d\n",
  //	   lion_ntoa(ni->ident),
  //	   ni->ident_port);


  ni->reply[0] = 0; /* signal its being process */ 

  ni->time = global_time_now;
  /*  time(&ni->time); / * ready timeout */

  /* Ok, we're hoping for a connection */
  //ni->status = IDENT_PENDING;

  /* add to linked list */
  ni->next = ident_node_head;
  ident_node_head = ni;


  ni->handle = lion_connect( lion_ntoa( ni->ident ), 
							 ni->ident_port,
							 0L, // iface for ident?
							 0,
							 LION_FLAG_FULFILL,
							 (void *)ni );

  lion_set_handler( ni->handle, ident_handler );

}



void ident_close(struct ident_node *in)
{
	struct ident_node *curr, *last;
	/* ok, we have a reply from in, bad or good */
	
	//	consolef("Closing identd %p/%p\n", in, in->handle);
	if (in->handle) {
		lion_set_userdata( in->handle, NULL );
		lion_close( in->handle );
		in->handle = NULL;
	}
	
	
	/* we call login.c so it can signal the login to continue, if it was
	 * paused */
	
	if (in->reply) {
		if (!in->reply[0])
			strcpy(in->reply, "NONE");
		login_ident_reply(in->reply);
	}
	
	// We need to unhooked the linked list.

	for (curr = ident_node_head, last = NULL;
		 curr; last = curr, curr=curr->next) {
		
		if (curr == in) {
			if (!last)
				ident_node_head = curr->next;
			else
				last->next = curr->next;
			
			break;
			
		}
	}
	
  free(in);
  //  consolef("[ident] freed. Head is %p\n", ident_node_head);
  
}







void ident_timeout(void)
{
	struct ident_node *in;

	//	consolef("ident_timeout\n");
	
	for (in = ident_node_head; in; in = in->next) {
		
		if ((in->time + IDENT_TIMEOUT) < lion_global_time) {
			
			consolef("Timeout from %s %d\n", 
					 lion_ntoa(in->ident),
					 in->local_port);

			if (in->reply)
				strcpy(in->reply, FROM_UNKNOWN);
#if 0
			strcpy(in->reply, "NONE6");
#endif      
			
			ident_close(in);
			return; // If we unlink this node, we must stop the for.
			
		}
	}
}


void ident_remove(char *reply)
{
	struct ident_node *in;

	//	consolef("ident_remove: %p \n", reply);

	for( in = ident_node_head; in; in = in->next) {
		consolef("   %p == %p ?\n", reply, in->reply);
		if (in->reply == reply) {
			in->reply = NULL;
		}
	}
}

