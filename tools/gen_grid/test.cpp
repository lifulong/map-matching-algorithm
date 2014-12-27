#include "gen_grid.h"

void test()
{
	GenMapGrid *gen_grid = new GenMapGrid("segs.txt");
	gen_grid->dumpGrid("dump.txt");
	/*
	double mLat1 = 39.90923; // point1纬度
	double mLon1 = 116.357428; // point1经度
	double mLat2 = 39.90923;// point2纬度
	double mLon2 = 116.397428;// point2经度
	*/
	//double distance = gen_tbl->GetShortDistance(mLon1, mLat1, mLon2, mLat2);
}


int main()
{
	test();
	return 0;
}


