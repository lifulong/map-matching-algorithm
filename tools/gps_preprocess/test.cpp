
#include <iostream>

using namespace std;

#include "gps_preprocess.h"

void test()
{
	GpsPreprocess *gp = new GpsPreprocess();
	gp->loadJsonGps("");
}

int main()
{
	test();
	return 0;
}

