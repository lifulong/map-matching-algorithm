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
	local edge_file=$1
	local seg_file=$2
	let seg_id=0

	exec 3<> $edge_file

	while read line
	do
		read edge_id rest<<-EOF
			$line
		EOF

		while [ "$rest" != "" ];
		do
			read lat lng rest<<-EOF
				$rest
			EOF
			let seg_id=$seg_id+1
			echo -e "$seg_id\t$lat\t$lng\t$edge_id" | tee -a $seg_file
		done

	done<&3

	exec 3<&-
	exec 3>&-
}

gen_segs $edges_file $segs_file


