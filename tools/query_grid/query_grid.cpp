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
		memset(buffer, 0, READ_BUFFER_LEN);
		grid_data.clear();
		EchoRunning();
		grid_data = SplitBySep(buffer, "\t");
		/*
		i = atoi(grid_data[0].c_str());
		j = atoi(grid_data[1].c_str());
		start_lng = atof(grid_data[2].c_str());
		start_lat = atof(grid_data[3].c_str());
		end_lng = atof(grid_data[4].c_str());
		end_lat = atof(grid_data[5].c_str());
		
		if(i < 0 || i > this->lng_num) {
			debug_msg("grid lng index error, %d.\n", i);
			int k;
			for(k=0; k<grid_data.size(); k++)
			{
				cout<<grid_data[k]<<endl;
			}
			return;
			continue;
		}

		if(j < 0 || j > this->lat_num) {
			debug_msg("grid lat index error, %d.\n", j);
			int k;
			for(k=0; k<grid_data.size(); k++)
			{
				cout<<grid_data[k]<<endl;
			}
			return;
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
			//seg = this->segs[seg_id - 1];
			//this->grid[i][j].node_segs.push_back(seg);
			pos++;
		}
		*/
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

	i = (lng - this->start_lng) / this->lng_gap + 1;
	j = (lat - this->start_lat) / this->lat_gap + 1;

	if(i >= this-> lng_num || j >= this->lat_num)
	{
		i = -1;
		j = -1;
	}

	if(i < 0 || j < 0) {
		/*
		cout << "getGridIndexByPoint: lng_num = " << lng_num << " lat_num = " << lat_num << endl;
		cout << "getGridIndexByPoint: i = " << i << " j = " << j << endl;
		cout << "getGridIndexByPoint start_lng:" << this->start_lng << " start_lat:" << this->start_lat << endl;
		cout << "getGridIndexByPoint lng:" << lng << " lat:" << lat << endl;
		*/
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

bool QueryGrid::on_seg(double lng, double lat, double start_lng, double start_lat, double end_lng, double end_lat)
{
	if(((start_lng - lng) * (lng - end_lng)) > 0)
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
 *  distance = distance(P3,P0) = |P3-P1|*sin(theta)
 *  lng(P0) = (ms_distance*lng(P2) + sign_flag*lng(P1)*|P3-P1|) / sign_flag*|P3-P1| + ms_distance
 *  lat(P0) = (ms_distance*lat(P2) + sign_flag*lat(P1)*|P3-P1|) / sign_flag*|P3-P1| + ms_distance
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
	double distance, ms_distance;
	seg_point_map map;

	start_lng = seg.start_lng;
	start_lat = seg.start_lat;
	end_lng = seg.end_lng;
	end_lat = seg.end_lat;

	es_distance = this->cal_line_distance(start_lng, start_lat, end_lng, end_lat);
	ps_distance = this->cal_line_distance(start_lng, start_lat, lng, lat);
	cos_theta = (end_lng-start_lng)*(end_lat-start_lat)/es_distance*ps_distance;
	sign_flag = (cos_theta>=0) ? 1 : -1;
	ms_distance = cos_theta * ps_distance;
	distance = ps_distance * sin(acos(cos_theta));
	map_lng = (ms_distance*end_lat + sign_flag*start_lat*es_distance)/(sign_flag*es_distance+ms_distance);
	map_lat = (ms_distance*end_lng + sign_flag*start_lng*es_distance)/(sign_flag*es_distance+ms_distance);

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
	map.on_seg = on_seg;

	return map;
}

vector<struct seg_point_map> QueryGrid::mapGridSegs(double lng, double lat, vector<seg> segs)
{
	struct seg_point_map map;
	vector<seg_point_map> seg_point_maps;

	seg_point_maps.clear();

	for(vector<seg>::iterator iter = segs.begin(); iter != segs.end(); iter++)
	{
		map = this->mapGridSeg(lng, lat, *iter);
		seg_point_maps.push_back(map);
	}

	return seg_point_maps;
}

vector<struct seg_point_map> QueryGrid::getGridSegs(double lng, double lat)
{
	int i, j, index_i, index_j, tmp_i, tmp_j;
	struct grid_index index;
	vector<seg> segs;
	//vector<seg_point_map> seg_point_maps;

	segs.clear();
	//seg_point_maps.clear();

	index = this->getGridIndexByPoint(lng, lat);

	index_i = index.i - 1;
	index_j = index.j - 1;

	for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
		{
			tmp_i = index_i + i;
			tmp_j = index_j + j;
			for(vector<seg>::iterator iter = this->grid[tmp_i][tmp_j].node_segs.begin(); 
					iter != this->grid[tmp_i][tmp_j].node_segs.begin(); iter++) {
				segs.push_back(*iter);
			}
		}

	return this->mapGridSegs(lng, lat, segs);
}


