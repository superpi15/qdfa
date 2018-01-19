CASEPATH=$1
echo $1
touch data.log

for f in $(ls ${CASEPATH})
do
	./a.out ${CASEPATH}/${f} > log/${f}.log
	grep Summary log/${f}.log >> data.log
done
