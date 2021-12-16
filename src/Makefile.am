
bin_PROGRAMS=lundftpd

lundftpd_SOURCES= main.c user.c socket.c io.c login.c misc.c data.c		\
         file.c quota.c fnmatch.c check.c help.c log.c groups.c			\
         ident.c site.c local.c global.c global_var.c dbutil.c tops.c	\
         dupe.c export.c checkchild.c sfv.c conf.c section.c			\
         sitecust.c strmode.c irc.c remote.c race.c race.h check.h		\
         dupe.h groups.h login.h remote.h user.h checkchild.h exec.h	\
         help.h ls.h section.h userflag.h colour.h export.h ident.h		\
         misc.h sfv.h version.h conf.h file.h io.h mp3genre.h site.h	\
         win32.h data.h fnmatch.h irc.h oldlogin.h sitecust.h getput.h	\
         local.h quota.h socket.h win32_dirent.h global.h log.h race.h	\
         tops.h dbutil.h

EXTRA_DIST=lundftpd.conf LICENSE

lundftpd_CPPFLAGS = -I$(srcdir)/../lion/src -I$(srcdir)/../lion/contrib/libdirlist
lundftpd_LDADD=$(srcdir)/../lion/src/liblion.a $(srcdir)/../lion/contrib/libdirlist/libdirlist.a

# Win32
EXTRA_DIST+=win32.h win32.c win32_dirent.h w32service.c mydb.c mydb.h
EXTRA_DIST+=getopt.c 


# Automatically increase build version if found.
all-local:
	@if test -n "$(BUILD)"; then "$(BUILD)" ; fi

