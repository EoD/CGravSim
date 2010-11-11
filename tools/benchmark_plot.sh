#!/bin/bash

if [ -z "$BASE" ]; then
	BASE="./tools/"
fi

. ${BASE}/benchmark.conf

echo -e "\nWelcome to the CGravSim benachmark plot tool"

if [ -z "$1" ]; then
	echo "Please specify at least one the wpt stat file!"
	echo "Multiple files should be seperated by space."
	exit 1
else
	echo "Plotting data for '$@'"
	echo
fi

command="set ylabel \"user [s]\"; set ytics; "
command+="set xlabel \"revision\"; "
command+="set title \"CGravSim benchmark\"; "

i=0
for inputfile in "$@"
do
	if [ -e "$inputfile" ]; then
		if [ "$i" != "0" ]; then
			command+=", "
		else
			command+="plot "
		fi
		sort "$inputfile" > "$inputfile$TMP" || { echo sort \"$inputfile\" failed; exit 1; }

		#remove everything behind ".wpt" and replace "_" by " "
		title=`echo "${inputfile}" | sed -e "s/\(.*\)\.wpt.*/\1\.wpt/" -e "s,_, ,g"`

		#add commands for both dots and lines for each file
		command+="\"$inputfile$TMP\" using $datarow notitle lt $linetype with $type_black smooth $smooth axes x1y1"
		command+=", \"$inputfile$TMP\" using $datarow title \"$title\" with $type_default axes x1y1"
		i=$(( i+1 ))
	else
		echo "ERROR \"$inputfile\" does not exist"
	fi
done

echo
echo "$command" | gnuplot -persist -

echo Waiting 10s before I delete the temporary plot files && sleep 10
for inputfile in "$@"; do
	rm "$inputfile$TMP"
done
