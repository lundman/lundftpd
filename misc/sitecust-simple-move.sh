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
# ftp-data/move.sh: User test not allowed to run.
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
# This script should be configured with:
# sitecust {
#	name = "MOVE";
#	type = "EXEC";
#	desc = "move a directory.";
#	exec = "ftp-data/move.sh";
#	flags = "UF_SEEALL";
#	args = "2,dir,section";
#};
#
# Where "args" enforces the two types (you can use "2,dir,dir" or perhaps
# "2,dirorfile,dirorfile" as you see fit. This is important as types "file" and
# "dir" are reduced. (path checked for "../" sections, and permissions verified
# so you don't get nasty things happening.
#
echo "This is the move.sh script running!"

# Called with the right number of arguments?
# Not needed, you specify number of arguments in .conf

# Log this call into lundftpd's log file.
echo "$0: ${L_USER} on ${1} to ${2}" 1>&2

echo "${L_CMD}: Processing ${1} -> ${2} : OK (please wait)"

# move release (any failure here, automatically goes to stderr)
mv -f "${L_ROOT}/${1}" "${L_ROOT}/${2}"

RETURNCODE=$?

# Check if that worked out successfully.
if [ ${RETURNCODE} != 0 ]; then
	echo "$0 Failed to run mv to ${2} : ${RETURNCODE}" 1>&2
    echo "${L_CMD} failed to execute, move failed. (${RETURNCODE})"
	exit 0
fi

echo "${L_CMD}: ${1} moved successfully"

# Announcing release (dump any output to /dev/null)
${L_EXE} -b "MOVE|USER=${L_USER}|DIR=${1}|SECTION=${2}"    > /dev/null 2>&1

echo "${L_CMD}: ${2}/${1} announced."

echo "${L_CMD}: completed."

