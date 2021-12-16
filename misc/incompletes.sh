#!/bin/sh
#
# Example incompletes script for lftpd to be run by cron.
#
# Run this when you run diskcull etc :)
#
#
# First the function
#
# Assumes there is a ".rules" file that keeps the permanent contents of
# .message, which it copies to .message then adds incompletes list.
#
# This function looks for the existance of ".incomplete" files within a tree
# passed to it. It then adds these to the .message message, as well as,
# creates 0 length files in the "root" of the directory tree passed. It
# removes any -INCOMPLETE files older than last time this was run.
#
do_incompletes() {
    cd $DIR
    rm -f /tmp/.timestamp
    touch /tmp/.timestamp
    sleep 1     # Need at least 1 sec between timestamp and first file created
    touch .rules
    cp -f .rules .message
    chmod 644 .message
    echo "MISSING FILES IN " ${AREA} " AREA: (Please rectify promptly)" >> .message
    for ENTRY in `find . -name "*NUKE*" -prune -o -name '*_INCOMPLETE_*' -print`
    do
	  dirname "${ENTRY}" >> .message
	  touch `dirname ${ENTRY} | sed -e 's/\//_/g' -e 's/^\._/.\//g' -e 's/$/-INCOMPLETE/g'`
	done
    find . -name \*-INCOMPLETE -type f -size 0 \! -newer /tmp/.timestamp -exec rm -f {} \;
	rm -f /tmp/.timestamp
}

#
#
# Make sure lftpd is up, if not, start it
#
cd /some/path/to/lftpd/
./lundftpd > /dev/null 2>&1
#
#
# Run diskcull to ensure free space
#
./diskcull -v -f diskcull.conf  >> diskcull.log 2>&1
#
#
#
# For each area, build incomplete
#
#Missing files bit for VCD
DIR=/disk04/vcd/
AREA=VCD
do_incompletes

#Missing files bit for DiVX
DIR=/disk01/files/divx/
AREA=DiVX
do_incompletes

