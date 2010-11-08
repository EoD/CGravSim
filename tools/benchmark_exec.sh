#!/bin/bash

if [ -z "$BASE" ]; then
	BASE="./tools/"
fi

#load variables
. ${BASE}/benchmark.conf

tversion=`${TIME} -V 2> /dev/stdout`
if [ "${tversion}" != "${TIME_VERSION}" ]; then
	echo "You're using '${tversion}' instead of '${TIME_VERSION}'. Are you sure what you are doing?"
	echo -n "I give you 10 seconds to think about it"
	for i in `seq -s " " 10`
	do
		sleep 1
		echo -n "."
	done	
	echo
fi

exename="${1}"
if [ "${exename}" == "" ]; then
	echo "Please specify the executable as first argument"
	exit 1
fi

inputfile="${2}"
if [ "${inputfile}" == "" ]; then
	echo "Please specify the wpt files as second argument"
	exit 1
fi


# 1. Get revision via ls-remote of the project directory
# 2. Take the value of the current branch and check for heads/CURRENTbranch 
# 3. Take the revision value of the above branch
# 4. take this revision to get the date of the revision
# Quite complicated, but it works ;)

revision=`git ls-remote . | grep -i "heads/\`git branch | grep \"*\" | sed -e 's,\*.,,'\`" | sed 's/[ \t\r\n\v\f].*//'`
revdate=`git log --date=${REVTIME_FORMAT} -1 ${revision} | grep Date | sed -e 's/Date:   //'`
filename="${inputfile}$STAT"
tempfile="${inputfile}$TMP"

execution() {
	if [ $output ]; then
		cp "${inputfile}" "${tempfile}"
		timex="`${TIME} -f "${TIME_FORMAT}" "${exename}" "${tempfile}" 2> /dev/stdout 1> /dev/null`"
	else
		#extract number of steps, replace the timestep with ONE huge one
		#and calculate with the old timestep at the end
		steps=`cat "${inputfile}" | head -n 1 | sed -e 's/.*;\(.*\);.*/\1/'`
		timestep=`cat "${inputfile}" | head -n 1 | sed -e 's/.*;.*;\(.*\)[ \t\r\n\v\f]/\1/'`
		timestep_new=`echo "$steps * $timestep" | bc`
		
		cat "${inputfile}" | sed -e "1 s/\(.*\);.*;.*/\1;1;$timestep_new/" > "${tempfile}"

		timex="`${TIME} -f "${TIME_FORMAT}" "${exename}" -t $timestep "${tempfile}" 2> /dev/stdout 1> /dev/null`"
	fi
	user_tmp=`echo "$timex" | grep ${USER} | sed -e "s/${USER}//"`
	sys_tmp=`echo "$timex" | grep ${SYS} | sed -e "s/${SYS}//"`
	user=`echo $user + $user_tmp | bc`
	sys=`echo $sys + $sys_tmp | bc`
	echo -n "."
}

echo "Executing '${exename}' with '${inputfile}' $LOOPS times"
for i in `seq -s " " $LOOPS` 
do
	execution
done
rm "${tempfile}"

user_cut=`echo "scale=${accuracy};$user / $LOOPS" | bc | sed -e 's,^\.,0\.,'`
sys_cut=`echo "scale=${accuracy};$sys / $LOOPS" | bc | sed -e 's,^\.,0\.,'`

echo -e -n "${exename}\t\"${revdate}\"" >> "${filename}"
if [ $verbose ]; then
	echo >> "${filename}"
	echo >> "${filename}"
	echo " ${LOOPS} executions" >> "${filename}"
	echo -e " ${user_cut}\t${USER}" >> "${filename}"
	echo -e " ${sys_cut}\t${SYS}" >> "${filename}"
	echo >> "${filename}"
else
	echo -e "\t${user_cut}\t${sys_cut}\t${LOOPS}" >> "${filename}"
fi

echo
tail -n 1 "${filename}"
