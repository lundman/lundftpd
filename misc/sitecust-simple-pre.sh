#!/bin/sh
#
# Example script with lundftpd.
#
# Arguments passed to us are those given with the site command. ($1, $2, ...)
#
# You can use stdout/echo to talk back to the client. "echo Searching..." but
# do avoid doing things like "echo 226 Goodbye." (Acutally, shouldn't be 
# possible anyway)
#
# You can use stderr to log things in lundftpd's log file (if used). Like
# "echo $0: 'User $USER not allowed to run.' 1>&2 "  (The '1>&2' sends it to
# stderr). The '$0' helps you know which script ran so you can find/debug it.
# This would make an entry in lundftpd's logs like:
# [sitecust] file opened
# ftp-data/pre.sh: User test not allowed to run.
# [sitecust] file closed
#
# Environment variables set are:
# L_USER  : (string)  The username of the user.
# L_UID   : (integer) The user id of the user. (So you can chown).
# L_GROUP : (string)  The current active group of user.
# L_GID   : (integer) The group id of the user. (chgrp)
# L_CWD   : (string)  The current directory of the user.
# L_ROOT  : (string)  The "root" directory of the lundftpd process.
# L_CMD   : (string)  The name of this script from lundftpd.conf.
# L_EXE   : (string)  The path to lundftpd executable.
#
#
# Safety aware things. Use quotes around the passed arguments, like "$1".
# and since paths in arguments could be "../../" or "/etc/passwd" the best
# way to avoid that is to assume that have "cwd" in into the right place, and
# use "basename $1" to only deal with the filename part.
#
echo "This is the pre.sh script running!"

# Called with the right number of arguments?
if [ $# != 2 ]; then
	echo "${L_CMD} wrong number of arguments. I want src /destination"
	exit 0
fi


echo "$0: ${L_USER} on ${1} to ${2}" 1>&2

# Make sure $1 (the release is a directory)
if [ ! -d "${L_ROOT}/${L_CWD}/${1}" ]; then
   echo "${L_CMD}: ${1} is not a directory."
   exit 0
fi

# User could be assholes and put ".." in the destination path, so lets 
# get rid of that shit right now.
DST=`echo ${2}|sed 's/\.\.//g'`

# Make sure $2 (the destination is a directory)
if [ ! -d "${L_ROOT}/${DST}" ]; then
   echo "${L_CMD}: ${DST} is not a directory."
   exit 0
fi


echo "${L_CMD}: ${1} is a directory: OK (please wait)"

# move release (any failure here, automatically goes to stderr)
mv -f "${L_ROOT}/${L_CWD}/${1}" "${L_ROOT}/${DST}/"

RETURNCODE=$?

# Check if that worked out successfully.
if [ ${RETURNCODE} != 0 ]; then
	echo "$0 Failed to run mv to ${DST} : ${RETURNCODE}" 1>&2
    echo "${L_CMD} failed to execute, move failed. (${RETURNCODE})"
	exit 0
fi

echo "${L_CMD}: ${1} moved successfully"

# Announcing release (dump any output to /dev/null)
${L_EXE} -b "PRE|USER=${L_USER}|DIR=${DST}/${1}"    > /dev/null 2>&1

echo "${L_CMD}: ${DST}/${1} announced."

echo "${L_CMD}: completed."

