#!/usr/bin/env bash

start=$(date +%s%N)
start_ms=${start:0:16}

##
thread_num=35

#------------------------------------
tmpfifo=$$.fifo
trap "exec 1000>&-;exec 1000<&-;exit 0" 2
mkfifo $tmpfifo
exec 1000<>$tmpfifo
rm -rf $tmpfifo

for ((i=1;i<=$thread_num;i++))
do
	echo >&1000
done

for i in $(seq 31 200)
do
	read -u1000
	{
		echo begin $i
		./acc.exe 8000000 $i
		echo >&1000
	} &
done

#######

wait

end=$(date +%s%N)
end_ms=${end:0:16}
echo "==============================="
echo " cost time(s):"
echo "scale=3;($end_ms-$start_ms)/1000000" | bc
