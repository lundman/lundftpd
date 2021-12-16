#!/bin/sh
#
# Put this in crontab --
#
# 0 0 * * mon /path/to/pcapps_week_rotate.sh

PCDIR="/tmp/pc/apps"

day=`/bin/date +%a`
#Use +U if you want Sunday as first day of week,
#and +V if you wnt monday, and want to adhere to ISO 8601
week=`/bin/date +%V`

if [ "$day" = "Mon" ]; then
	cd $PCDIR
	rm -f current
	rmdir week*
	mkdir -p week$week
	chmod 777 week$week
	ln -fs week$week current
fi
