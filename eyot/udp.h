#ifndef UDP_H_INCLUDED
#define UDP_H_INCLUDED

#ifdef WITH_SSL
#include <openssl/blowfish.h>
#endif


// Defines


// Bah! irc_channel has udp_t *, and
// udp_t has a irc_channel *. So both includes need both
// So lets get around it
#ifndef IRC_H_INCLUDED
struct irc_channel;
#endif


struct udp_struct {

	lion_t *handle;

	char *name;

	unsigned long host;
	int port;
	int remote_port;

	float cps_up;
	float cps_down;

#ifdef WITH_SSL
	BF_KEY key;
#endif

	unsigned int num_channels;
	struct irc_channel **channels;

	struct udp_struct *next;

};

typedef struct udp_struct udp_t;




// Variables







// Functions

udp_t  *udp_init               ( char *, unsigned long, int, char *, int );
void    udp_free               ( void );

udp_t  *udp_assign_channel     ( struct irc_channel * );
void    udp_send_all           ( char * );

udp_t  *udp_find               ( char * );
int     udp_sendf              ( udp_t *, char const *, ...);


#endif
