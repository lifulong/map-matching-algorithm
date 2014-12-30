#include <iostream>
#include <vector>

#include "query_grid.h"

using namespace std;

void test()
{
	vector<seg_point_map> maps;
	maps.clear();

	QueryGrid *query_grid = new QueryGrid("segs.txt", "grid_info", "grid.txt");
	//query_grid->loadGrid("grid_ifo", "grid.txt");
	printf("begin query grid segs......\n");
	maps = query_grid->getGridSegs(115.5, 40.0);

	printf("maps_size: %d\n", maps.size());
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
		printf("on_seg:\t%d\n", iter->on_seg);
		printf("\n");
	}
}


int main()
{
	test();
	return 0;
}


