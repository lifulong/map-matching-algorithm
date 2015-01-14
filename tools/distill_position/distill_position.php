<?php

function distill_position($positions, $num_list)
{
	$res_position = array();

	foreach($num_list as $num)
		$res_position[] = $positions[$num];

	return $res_position;
}

function get_positions($position_file)
{
	$json_string = file_get_contents($position_file);

	$positions = json_decode($json_string, True);

	return $positions;
}

/**
 *
 * @positions: position array
 * @dump_file:
 *
 **/
function dump_positions($positions, $dump_file)
{
	$json_string = json_encode($positions);
	file_put_contents($dump_file, $json_string);

	return;
}

function get_num_list($num_file)
{
	$json_string = file_get_contents($num_file);

	$num_list = json_decode($json_string, True);
	echo "num_list:".count($num_list)."\n";
	echo "num_list:".var_dump($num_list)."\n";

	return $num_list;
}

function main()
{
	$num_list = get_num_list("num_list");
	$positions = get_positions("test.json");
	$res_positions = distill_position($positions, $num_list);
	dump_positions($res_positions, "dump.json");
}

main();



