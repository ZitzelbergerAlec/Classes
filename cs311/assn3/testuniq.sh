#!/bin/bash
# Runs test on uniqify
# Usage: 
# testuniq.sh subfolder
# if no subfolder, just use "."

function p {
	local STR="$1"
	STR+=", "
	STR+=$(stat $1 --printf=%s)
	STR+=", "
	STR+=$(/usr/bin/time -f "%S" uniqify 10 < $1 > /dev/null)
	echo $STR
}

echo "Filename, size (bytes), system time"
for f in $1/*txt
	do 
		p $f
done
