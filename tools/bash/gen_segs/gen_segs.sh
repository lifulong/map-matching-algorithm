#!/bin/bash

# @input: road edge file
#	edge_id	lat1	lng1	lat2	lng2	...
# @output: road network segments file
#	seg_id	lat	lng	edge_id
# @author: lifulong

Usage="Usage: $0 road_file output_segs_file\t"

[ $# -ne 2 ] && { echo -e "$Usage"; exit 1; }
edges_file=$1
segs_file=$2

function gen_segs()
{
	echo -e "[INFO]\tStart gen_segs... ..."

	local edge_file=$1
	local seg_file=$2
	let seg_id=0

	[ -f $seg_file ] && { rm -f $seg_file; }

	exec 3<> $edge_file

	while read line
	do
		let label=$seg_id%4

		case $label in
			0) echo -n -e "\b\b\b---";;
			1) echo -n -e "\b\b\b\\\\\\";;
			2) echo -n -e "\b\b\b|||";;
			3) echo -n -e "\b\b\b///";;
		esac

		read edge_id rest<<-EOF
			$line
		EOF

		#Fix:set lat and lng to "" while read an new line
		lat=""
		lng=""
		last_lat=""
		last_lng=""

		while [ "$rest" != "" ];
		do
			last_lat=$lat
			last_lng=$lng
			read lat lng rest<<-EOF
				$rest
			EOF
			if [ "$last_lat" == "" -o "$last_lng" == "" ]; then
				continue
			fi
			let seg_id=$seg_id+1
#			if [ "$last_lat" != "" -a "$last_lng" != "" ]; then
#				echo -e "$seg_id\t$last_lng\t$last_lat\t$lng\t$lat\t$edge_id" | tee -a $seg_file
			echo -e "$seg_id\t$last_lng\t$last_lat\t$lng\t$lat\t$edge_id" >> $seg_file
#			fi
		done

	done<&3

	exec 3<&-
	exec 3>&-

	echo -e "[INFO]\tFinish gen_segs... ..."
}

gen_segs $edges_file $segs_file


