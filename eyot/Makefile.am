
bin_PROGRAMS=eyot

eyot_SOURCES=eyot.c udp.c irc.c irc_state.c parser.c cmd.c conf.c		\
                         announce.c arguments.c blowfish.c announce.h	\
                         arguments.h blowfish.h cmd.h conf.h eyot.h		\
                         irc.h irc_state.h parser.h udp.h

SUBDIRS=conf

EXTRA_DIST=readme.txt

eyot_CPPFLAGS = -I$(srcdir)/../lion/src
eyot_LDADD=$(srcdir)/../lion/src/liblion.a
