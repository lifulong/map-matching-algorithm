#!/bin/bash

Usage="Usage: $0 real_pos_file fake_pos_file"

[ $# -ne 2 ] && { echo "$Usage"; exit 1; }

_DEBUG="true"
function DEBUG()
{
	if [ "$_DEBUG" == "true" ];then
		$@
	fi
}

real_pos_file=$1
fake_pos_file=$2
result_file="deviation"

declare -a real_pos_array
declare -a fake_pos_array

function load_pos()
{
	pos_file=$1
	flag=$2

	let i=0
	while read line
	do
		#read lng lat <<-EOF
		#	$line
		#EOF
		case $flag in
			"real")
				real_pos_array[$i]=$line;;
			"fake")
				fake_pos_array[$i]=$line;;
		esac
		let i=$i+1
	done<$pos_file
}

function get_deviation()
{
	if [ ${#real_pos_array[*]} != ${#fake_pos_array[*]} ];then
		echo -e "[Error]:real pos count not equal fake pos count\n"
		return
	fi

	count=${#real_pos_array[*]}
	total_lng=0
	total_lat=0
	average_lng=0
	average_lat=0
	let i=0
	while true
	do
		if [ $i -ge $count ];then
			average_lng=`echo "scale=6;$total_lng/$count" | bc`
			average_lat=`echo "scale=6;$total_lat/$count" | bc`
			pos=`expr index $average_lng .`
			[ $pos -eq 1 ] && { average_lng="0"$average_lng; }
			pos=`expr index $average_lat .`
			[ $pos -eq 1 ] && { average_lat="0"$average_lat; }
			echo -e "$average_lng\t$average_lat" | tee $result_file
			return
		fi

		real_lng_lat=${real_pos_array[$i]}
		fake_lng_lat=${fake_pos_array[$i]}

		read real_lng real_lat <<-EOF
			$real_lng_lat
		EOF
		read fake_lng fake_lat <<-EOF
			$fake_lng_lat
		EOF

		dev_lng=`scale=6;echo $real_lng-$fake_lng | bc`
		dev_lat=`scale=6;echo $real_lat-$fake_lat | bc`
		total_lng=`scale=6;echo $total_lng+$dev_lng | bc`
		total_lat=`scale=6;echo $total_lat+$dev_lat | bc`
		DEBUG echo "total_lng:$total_lng"
		DEBUG echo "total_lat:$total_lat"

		let i=$i+1
	done
}

load_pos $real_pos_file "real"
load_pos $fake_pos_file "fake"

get_deviation


