#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "map_index.h"

#define READ_BUFFER_LINE	4096

double MapIndex::round(double val, double mod, char type)
{
#define MULTI	100000
	int i_val, i_mod;
	int tmp;

	i_val = val*MULTI;
	i_mod = mod*MULTI;

	if(0 == type)		//up bound
	{
		tmp = i_val % i_mod;
		if(0 != tmp)
			i_val = i_val - tmp + i_mod;
		val = (double)i_val / MULTI;
	}
	else if(1 == type)		//down bound
	{
		tmp = i_val % i_mod;
		if(0 != tmp)
			i_val = i_val - tmp;
		val = (double)i_val / MULTI;
	}
	else
	{
		return val;
	}

	return val;
}

void MapIndex::preprocessGridInfo(string type)
{
	if(0 == strcmp(type.c_str(), "round"))
	{
		debug_msg("%f\t%f\t%f\t%f\n", map_start_lat, map_end_lat, map_start_lng, map_end_lng);
		map_start_lat = this->round(map_start_lat, map_lat_gap, 1);
		map_end_lat = this->round(map_end_lat, map_lat_gap, 0);
		map_start_lng = this->round(map_start_lng, map_lng_gap, 1);
		map_end_lng = this->round(map_end_lng, map_lng_gap, 0);
	}

	debug_msg("%f\t%f\t%f\t%f\t%f\t%f\n", map_start_lat, map_end_lat, map_start_lng, map_end_lng, map_lat_gap, map_lng_gap);
	map_lat_num = fabs(map_end_lat - map_start_lat) / map_lat_gap + 1;
	map_lng_num = fabs(map_end_lng - map_start_lng) / map_lng_gap + 1;
	debug_msg("lat_num:%d\tlng_num:%d\n", map_lat_num, map_lng_num);
}


void MapIndex::genGrid(string segs_file)
{
	FILE *fp;
	char buffer[READ_BUFFER_LINE];
	struct seg seg;
	vector<string> seg_info;

	if(segs_file == "")
		return;

	debug_msg("Start genGrid ... ...\n");

	seg_info.clear();

	fp = fopen(segs_file.c_str(), "r");
	while(NULL != fgets(buffer, READ_BUFFER_LINE, fp))
	{
		EchoRunning();
		seg_info = SplitBySep(buffer, "\t");
		if(seg_info.size() != 6)
			continue;
		seg.seg_id = atoi(seg_info[0].c_str());
		seg.start_lng = atof(seg_info[1].c_str());
		seg.start_lat = atof(seg_info[2].c_str());
		seg.end_lng = atof(seg_info[3].c_str());
		seg.end_lat = atof(seg_info[4].c_str());
		seg.edge_id = atoi(seg_info[5].c_str());
		this->updateGrids(seg);
	}
	fclose(fp);
	debug_msg("genGrid Finish.\n");
}

void MapIndex::dumpGrid(string dump_file="")
{
	int i, j;
	FILE *fp;

	if("" == dump_file)
		fp = stdout;
	else
	{
		fp = fopen(dump_file.c_str(), "w");
	}

	debug_msg("Start dumpGrid ... ...\n");

	for(i = 0; i < map_lng_num; i++)
	{
		for(j = 0; j < map_lat_num; j++)
		{
			//format:i j start_lng start_lat end_lng end_lat seg_id1 ... seg_idn
			fprintf(fp, "%d\t%d\t%f\t%f\t%f\t%f", i, j, grid[i][j].start_lng, grid[i][j].start_lat,
					grid[i][j].end_lng, grid[i][j].end_lat);
			for(vector<seg>::iterator iter = grid[i][j].node_segs.begin(); iter != grid[i][j].node_segs.end(); iter++)
			{
				fprintf(fp, "\t%d", iter->seg_id);
			}
			fprintf(fp, "\n");
		}
	}

	if("" != dump_file)
	{
		fclose(fp);
	}

	debug_msg("dumpGrid Finish.\n");
}


//private functions


void MapIndex::initGenGrid()
{
	int i, j;

	grid = new grid_node*[map_lng_num];
	for(i = 0; i < map_lng_num; i++)
	{
		grid[i] = new grid_node[map_lat_num];
	}

	/**
	 * first dimension is horizontal, lng
	 * second dimension is vertical, lat
	 */
	for(i = 0; i < map_lng_num; i++)
		for(j = 0; j < map_lat_num; j++) {
			grid[i][j].start_lng = this->map_start_lng + i*this->map_lng_gap;
			grid[i][j].end_lng = grid[i][j].start_lng + this->map_lng_gap;
			grid[i][j].start_lat = this->map_start_lat + j*this->map_lat_gap;
			grid[i][j].end_lat = grid[i][j].start_lat + this->map_lat_gap;
			grid[i][j].node_segs.clear();
		}
}

struct grid_index MapIndex::getGridIndexByPoint(double lng, double lat)
{
	int i=0, j=0;

	struct grid_index index = {-1, -1};

	if(lng < this->map_start_lng || lng > this->map_end_lng || lat < this->map_start_lat || lat > this->map_end_lat)
		return index;

	//i = (lng - this->start_lng) / this->lng_gap + 1;
	//j = (lat - this->start_lat) / this->lat_gap + 1;
	i = (lng - this->map_start_lng) / this->map_lng_gap;
	j = (lat - this->map_start_lat) / this->map_lat_gap;

	if(i >= this->map_lng_num || j >= this->map_lat_num)
	{
		debug_msg("getGridIndexByPoint: i = %d j = %d.\n", i, j);
		i = -1;
		j = -1;
	}

	if(i < 0 || j < 0) {
		debug_msg("getGridIndexByPoint lng: %f,lat: %f.\n", lng, lat);
		debug_msg("getGridIndexByPoint start_lng: %f,start_lat: %f.\n", this->map_start_lng, this->map_start_lat);
		debug_msg("getGridIndexByPoint: lng_num = %d, lat_num = %d.\n", map_lng_num, map_lat_num);
		debug_msg("getGridIndexByPoint: i = %d j = %d.\n", i, j);
	}

	index.i = i;
	index.j = j;

	return index;
}


void MapIndex::updateGrid(struct grid_index index, struct seg seg)
{
	if(index.i < 0 || index.i > map_lng_num || index.j < 0 || index.j > map_lat_num)
	{
		cout << "updateGrid:index valid: i = " << index.i << " j = " << index.j << endl;
		return;
	}
	this->grid[index.i][index.j].node_segs.push_back(seg);
	//printf("i:%d\tj:%d\tsize:%d\n", index.i, index.j, grid[index.i][index.j].node_segs.size());
}

void MapIndex::updateHorizontalGrids(double k, double start_lng, double start_lat, 
		double end_lng, double end_lat, struct seg seg)
{
	double curr_lng, curr_lat;
	struct grid_index index={0, 0}, last_index={0, 0};
	
	curr_lng = start_lng;
	curr_lat = start_lat;

	while(curr_lng <= end_lng)
	{
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		last_index = index;
		this->updateGrid(index, seg);
		curr_lng = curr_lng + map_lng_gap;
	}

	if(curr_lng > end_lng)
	{
		curr_lng = end_lng;
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		if(last_index.i != index.i || last_index.j != index.j)
			this->updateGrid(index, seg);
	}

}

void MapIndex::updateVerticalGrids(double k, double start_lng, double start_lat, 
		double end_lng, double end_lat, struct seg seg)
{
	double curr_lng, curr_lat;
	struct grid_index index={0, 0}, last_index={0, 0};
	
	curr_lat = start_lat;
	curr_lng = start_lng;

	while(curr_lat <= end_lat)
	{
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		last_index = index;
		this->updateGrid(index, seg);
		curr_lat = curr_lat + map_lat_gap;
	}

	if(curr_lat > end_lat)
	{
		curr_lat = end_lat;
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		if(last_index.i != index.i || last_index.j != index.j)
			this->updateGrid(index, seg);
	}
}

void MapIndex::updateHorizOrientGrids(double k, double start_lng, double start_lat, 
		double end_lng, double end_lat, struct seg seg)
{
	double curr_lng, curr_lat;
	struct grid_index index={0, 0}, last_index={0, 0};

	curr_lng = start_lng;
	curr_lat = start_lat;

	while(curr_lng <= end_lng)
	{
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		last_index = index;
		this->updateGrid(index, seg);
		curr_lng = curr_lng + map_lng_gap;
		curr_lat = k * (curr_lng - start_lng) + start_lat;
	}

	if(curr_lng > end_lng)
	{
		curr_lng = end_lng;
		curr_lat = k * (curr_lng - start_lng) + start_lat;
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		if(last_index.i != index.i || last_index.j != index.j)
			this->updateGrid(index, seg);
	}
}

void MapIndex::updateVertOrientGrids(double k, double start_lng, double start_lat, 
		double end_lng, double end_lat, struct seg seg)
{	
	double curr_lng, curr_lat;
	struct grid_index index={0, 0}, last_index={0, 0};

	curr_lat = start_lat;
	curr_lng = start_lng;

	while(curr_lat <= end_lat)
	{
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		last_index = index;
		this->updateGrid(index, seg);
		curr_lat = curr_lat + map_lat_gap;
		curr_lng = (curr_lat - start_lat) / k + start_lng;
	}

	if(curr_lng > end_lng)
	{
		curr_lat = end_lat;
		curr_lng = (curr_lat - start_lat) / k + start_lng;
		index = this->getGridIndexByPoint(curr_lng, curr_lat);
		if(last_index.i != index.i || last_index.j != index.j)
			this->updateGrid(index, seg);
	}
}

void MapIndex::updateGrids(struct seg seg)
{
	double k = 0, abs_k = 0;
	double left_lat=0, left_lng=0, right_lat=0, right_lng=0;
	double buttom_lat=0, buttom_lng=0, top_lat=0, top_lng=0;
	// 1:horizontal		2:vertical		3:horiz_orientation	4:vertical_orientation
	int direction = 0;

	if(seg.start_lat == seg.end_lat) {

		k = 0;
		direction = 1;
	} else if(seg.start_lng == seg.end_lng) {

		k = HUGE_VAL;
		direction = 2;
	} else {

		k = (seg.start_lng - seg.end_lng)/(seg.start_lat - seg.end_lat);
		abs_k = fabs(k);

		if(abs_k <= 1) {
			direction = 3;
		} else {
			direction = 4;
		}
	}

	if(2 == direction || 4 == direction) {

		if(seg.start_lng <= seg.end_lng) {

			buttom_lng = seg.start_lng;
			buttom_lat = seg.start_lat;
			top_lng = seg.end_lng;
			top_lat = seg.end_lat;
		} else {

			buttom_lng = seg.end_lng;
			buttom_lat = seg.end_lat;
			top_lng = seg.start_lng;
			top_lat = seg.start_lat;
		}

	} else if(1 == direction || 3 == direction) {

		if(seg.start_lat <= seg.end_lat) {

			left_lng = seg.start_lng;
			left_lat = seg.start_lat;
			right_lng = seg.end_lng;
			right_lat = seg.end_lat;
		} else {

			left_lng = seg.end_lng;
			left_lat = seg.end_lat;
			right_lng = seg.start_lng;
			right_lat = seg.start_lat;
		}
	}

	switch(direction)
	{
		case 0:
			break;
		case 1:
			updateHorizontalGrids(abs_k, left_lng, left_lat, right_lng, right_lat, seg);
			break;
		case 2:
			updateVerticalGrids(abs_k, buttom_lng, buttom_lat, top_lng, top_lat, seg);
			break;
		case 3:
			updateHorizOrientGrids(abs_k, left_lng, left_lat, right_lng, right_lat, seg);
			break;
		case 4:
			updateVertOrientGrids(abs_k, buttom_lng, buttom_lat, top_lng, top_lat, seg);
			break;
		default:
			break;
	}
}


