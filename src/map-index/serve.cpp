/**
 *
 * Use Ace.
 *
 *
 *
 ***/

#include "map_index.h"

#ifdef TEST_MAP_INDEX

#include <string>
#include <vector>

using namespace std;

#include <stdio.h>

void test()
{
	vector<seg_point_map> maps;
	maps.clear();

	MapIndex *map_index = new MapIndex("", "", "", "segs.txt", "grid_info", "grid.txt");
	printf("begin query grid segs......\n");
	maps = map_index->getGridSegs(115.714691, 39.570098, 200);

	printf("maps_size: %lu\n", maps.size());
	for(vector<seg_point_map>::iterator iter = maps.begin(); iter != maps.end(); iter++)
	{
		printf("Map:\n");
		printf("seg_id:\t%d\n", iter->seg_id);
		printf("edge_id:\t%d\n", iter->edge_id);
		printf("start_lng:\t%f\n", iter->start_lng);
		printf("start_lat:\t%f\n", iter->start_lat);
		printf("end_lng:\t%f\n", iter->end_lng);
		printf("end_lat:\t%f\n", iter->end_lat);
		printf("lng:\t%f\n", iter->lng);
		printf("lat:\t%f\n", iter->lat);
		printf("map_lng:\t%f\n", iter->map_lng);
		printf("map_lat:\t%f\n", iter->map_lat);
		printf("distance:\t%f\n", iter->distance);
		printf("min_distance:\t%f\n", iter->min_distance);
		printf("on_seg:\t%d\n", iter->on_seg);
		printf("\n");
	}
}

int main()
{
	test();
	return 0;
}

#else

int main()
{
	return 0;
}

#endif

