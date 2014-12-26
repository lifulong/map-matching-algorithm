#include "gen_distance_table.h"

int main()
{
	double bj_start_lat = 39.4, bj_end_lat = 41.6, bj_start_lng = 115.7, bj_end_lng = 117.4;

	GenDistanceTable *genTable = new GenDistanceTable();

	genTable->genLngDistance(bj_start_lat, bj_end_lat, 0.1, "lat_table.txt");

	genTable->genLatDistance(116.0, bj_start_lat, "start_lng_table.txt");

	genTable->genLatDistance(116.0, bj_end_lat, "end_lng_table.txt");

	return 0;
}
