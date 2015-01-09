
#include <iostream>

using namespace std;

#include "gps_preprocess.h"

#define USAGE(method)	("Usage:\n"+method+"\tfilename\n")

void test(string filename)
{
	size_t pos;
	string dumpfile, basename, ext;

	pos = filename.find_last_of(".");
	if(string::npos == pos)
	{
		basename = filename;
		ext = "";
		dumpfile = basename + "_append";
	}
	else
	{
		basename = filename.substr(0, pos);
		ext = filename.substr(pos+1);
		dumpfile = basename + "_append" + "." + ext;
	}


	GpsPreprocess *gp = new GpsPreprocess();
	//gp->loadJsonGps("./../../datasets/14099164_1.json");		//1 line
	gp->loadJsonGps(filename);		//many line
	gp->appendLineHeading();		//many line
	gp->appendLongSpeed();		//many line
	//gp->dumpJsonGps();
	gp->dumpJsonGpsBeauty(dumpfile);
}

int main(int argc ,char *argv[])
{
	string filename;

	if(1 != argc && 2 != argc) {
		string usage = USAGE(string(argv[0]));
		debug_msg("%s", usage.c_str());
		return 1;
	}

	if(2 == argc)
		filename = string(argv[1]);
	else
		filename = "./../../datasets/9710356_1.json";

	test(filename);

	return 0;
}



