#!/bin/bash
DIR=./
#ID_BEGIN=0
#FILE_NUM=10
POWER=4
PROGRAM=a.out
if [ "$1" != "" ] ; then
	DIR=$1
	echo "set DIR to ${DIR}."
fi
#if [ "$2" != "" ] ; then
#	ID_BEGIN=$2
#	echo "set ID_BEGIN to ${ID_BEGIN}."
#fi
#if [ "$3" != "" ] ; then
#	FILE_NUM=$3
#	echo "set FILE_NUM to ${FILE_NUM}."
#fi
if [ "$2" != "" ] ; then
	POWER=$2
	echo "set POWER to ${POWER}."
fi
#if [ "$5" != "" ] ; then
#	PERCENT=$5
#	echo "set PERCENT to ${PERCENT}."
#fi

END=$(( $ID_BEGIN + $FILE_NUM - 1))

INC=10
PERCENT=$INC
IS_ALLRAND=_allrand

for COUNTER in $(seq ${INC} 100)
do
	BIDC_LOG="m0_p${POWER}_${PERCENT}.log"
	touch ./log/${BIDC_LOG}
	for FILE in $( ls ${DIR}/*_p${POWER}_${PERCENT}${IS_ALLRAND}_incmp.spec );
	do
		#echo "$ID"
		./$PROGRAM $FILE >> ./log/${BIDC_LOG}
	done
	
	BI_LOG="m1_p${POWER}_${PERCENT}.log"
	touch ./log/${BI_LOG}
	for FILE in $( ls ${DIR}/*_p${POWER}_${PERCENT}${IS_ALLRAND}_cmp.spec );
	do
		#echo "$ID"
		./$PROGRAM ${FILE} 1 >> ./log/${BI_LOG}
	done


	PC_LOG="m2_p${POWER}_${PERCENT}.log"
	touch ./log/${PC_LOG}
	for FILE in $( ls ${DIR}/*_p${POWER}_${PERCENT}${IS_ALLRAND}_incmp.spec );
	do
		#echo "$ID"
		./$PROGRAM ${FILE} 2 >> ./log/${PC_LOG}
	done

	GBI_LOG="m3_p${POWER}_${PERCENT}.log"
	touch ./log/${GBI_LOG}
	for FILE in $( ls ${DIR}/*_p${POWER}_${PERCENT}${IS_ALLRAND}_incmp.spec );
	do
		#echo "$ID"
		./$PROGRAM ${FILE} 3 >> ./log/${GBI_LOG}
	done

	FCMP_LOG="mf_p${POWER}_${PERCENT}.log"
	touch ./log/${FCMP_LOG}
	for FILE in $( ls ${DIR}/*_p${POWER}_${PERCENT}${IS_ALLRAND}_fcmp.spec );
	do
		#echo "$ID"
		./$PROGRAM ${FILE} 1 >> ./log/${FCMP_LOG}
	done
	#echo $PERCENT
	PERCENT=$(( $PERCENT + $INC ))
	if [ "$PERCENT" -gt 100 ];
		then break
	fi
done

