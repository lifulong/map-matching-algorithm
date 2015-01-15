<?php


/**
  * @roads: geos info has been loaded to memory
  *
  **/
function distill_roads($roads, $num_list)
{
	$road_positions = array();

	if(count($num_list) != 0)
		foreach($num_list as $num)
		{
			$positions = $roads[$num];
			if(empty($positions))
				echo "get positions for ".$num." error.\n";
			$road_positions[] = $positions;
		}
	else
	{
		while(list($road_id, $positions) = each($roads))
		{
			if(empty($positions))
				echo "get positions for ".$road_id." error.\n";
			$road_positions[] = $positions;
		}
	}

	return $road_positions;
}

function load_roads($roads_file)
{
	$content = file_get_contents($roads_file);
	$array_roads = explode("\n", $content);
	$road_positions = array();
	foreach ($array_roads as $road)
	{
		$positions = array();
		if(empty($road))
		{
			echo "parse an empty road.\n";
			continue;
		}

		$road_segs = explode("\t", $road);
		if(count($road_segs) % 2 != 1)
		{
			echo "parse an error road: ".$road_segs[0]." length is ".count($road_segs).".\n";
			continue;
		}

		$road_id = $road_segs[0];
		for($i = 1; $i < count($road_segs); $i+=2)
		{
			$position = array('lat' => $road_segs[$i], 'lng' => $road_segs[$i+1]);
			$positions[] = $position;
		}
		$road_positions[$road_id] = $positions;

	}
	return $road_positions;
}

function load_num_list($num_list_file)
{
	$num_list_string = file_get_contents($num_list_file);
	$num_list = json_decode($num_list_string, True);
	return $num_list;
}

function dump_road_positions($road_positions, $dumpfile)
{
	//while(list($road_id, $positions) = each($road_positions))
	foreach($road_positions as $positions)
	{
		$json_string = json_encode($positions);
		file_put_contents($dumpfile, $json_string."\n", FILE_APPEND);
	}
}


function main()
{
	echo "Staring distill ... ...\n";

	$roads = load_roads("geos.txt");
	$num_list = load_num_list("road_list");
	$road_positions = distill_roads($roads, $num_list);
	if(!empty($road_positions))
		dump_road_positions($road_positions, "road_positions");

	echo "Done.\n";
}


main();

