

POWER=$1
DIR_LOG=log
DIR_COL=col
INC=10
PERCENT=$INC
BIDC_COL_LOG="m0_p${POWER}.csv"
BI_COL_LOG="m1_p${POWER}.csv"
PC_COL_LOG="m2_p${POWER}.csv"
GBI_COL_LOG="m3_p${POWER}.csv"
FCMP_COL_LOG="mf_p${POWER}.csv"

for COUNTER in $(seq ${INC} 100)
do
	BIDC_LOG="${DIR_LOG}/m0_p${POWER}_${PERCENT}.log"
	if [ -f "${BIDC_LOG}" ]
	then
		grep Toffli $BIDC_LOG | \
		awk -v PERC=${PERCENT} '{ gate += $2; time += $3} \
		END {print PERC " , "gate/NR \
		" , " time/NR}' >> ${DIR_COL}/${BIDC_COL_LOG}
	fi

	BI_LOG="${DIR_LOG}/m1_p${POWER}_${PERCENT}.log"
	if [ -f "${BI_LOG}" ]
	then
		grep Toffli $BI_LOG | \
		awk -v PERC=${PERCENT} '{ gate += $2; time += $3} \
		END {print PERC " , "gate/NR \
		" , " time/NR}' >> ${DIR_COL}/${BI_COL_LOG}
	fi

	PC_LOG="${DIR_LOG}/m2_p${POWER}_${PERCENT}.log"
	if [ -f "${PC_LOG}" ]
	then
		grep Toffli $PC_LOG | \
		awk -v PERC=${PERCENT} '{ gate += $2; time += $3} \
		END {print PERC " , "gate/NR \
		" , " time/NR}' >> ${DIR_COL}/${PC_COL_LOG}
	fi

	GBI_LOG="${DIR_LOG}/m3_p${POWER}_${PERCENT}.log"
	if [ -f "${GBI_LOG}" ]
	then
		grep Toffli $GBI_LOG | \
		awk -v PERC=${PERCENT} '{ gate += $2; time += $3} \
		END {print PERC " , "gate/NR \
		" , " time/NR}' >> ${DIR_COL}/${GBI_COL_LOG}
	fi

	FCMP_LOG="${DIR_LOG}/mf_p${POWER}_${PERCENT}.log"
	if [ -f "${FCMP_LOG}" ]
	then
		grep Toffli $FCMP_LOG | \
		awk -v PERC=${PERCENT} '{ gate += $2; time += $3} \
		END {print PERC " , "gate/NR \
		" , " time/NR}' >> ${DIR_COL}/${FCMP_COL_LOG}
	fi
	#echo $PERCENT
	PERCENT=$(( $PERCENT + $INC ))
	if [ "$PERCENT" -gt 100 ];
		then break
	fi
done
