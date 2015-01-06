#include <fstream>

using namespace std;

#include <math.h>

#include "gps_preprocess.h"
#include "../../json/json.h"


GpsPreprocess::GpsPreprocess(string data_file)
{
	if("" == data_file)
		return;

	cout << "no support input file now." << endl;
}

int GpsPreprocess::getErrno()
{
	return 0;
}

string GpsPreprocess::getErrMsg()
{
	return "";
}

void GpsPreprocess::loadJsonGps(string gps_file)
{
	string json_string = "";
	ifstream infile;
#define LINE_BUFFER_LEN	1*1024*1024
	char buffer[LINE_BUFFER_LEN];

	infile.open(gps_file, ios_base::in);
	if(!infile.good())
		return ;

	while(infile.getline(buffer, LINE_BUFFER_LEN).good())
	{
		json_string += string(buffer);
	}

	infile.close();

	js.clear();
	js = json(json_string);

	cout << "json_string:" << json_string << endl;
	cout << "json_size:" << js.size() << endl;
	cout << "to_string:" << js.to_string() << endl;

	return ;
}

void GpsPreprocess::dumpJsonGps(string dump_file)
{
}

/*
static const double DEF_PI = 3.14159265359; // PI
static const double DEF_2PI = 6.28318530712; // 2*PI
static const double DEF_PI180 = 0.01745329252; // PI/180.0
static const double DEF_R = 6370693.5; // radius of earth
*/

double GpsPreprocess::cal_line_distance(double start_lng, double start_lat, double end_lng, double end_lat)
{
	double distance, sq_distance;

	sq_distance = (end_lng - start_lng)*(end_lng - start_lng) + (end_lat - start_lat)*(end_lat - start_lat);
	distance = sqrt(sq_distance);

	return distance;
}

double GpsPreprocess::cal_short_distance(double lon1, double lat1, double lon2, double lat2)
{
		double ew1, ns1, ew2, ns2;
		double dx, dy, dew;
		double distance;
		// 角度转换为弧度
		ew1 = lon1 * DEF_PI180;
		ns1 = lat1 * DEF_PI180;
		ew2 = lon2 * DEF_PI180;
		ns2 = lat2 * DEF_PI180;
		// 经度差
		dew = ew1 - ew2;
		// 若跨东经和西经180 度，进行调整
		if (dew > DEF_PI)
			dew = DEF_2PI - dew;
		else if (dew < -DEF_PI)
			dew = DEF_2PI + dew;
		dx = DEF_R * cos(ns1) * dew; // 东西方向长度(在纬度圈上的投影长度)
		dy = DEF_R * (ns1 - ns2); // 南北方向长度(在经度圈上的投影长度)
		// 勾股定理求斜边长
		distance = sqrt(dx * dx + dy * dy);
		return distance;
}

double GpsPreprocess::cal_long_distance(double lon1, double lat1, double lon2, double lat2)
{
		double ew1, ns1, ew2, ns2;
		double distance;
		// 角度转换为弧度
		ew1 = lon1 * DEF_PI180;
		ns1 = lat1 * DEF_PI180;
		ew2 = lon2 * DEF_PI180;
		ns2 = lat2 * DEF_PI180;
		// 求大圆劣弧与球心所夹的角(弧度)
		distance = sin(ns1) * sin(ns2) + cos(ns1) * cos(ns2) * cos(ew1 - ew2);
		// 调整到[-1..1]范围内，避免溢出
		if (distance > 1.0)
			distance = 1.0;
		else if (distance < -1.0)
			distance = -1.0;
		// 求大圆劣弧长度
			distance = DEF_R * acos(distance);
		return distance;
}


