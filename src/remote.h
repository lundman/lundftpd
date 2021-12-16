#ifndef REMOTE_H_INCLUDED
#define REMOTE_H_INCLUDED

// Defines

enum remote_enum {
	REMOTE_CMD_NONE=0, // Set only to none when we've replied to a command.
	REMOTE_CMD_SIZE,
	REMOTE_CMD_PASV,
	REMOTE_CMD_LIST,
	REMOTE_CMD_CWD,
	REMOTE_CMD_RETR,
	REMOTE_CMD_MDTM,
	REMOTE_CMD_STOR,
	REMOTE_CMD_APPE,
	REMOTE_CMD_REST,
	REMOTE_CMD_TYPE,
	REMOTE_CMD_PROT,
	REMOTE_CMD_NLST,
	REMOTE_CMD_MKD,
	REMOTE_CMD_DELE,
	REMOTE_CMD_RMD,
	REMOTE_CMD_RNFR,
	REMOTE_CMD_RNTO,
	REMOTE_CMD_PORT,
	REMOTE_CMD_CCSN,
	REMOTE_CMD_SSCN,
	REMOTE_CMD_PRET
};

typedef enum remote_enum remote_enum_t;


enum remote_state_enum {
	REMOTE_STATE_NONE = 0,
	REMOTE_STATE_CONNECTED,
	REMOTE_STATE_AWAIT220,
	REMOTE_STATE_SENT_USER,
	REMOTE_STATE_SENT_PASS,
	REMOTE_STATE_SENT_AUTH_TLS,
	REMOTE_STATE_SENT_AUTH_SSL,
	REMOTE_STATE_AWAIT_SSL,
	REMOTE_STATE_CHECK_PROT,
	REMOTE_STATE_SENT_PROT,
	REMOTE_STATE_CHECK_CCSN,
	REMOTE_STATE_SENT_CCSN,
	REMOTE_STATE_CHECK_SSCN,
	REMOTE_STATE_SENT_SSCN,
	REMOTE_STATE_CHECK_TYPE,
	REMOTE_STATE_SENT_TYPE,
	REMOTE_STATE_CHECK_DONE,
	REMOTE_STATE_RELAY
};

typedef enum remote_state_enum remote_state_t;


// Variables

struct remote_struct {
	lion_t *handle;
	remote_enum_t command;  // so we know what to send and how to reply with
                            // failures.
	char *args;             // Any arguments passed along?
	section_t *section;     // which section is this regarding!

	char *pret;             // PRET command path, if given

	remote_state_t state;
};

typedef struct remote_struct remote_t;




// Functions



void        remote_relay         ( struct login_node *, remote_enum_t, char *);
void        remote_release       ( struct login_node * );
int         remote_handler       ( lion_t *, void *, int, int, char * );
int         remote_state         ( struct login_node *, char * );
int         remote_ispret        ( struct login_node * );
void        remote_clearpret     ( struct login_node * );


#endif
