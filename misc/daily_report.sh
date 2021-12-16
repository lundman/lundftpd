#!/bin/sh
#

HOST=localhost
PORT=21
USER=anonymous
PASS=lundftpd@

#
# Specify paths to check as SHORTNAME:/real/full/path/
#
PATHS="TV:/tv/ VCD:/iso/vcd/"

#
# Example Daily report
#
# This will go through a set of directories and work out whats new since
# this script ran previously.
#
# 




echo "Building script"

rm -f .script

touch .script
chmod 600 .script

if [ $? != 0 ]; then
  echo "Couldn't create/chmod .script"
  exit 0
fi

echo "open ${HOST} ${PORT}"      >> .script
echo "user ${USER} ${PASS}"      >> .script
#echo "pass ${PASS}"              >> .script

for item in ${PATHS}
do
  ZNAME=`echo ${item} | cut -f 1 -d ':'`
  ZPATH=`echo ${item} | cut -f 2 -d ':'`

  echo "cd ${ZPATH}"              >> .script
  echo "ls -lt  .list.${ZNAME}"   >> .script

done


echo "quit"                      >> .script


# 
# Call FTP to retrieve directories
#

echo "Connecting to FTP..."

ftp -vin < .script > .script.log 2>&1


# Check return code.

if [ $? != 0 ]; then

  echo "FTP failed -- exit"
  exit 0

fi

echo "Processing..."

#
# Compare today's contents with yesterday
#

touch files.today
mv -f files.today files.yesterday
touch files.today
chmod 600 files.today

if [ $? != 0 ]; then
  echo "Couldn't create/chmod files.today"
  exit 0
fi




date                                       >> files.today
echo ""                                    >> files.today
echo ""                                    >> files.today

for item in ${PATHS}
do
  ZNAME=`echo ${item} | cut -f 1 -d ':'`
  ZPATH=`echo ${item} | cut -f 2 -d ':'`

  touch .list.${ZNAME}.yesterday
  echo "${ZNAME}:"                            >> files.today
  echo ""                                    >> files.today
  diff .list.${ZNAME}.yesterday .list.${ZNAME}|grep '^>' >> files.today 
  echo ""                                    >> files.today
  echo ""                                    >> files.today

  mv -f .list.${ZNAME} .list.${ZNAME}.yesterday

done


#
# Done - move all today's files, to tomorrow.
#

rm -f .script

#
# Process result, mail it etc.
#

echo "Done."

# DOSify it too

sed 's/$/\/g' < files.today > files.today.txt

# Mail it
#mail -s 'New Files Report' recipient@host.com

# Or PGP and then mail it
#gpg --batch --always-trust --armor --output - --recipient user_key_here --encrypt files.today.txt | mail -s 'subject' recipient@host.com

