#!/bin/bash
# Runs test on uniqify

function p {
	local STR="$1"
	STR+=", "
	STR+=$(stat $1 --printf=%s)
	STR+=", "
	STR+=$(/usr/bin/time -f "%S" uniqify 10 < $1 > /dev/null)
	echo $STR
}

echo "Filename, size (bytes), system time"
for f in textfiles/*
	do 
		p $f
done
