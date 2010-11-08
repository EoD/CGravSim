#!/bin/bash

if [ -z "$BASE" ]; then
	BASE="./tools/"
fi

#load variables
. ${BASE}/benchmark.conf

LS="`which ls`"

echo
echo "Welcome to the CGravSim benachmark tool"

if [ -z "$1" ]; then
	echo "You have to specify at least one file which I should benchmark!"
	echo "You can also specify multiple files seperated by space." 
	exit 1
fi

echo
echo "WARNING! I will clean AND \"reset --hard\" this directory in $delay seconds (deletes exe/ dep*/ obj*/ folders)"
for i in `seq -s " " $delay`; do
	sleep 1
	echo -n "."
done

echo
echo

for i in `seq -s " " $max_reset`; do
	echo "Step $i out of $max_reset"
	if [ $i != 1 ]; then
		git reset --hard HEAD^
	fi
	make clean 1> /dev/null || { echo "make clean failed"; exit 1; }
	make auto 1> /dev/null || { echo "make auto failed"; exit 1; }
	exename="$EXE/`$LS $ls_args $EXE/ | grep "cgravsim_" | head -n 1`"
	strip "$exename" || exit 1
	for inputfile in "$@"; do
		${execution} "$exename" "$inputfile"
	done
	echo
done
git merge -n origin

newfiles=""
for inputfile in "$@"; do
	newfiles+=' '

	#we don't like spaces in our files. Just replace them by "_"
	newfile="`echo "$inputfile$STAT$TMP" | sed -e 's, ,_,g'`"

	#only use entries which use the same executable (arch) and have the same loops (at the end of line)
	cat "$inputfile$STAT" | grep "$exename" | grep "$LOOPS$" > "$newfile"

	newfiles+="$newfile"
done

"${plot}" ${newfiles} && \
for newfile in $newfiles; do
	rm "$newfile"
done
