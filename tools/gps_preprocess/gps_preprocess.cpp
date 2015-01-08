#include <fstream>

using namespace std;

#include <math.h>

#include "common.h"
#include "gps_preprocess.h"
#include "../../json/json.h"

#define DEVIATION	1.E-10


GpsPreprocess::GpsPreprocess(string data_file)
{
	if("" == data_file)
		return;

	this->init(data_file);
}

void GpsPreprocess::init(string data_file)
{
	this->loadJsonGps(data_file);
}

bool GpsPreprocess::isEqualDouble(double val1, double val2)
{
	if(fabs(val1-val2) <= DEVIATION)
		return true;
	return false;
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
	json_string += string(buffer);

	infile.close();

	this->js.clear();
	this->js = json::parse(json_string);

	//debug_msg("json_size:%d.\n", js.size());

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

void GpsPreprocess::appendLongSpeed()
{
	unsigned int i;
	double last_lng, last_lat, lng, lat;
	double last_tm, tm;
	double speed;

	lng = String2Double(js[0]["lng"].to_string());
	lat = String2Double(js[0]["lat"].to_string());
	tm = String2Double(js[0]["gps_time"].to_string());
	this->js[0]["speed"] = 0;
	for(i = 1; i < this->js.size(); i++)
	{
		last_lng = lng;
		last_lat = lat;
		last_tm = tm;
		lng = String2Double(js[i]["lng"].to_string());
		lat = String2Double(js[i]["lat"].to_string());
		speed = this->calLongSpeed(last_lng, last_lat, lng, lat, last_tm, tm);
		debug_msg("%f\t%f\t%f\t%f\t%f\n", last_lng, last_lat, lng, lat, speed);
		this->js[i]["speed"] = speed;
	}

	return;
}

void GpsPreprocess::appendShortSpeed()
{
	return;
}

void GpsPreprocess::appendLineHeading()
{
	unsigned int i;
	double last_lng, last_lat, lng, lat;
	double heading;

	lng = String2Double(js[0]["lng"].to_string());
	lat = String2Double(js[0]["lat"].to_string());
	this->js[0]["heading"] = 0;
	for(i = 1; i < this->js.size(); i++)
	{
		last_lng = lng;
		last_lat = lat;
		lng = String2Double(js[i]["lng"].to_string());
		lat = String2Double(js[i]["lat"].to_string());
		heading = this->calLineHeading(last_lng, last_lat, lng, lat);
		//debug_msg("%f\t%f\t%f\t%f\t%f\t%f\n", last_lng, last_lat, lng, lat, heading, heading/DEF_PI180 );
		this->js[i]["heading"] = heading;
	}

	return;
}

double GpsPreprocess::calLineDistance(double start_lng, double start_lat, double end_lng, double end_lat)
{
	double distance, sq_distance;

	sq_distance = (end_lng - start_lng)*(end_lng - start_lng) + (end_lat - start_lat)*(end_lat - start_lat);
	distance = sqrt(sq_distance);

	return distance;
}

double GpsPreprocess::calShortDistance(double lon1, double lat1, double lon2, double lat2)
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

double GpsPreprocess::calLongDistance(double lon1, double lat1, double lon2, double lat2)
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

double GpsPreprocess::calLongSpeed(double lng1, double lat1, double lng2, double lat2, double time1, double time2)
{
	double distance, time_gap;

	if(isEqualDouble(time1, time2))
		return 0;

	time_gap = time2 - time1;
	distance = this->calLongDistance(lng1, lat1, lng2, lat2);

	return distance/time_gap;
}

double GpsPreprocess::calShortSpeed(double lng1, double lat1, double lng2, double lat2, double time1, double time2)
{
	double distance, time_gap;

	if(isEqualDouble(time1, time2))
		return 0;

	time_gap = time2 - time1;
	distance = this->calShortDistance(lng1, lat1, lng2, lat2);

	return distance/time_gap;
}

/**
 *				 +  P2
 *             .
 *           .
 *         .
 *       .
 *     . )theta
 * P1 +----------+  P3
 *
 *
 * (P2-P1(P3-P1) = |P2-P1|*|P3-P1|*cos(theta)
 */

double GpsPreprocess::calLineHeading(double lng1, double lat1, double lng2, double lat2)
{
	double theta;
	double cos_theta;
	double fake_lng, fake_lat;
	double distance1, distance2;

	if(isEqualDouble(lng1, lng2))
		return 0.0;

	if(isEqualDouble(lng1, lng2)) {

		if(lat2 > lat1)
			theta = DEF_PI/2;
		else
			theta = -DEF_PI/2;
		return theta;
	}

	fake_lng = lng1 + fabs(lng2 - lng1);
	fake_lat = lat1;

	distance1 = this->calLineDistance(lng1, lat1, lng2, lat2);
	distance2 = this->calLineDistance(lng1, lat1, fake_lng, fake_lat);

	//FIXME: check if distance1 or distance2 equal zero
	if(isEqualDouble(distance1, 0) || isEqualDouble(distance2, 0)) {
		debug_msg("%f\t%f\n", distance1, distance2);
		return 0.0;
	}
	cos_theta = ((lng2-lng1)*(fake_lng-lng1)+(lat2-lat1)*(fake_lat-lat1)) / (distance1*distance2);

	theta = acos(cos_theta);
	//debug_msg("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", lng2-lng1, fake_lng-lng1, lat2-lat1, fake_lat-lat1, distance1, distance2, cos_theta, theta);

	return theta;
}


