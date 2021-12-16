struct an_event {

	char *type;
	char *fmt;

	struct an_event *next;

};

struct an_announce {

	char *section;
	char *channel;
	char *name;

	struct an_event *events;

	struct an_announce *next;

};

struct an_announce *an_add(char *section, char *channel, char *name);
struct an_announce *an_find(char *section);
struct an_event *an_add_event(char *section, char *type, char *format, char *name);
struct an_event *an_find_event(char *section, char *type);
//char *an_format(char **keys, char **values, int items, void *optarg);
char *an_format(char **keys, char **values, int items, irc_channel_t *ic);
void an_assigntochannel(irc_channel_t *ic);
