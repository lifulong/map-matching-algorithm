
#include <iostream>

using namespace std;

#include "../../json/json.h"
#include "common.h"
#include "simple_matcher.h"

#define USAGE(method)	("Usage:\n"+method+"\tfilename\n")

json loadJsonGps(string gps_file)
{
	json js;
	string json_string = "";
	ifstream infile;
#undef LINE_BUFFER_LEN
#define LINE_BUFFER_LEN	1*1024*1024
	char buffer[LINE_BUFFER_LEN];

	js.clear();
	infile.open(gps_file, ios_base::in);
	if(!infile.good())
		return js;

	while(infile.getline(buffer, LINE_BUFFER_LEN).good())
	{
		json_string += string(buffer);
	}
	json_string += string(buffer);

	infile.close();

	js = json::parse(json_string);

	//debug_msg("json_size:%d.\n", js.size());

	return js;
}

void dumpJsonGps(json js, string dump_file)
{
	string json_string = "";
	FILE *fp;

	if("" != dump_file) {
		fp = fopen(dump_file.c_str(), "w");
		//FIXME: log the error here.
		if(NULL == fp)
			fp = stdout;
	}
	else
		fp = stdout;

	json_string = js.to_string();

	fwrite(json_string.c_str(), json_string.length(), 1, fp);

	if(stdout != fp)
		fclose(fp);
}


void test(string filename)
{
	unsigned int i, len;
	size_t pos;
	struct position position;
	vector<point> points;
	string dumpfile, basename, ext;
	json js;
	Matcher *matcher = new Matcher();

	pos = filename.find_last_of(".");
	if(string::npos == pos)
	{
		basename = filename;
		ext = "";
		dumpfile = basename + "_valid";
	}
	else
	{
		basename = filename.substr(0, pos);
		ext = filename.substr(pos+1);
		dumpfile = basename + "_valid" + "." + ext;
	}

	js = loadJsonGps(filename);
	len = js.size();
	for(i = 0; i < len; i++)
	{
		position.lat = String2Double(js[i]["lat"]);
		position.lng = String2Double(js[i]["lng"]);
		points = matcher->match(0, position);
		if(points.size() > 0) {
			js[i]["lat"] = Double2String(points[0].lat);
			js[i]["lng"] = Double2String(points[0].lng);
		} else {
			debug_msg("points size error:%u\n", points.size());
		}
		/*
		for(vector<point>::iterator iter=points.begin(); iter != points.end(); iter++)
		{
			js[i]["lat"] = Double2String(iter->lat);
			js[i]["lng"] = Double2String(iter->lng);
		}
		*/
	}

	dumpJsonGps(js, dumpfile);
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


