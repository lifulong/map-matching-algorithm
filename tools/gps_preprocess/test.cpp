
#include <iostream>

using namespace std;

#include "gps_preprocess.h"

void test()
{
	GpsPreprocess *gp = new GpsPreprocess();
	//gp->loadJsonGps("./../../datasets/14099164_1.json");		//1 line
	gp->loadJsonGps("./../../datasets/14096444_1.json");		//many line
	gp->appendLineHeading();		//many line
	gp->appendLongSpeed();		//many line
}

int main()
{
	test();
	return 0;
}

