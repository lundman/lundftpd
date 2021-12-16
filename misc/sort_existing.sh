#!/bin/sh
#
# This is a oneoff script that sorts releases into weekXX/ directories
# and is only really useful for people who have a huge /pc/apps tree
# that they would like to weekify for use with pcapps_week_rotate.sh
#
# - friar 2003

JAN=0
FEB=31
MAR=$(( $FEB + 28 ))
APR=$(( $MAR + 31 ))
MAY=$(( $APR + 30 ))
JUN=$(( $MAY + 31 ))
JUL=$(( $JUN + 30 ))
AUG=$(( $JUL + 31 ))
SEP=$(( $AUG + 31 ))
OCT=$(( $SEP + 30 ))
NOV=$(( $OCT + 31 ))
DEC=$(( $NOV + 30 ))

for i in *; do
	mon=`ls -ld "$i" | awk '{ print $6 }'`
	day=`ls -ld "$i" | awk '{ print $7 }'`

	if [ "$mon" = "Jan" ]; then add=$JAN; fi
	if [ "$mon" = "Feb" ]; then add=$FEB; fi
	if [ "$mon" = "Mar" ]; then add=$MAR; fi
	if [ "$mon" = "Apr" ]; then add=$APR; fi
	if [ "$mon" = "May" ]; then add=$MAY; fi
	if [ "$mon" = "Jun" ]; then add=$JUN; fi
	if [ "$mon" = "Jul" ]; then add=$JUL; fi
	if [ "$mon" = "Aug" ]; then add=$AUG; fi
	if [ "$mon" = "Sep" ]; then add=$SEP; fi
	if [ "$mon" = "Oct" ]; then add=$OCT; fi
	if [ "$mon" = "Nov" ]; then add=$NOV; fi
	if [ "$mon" = "Dec" ]; then add=$DEC; fi

	week=$(( ($add + $day) / 7 ))
	if [ $week -lt 10 ]; then
		prweek=week0$week
	else
		prweek=week$week
	fi

	mkdir -p $prweek
	if [ "$?" -ne 0 ]; then
		echo "Failed to created a directory... bailing out."
		exit 1
	fi
	echo "Moving to $prweek -- $i"
	mv "$i" $prweek

done
