#!/bin/bash

fix_file=deviation

edges_file=edges.txt
geos_file=geos.txt
vertices_file=vertices.txt

edges_fix_file=${edges_file%"."}"_valid.txt"
geos_fix_file=${geos_file%"."}"_valid.txt"
vertices_fix_file=${vertices_file%"."}"_valid.txt"

fix_lat=""
fix_lng=""

function load_fix_info()
{
	echo `date +%H:%M:%S`"[INFO] start load fix info ..."
	fix_info_file=$1

	while read line
	do
		read fix_lng fix_lat<<-EOF
			$line
		EOF
		return
	done<$fix_info_file

	if [ "$fix_lng" == "" -o "$fix_lat" == "" ];then
		echo -e "load fix info error.\texited.\n";
		exit 1;
	fi
	echo `date +%H:%M:%S`"[INFO] end load fix info ..."
}

function fix_edges()
{
	return
}

function fix_vertices()
{
	echo `date +%H:%M:%S`"[INFO] start fix vertices ..."
	local vertices_file=$1
	local vertices_fix_file=$2

	[ -f $vertices_fix_file ] && { rm -f $vertices_fix_file; }

	let i=0
	while read line
	do

		let i=$i+1
		let label=$i%4

		case $label in
			0) echo -n -e "\b\b\b---";;
			1) echo -n -e "\b\b\b\\\\\\";;
			2) echo -n -e "\b\b\b|||";;
			3) echo -n -e "\b\b\b///";;
		esac

		read node_id lat lng<<-EOF
			$line
		EOF

		lat=`echo "scale=6;$lat+$fix_lat" | bc`
		lng=`echo "scale=6;$lng+$fix_lng" | bc`
		echo -e "$node_id\t$lat\t$lng" >> $vertices_fix_file

	done<$vertices_file
	echo `date +%H:%M:%S`"[INFO] end fix vertices ..."
}


function fix_geos()
{
	echo `date +%H:%M:%S`"[INFO] start fix geos ..."
	local geos_file=$1
	local geos_fix_file=$2

	[ -f $geos_fix_file ] && { rm -f $geos_fix_file; }

	let i=0
	while read line
	do

		let i=$i+1
		let label=$i%4

		case $label in
			0) echo -n -e "\b\b\b---";;
			1) echo -n -e "\b\b\b\\\\\\";;
			2) echo -n -e "\b\b\b|||";;
			3) echo -n -e "\b\b\b///";;
		esac

		read seg_id rest<<-EOF
			$line
		EOF

		new_road="$seg_id"
		while [ "$rest" != "" ]
		do
			read lat lng rest<<-EOF
				$rest
			EOF
			lat=`echo "scale=6;$lat+$fix_lat" | bc`
			lng=`echo "scale=6;$lng+$fix_lng" | bc`
			new_road=${new_road}"\t"${lat}"\t"${lng}
		done

		echo -e "$new_road" >> $geos_fix_file

	done<$geos_file
	echo `date +%H:%M:%S`"[INFO] end fix geos ..."
}


load_fix_info $fix_file
#fix_vertices $vertices_file $vertices_fix_file
fix_geos $geos_file $geos_fix_file

