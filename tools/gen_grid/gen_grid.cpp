#include "gen_distance_table.h"

double test()
{
	GenDistanceTable *gen_tbl = new GenDistanceTable();
	double mLat1 = 39.90923; // point1纬度
	double mLon1 = 116.357428; // point1经度
	double mLat2 = 39.90923;// point2纬度
	double mLon2 = 116.397428;// point2经度
	double distance = gen_tbl->GetShortDistance(mLon1, mLat1, mLon2, mLat2);
	cout << "LongDistance:\t" << distance << endl;
}


int main()
{
	test();
	return 0;
}


