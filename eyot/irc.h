#ifndef IRC_H_INCLUDED
#define IRC_H_INCLUDED


#include "udp.h"


// Defines

#define IRC_RECONNECT_DELAY    30
#define IRC_RECONNECT_STALLED 1200

enum irc_state {
	IRC_NONE = 0,
	IRC_INITIALISE,
	IRC_GREETING,
	IRC_JOIN,
	IRC_ACTIVE
};

#define IRC_FLAG_TLS         1   // attempt TLS
#define IRC_FLAG_FORCE_TLS   2   // stop if TLS fails




struct irc_channel {

	char *ftpname;
	char *announce;
	char *name;
	char *key;
	char *invite;
	char *fish;

	udp_t *ftpserver;

	void *ircserver;  // So we can reach our server too.

	void **announces;
	int num_announce;


	struct irc_channel *next;

};

typedef struct irc_channel irc_channel_t;


struct irc_server {

	// Conf variables
	char *host;
	int port;
	char *passwd;
	char *mode;

	char *name;
	char *nick;
	char *user;
	char *owner;

	int flags;

	irc_channel_t *channels;


	// run-time variables
	irc_channel_t *active_channel; // set to channel currently active.
	char *reply;   // nick, or channel to reply to.

	int state;

	lion_t *handle;

	time_t idle;   // idle timestamp


	struct irc_server *next;

};

typedef struct irc_server irc_server_t;


// Variables







// Functions

int     irc_init               ( irc_server_t * );
void    irc_free               ( void );
void    irc_poll               ( void );
int     irc_say_all            ( char const *, ...);
int     irc_say_channel        ( irc_channel_t *, char const *, ...);
void    irc_setactive          ( irc_server_t *, char * );
irc_server_t *irc_find_by_name ( char * );
irc_channel_t *ircchan_find_by_name ( irc_server_t *, char * );


void    irc_bw                ( irc_server_t *, char * );
void    irc_speed             ( irc_server_t *, char * );
void    irc_uploaders         ( irc_server_t *, char * );
void    irc_downloaders       ( irc_server_t *, char * );


#endif
