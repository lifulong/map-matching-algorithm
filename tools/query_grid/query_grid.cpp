#include <string.h>
#include <fstream>

#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "query_grid.h"

#define READ_BUFFER_LEN		40960

QueryGrid::QueryGrid(string segs_file, string grid_info, string grid_file)
{
	if("" == segs_file || "" == grid_info || "" == grid_file)
		return;

	this->loadSegs(segs_file);

	this->loadGrid(grid_info, grid_file);
}

int QueryGrid::getErrno()
{
	return this->errno;
}

string QueryGrid::getErrMsg()
{
	return this->errmsg;
}


void QueryGrid::init()
{
	int i, j;

	grid = new grid_node*[lng_num];
	for(i = 0; i < lng_num; i++)
	{
		grid[i] = new grid_node[lat_num];
	}
	for(i = 0; i < lng_num; i++)
	{
		for(j = 0; j < lat_num; j++)
		{
			grid[i][j].node_segs.clear();
		}
	}
}

void QueryGrid::loadSegs(string segs_file)
{
	ifstream infile;
	char buffer[READ_BUFFER_LEN];
	struct seg seg;
	vector<string> seg_data;

	debug_msg("[INFO]loading segs... ...\n");
	infile.open(segs_file.c_str(), ifstream::in);

	this->segs.push_back(seg);	//FIXME:First seg is padding, invalid.

	while(!infile.getline(buffer, READ_BUFFER_LEN).eof())
	{
		seg_data = SplitBySep(buffer, "\t");
		if(seg_data.size() != 6)
			continue;
		seg.seg_id = atoi(seg_data[0].c_str());
		seg.start_lng = atof(seg_data[1].c_str());
		seg.start_lat = atof(seg_data[2].c_str());
		seg.end_lng = atof(seg_data[3].c_str());
		seg.end_lat = atof(seg_data[4].c_str());
		seg.edge_id = atoi(seg_data[5].c_str());
		this->segs.push_back(seg);
	}

	infile.close();
	debug_msg("[INFO]loading segs ok... ...\n");
}

void QueryGrid::setValue(string key, string value)
{
	if(0 == strcmp(key.c_str(), "start_lng")) {

		this->start_lng = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "start_lat")) {

		this->start_lat = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "end_lng")) {

		this->end_lng = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "end_lat")) {

		this->end_lat = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "lng_gap")) {

		this->lng_gap = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "lat_gap")) {

		this->lat_gap = atof(value.c_str());
	} else {

		debug_msg("unkown key value detect: key=%s, value=%s.\n", key.c_str(), value.c_str());
	}
}

void QueryGrid::loadGridInfo(string grid_info)
{
	FILE *fp;
	char buffer[READ_BUFFER_LEN];
	vector<string> key_value;
	string key, value;

	fp = fopen(grid_info.c_str(), "r");

	while(fgets(buffer, READ_BUFFER_LEN, fp))
	{
		key_value = SplitBySep(buffer, ":");
		if(key_value.size() != 2)
		{
			debug_msg("load grid info error.\n");
			continue;
		}
		key = key_value[0];
		value = key_value[1];
		this->setValue(key, value);
	}

	fclose(fp);

	//FIXME: start_lng end_lng ... check
	this->lng_num = fabs(start_lng - end_lng) / lng_gap + 1;
	this->lat_num = fabs(start_lat - end_lat) / lat_gap + 1;
}

void QueryGrid::dumpGridInfo()
{
	cout << "dumpGridInfo:" << endl;
	cout << "start_lng:" << start_lng << endl;
	cout << "start_lat:" << start_lat << endl;
	cout << "end_lng:" << end_lng << endl;
	cout << "end_lat:" << end_lat << endl;
	cout << "lng_gap:" << lng_gap << endl;
	cout << "lat_gap:" << lat_gap << endl;
	cout << endl;
}

void QueryGrid::loadGridData(string grid_file)
{
	int i, j, pos, len, seg_id, segs_size;
	double start_lng, start_lat, end_lng, end_lat;
	FILE *fp;
	char buffer[READ_BUFFER_LEN];
	vector<string> grid_data;
	struct seg seg;

	debug_msg("[INFO]loading grid ... ...\n");
	segs_size = this->segs.size();
	fp = fopen(grid_file.c_str(), "r");

	memset(buffer, 0, READ_BUFFER_LEN);
	while(fgets(buffer, READ_BUFFER_LEN, fp))
	{
		grid_data.clear();
		EchoRunning();
		grid_data = SplitBySep(buffer, "\t");
		if(grid_data.size() < 6) {
			debug_msg("grid data size error, %d.\n", grid_data.size());
			debug_msg("grid data size error, %s.\n", buffer);
			continue;
		}
		i = atoi(grid_data[0].c_str());
		j = atoi(grid_data[1].c_str());
		start_lng = atof(grid_data[2].c_str());
		start_lat = atof(grid_data[3].c_str());
		end_lng = atof(grid_data[4].c_str());
		end_lat = atof(grid_data[5].c_str());
		
		if(i < 0 || i > this->lng_num) {
			debug_msg("grid lng index error, %d.\n", i);
			continue;
		}

		if(j < 0 || j > this->lat_num) {
			debug_msg("grid lat index error, %d.\n", j);
			continue;
		}

		this->grid[i][j].start_lng = start_lng;
		this->grid[i][j].start_lat = start_lat;
		this->grid[i][j].end_lng = end_lng;
		this->grid[i][j].end_lat = end_lat;

		pos = 6;
		len = grid_data.size();
		while(pos < len)
		{
			seg_id = atoi(grid_data[pos].c_str());
			if(seg_id <= 0 || seg_id > segs_size) {
				debug_msg("seg_id invlid, seg_id=%d,segs_size=%d.\n", seg_id, segs_size);
				pos++;
				continue;
			}
			seg = this->segs[seg_id - 1];
			this->grid[i][j].node_segs.push_back(seg);
			pos++;
		}
	}

	fclose(fp);
	debug_msg("[INFO]loading grid ok ... ...\n");
}

void QueryGrid::loadGrid(string grid_info, string grid_file)
{
	this->loadGridInfo(grid_info);
	this->dumpGridInfo();
	this->init();
	this->loadGridData(grid_file);
}

struct grid_index QueryGrid::getGridIndexByPoint(double lng, double lat)
{	
	int i=0, j=0;

	struct grid_index index;

	i = (lng - this->start_lng) / this->lng_gap;
	j = (lat - this->start_lat) / this->lat_gap;

	if(i >= this-> lng_num || j >= this->lat_num)
	{
		debug_msg("getGridIndexByPoint: i = %d j = %d.\n", i, j);
		i = -1;
		j = -1;
	}

	if(i < 0 || j < 0) {
		debug_msg("getGridIndexByPoint lng: %f,lat: %f.\n", lng, lat);
		debug_msg("getGridIndexByPoint start_lng: %f,start_lat: %f.\n", this->start_lng, this->start_lat);
		debug_msg("getGridIndexByPoint: lng_num = %d, lat_num = %d.\n", lng_num, lat_num);
		debug_msg("getGridIndexByPoint: i = %d j = %d.\n", i, j);
	}

	index.i = i;
	index.j = j;

	return index;
}

double QueryGrid::cal_line_distance(double start_lng, double start_lat, double end_lng, double end_lat)
{
	double distance, sq_distance;

	sq_distance = (end_lng - start_lng)*(end_lng - start_lng) + (end_lat - start_lat)*(end_lat - start_lat);
	distance = sqrt(sq_distance);

	return distance;
}

double QueryGrid::cal_short_distance(double lon1, double lat1, double lon2, double lat2)
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

double QueryGrid::cal_long_distance(double lon1, double lat1, double lon2, double lat2)
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

bool QueryGrid::on_seg(double lng, double lat, double start_lng, double start_lat, double end_lng, double end_lat)
{
	if(((start_lng - lng) * (lng - end_lng)) >= 0)
		return true;
	return false;
}

/**
 *				 +  P3
 *             .
 *           .
 *         .
 *       .
 *     . )theta
 * P1 +----------+-----------+ P2
 *               P0
 *
 * (P3-P1)(P2-P1) = |P3-P1|*|P2-P1|*cos(theta)
 *  ms_distance = distance(P0,P1) = |P3-P1|*cos(theta)
 *  es_distance = distance(P2,P1) = |P2-P1|
 *  distance = distance(P3,P0) = |P3-P1|*sin(theta)
 *  lng(P0) = (ms_distance*(lng(P2)-lng(P1))*sign_flag + lng(P1)*|P2-P1|) / es_distance
 *  lat(P0) = (ms_distance*(lat(P2)-lat(P1))*sign_flag + lat(P1)*|P2-P1|) / es_distance
 */
seg_point_map QueryGrid::mapGridSeg(double lng, double lat, seg seg)
{
	bool on_seg;
	int sign_flag = 1;
	double cos_theta;
	//double theta, cos_theta, sin_theta;
	double map_lng, map_lat;
	double start_lng, start_lat, end_lng, end_lat;
	double es_distance, ps_distance;	//es_distance:end to start  ps_distance: point to start
	double pe_distance, min_distance;
	double distance, ms_distance;
	seg_point_map map;

	start_lng = seg.start_lng;
	start_lat = seg.start_lat;
	end_lng = seg.end_lng;
	end_lat = seg.end_lat;

	es_distance = this->cal_short_distance(start_lng, start_lat, end_lng, end_lat);
	ps_distance = this->cal_short_distance(start_lng, start_lat, lng, lat);
	pe_distance = this->cal_short_distance(end_lng, end_lat, lng, lat);
	cos_theta = ((end_lng-start_lng)*(lng-start_lng)+(end_lat-start_lat)*(lat-start_lat))/es_distance*ps_distance;
	sign_flag = (cos_theta>=0) ? 1 : -1;
	ms_distance = fabs(cos_theta) * ps_distance;
	distance = ps_distance * fabs(sin(acos(cos_theta)));
	map_lng = (ms_distance*(end_lng - start_lng)*sign_flag + es_distance*start_lng)/es_distance;
	map_lat = (ms_distance*(end_lat - start_lat)*sign_flag + es_distance*start_lat)/es_distance;

	min_distance = ps_distance > pe_distance ? pe_distance : ps_distance;
	on_seg = this->on_seg(map_lng, map_lat, start_lng, start_lat, end_lng, end_lat);

	map.seg_id = seg.seg_id;
	map.edge_id = seg.edge_id;
	map.start_lng = seg.start_lng;
	map.start_lat = seg.start_lat;
	map.end_lng = seg.end_lng;
	map.end_lat = seg.end_lat;

	map.lng = lng;
	map.lat = lat;
	map.map_lng = map_lng;
	map.map_lat = map_lat;
	map.distance = distance;
	map.min_distance = min_distance;
	map.on_seg = on_seg;

	return map;
}

vector<struct seg_point_map> QueryGrid::mapGridSegs(double lng, double lat, double distance, vector<seg> segs)
{
	struct seg_point_map map;
	vector<seg_point_map> seg_point_maps;

	seg_point_maps.clear();

	for(vector<seg>::iterator iter = segs.begin(); iter != segs.end(); iter++)
	{
		map = this->mapGridSeg(lng, lat, *iter);
		if(0 != distance) {

			if((map.on_seg = true && map.distance < distance) || (map.on_seg = false && map.min_distance < distance))
				seg_point_maps.push_back(map);
		} else {

			seg_point_maps.push_back(map);
		}
	}

	return seg_point_maps;
}

vector<struct seg_point_map> QueryGrid::getGridSegs(double lng, double lat, double distance)
{
	int i, j, index_i, index_j, tmp_i, tmp_j;
	struct grid_index index;
	vector<seg> segs;
	vector<seg_point_map> seg_point_maps;

	segs.clear();
	seg_point_maps.clear();

	index = this->getGridIndexByPoint(lng, lat);

	if(index.i == -1 || index.j == -1) {
		debug_msg("get grid index by point error, %d, %d.\n", index.i, index.j);
		return seg_point_maps;
	}
	index_i = index.i - 1;
	index_j = index.j - 1;

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
		{
			tmp_i = index_i + i;
			tmp_j = index_j + j;

			if(tmp_i < 0 || tmp_i > this->lng_num) {
				continue;
			}

			if(tmp_j < 0 || tmp_j > this->lat_num) {
				continue;
			}

			for(vector<seg>::iterator iter = this->grid[tmp_i][tmp_j].node_segs.begin(); 
					iter != this->grid[tmp_i][tmp_j].node_segs.end(); iter++) {
				segs.push_back(*iter);
			}
		}

	return this->mapGridSegs(lng, lat, distance, segs);
}


