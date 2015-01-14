#include <string.h>
#include <fstream>

#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "map_index.h"

#define READ_BUFFER_LEN		40960


double MapIndex::calLineDistance(double start_lng, double start_lat, double end_lng, double end_lat)
{
	double distance, sq_distance;

	sq_distance = (end_lng - start_lng)*(end_lng - start_lng) + (end_lat - start_lat)*(end_lat - start_lat);
	distance = sqrt(sq_distance);

	return distance;
}

double MapIndex::calShortDistance(double lon1, double lat1, double lon2, double lat2)
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

double MapIndex::calLongDistance(double lon1, double lat1, double lon2, double lat2)
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

bool MapIndex::onSeg(double lng, double lat, double start_lng, double start_lat, double end_lng, double end_lat)
{
	if(((start_lng - lng)*(end_lng - lng) <= 0) && ((start_lat - lat)*(end_lat - lat) <= 0))
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
seg_point_map MapIndex::mapGridSeg(double lng, double lat, seg seg)
{
	bool on_seg;
	int sign_flag = 1;
	double cos_theta;
	//double theta, cos_theta, sin_theta;
	double map_lng, map_lat;
	double start_lng, start_lat, end_lng, end_lat;
	//es_distance:end to start  ps_distance: point to start
	double es_distance, ps_distance, ms_distance;
	double ps_short_distance, pe_short_distance, short_distance, min_short_distance;
	seg_point_map map;

	start_lng = seg.start_lng;
	start_lat = seg.start_lat;
	end_lng = seg.end_lng;
	end_lat = seg.end_lat;

	//FIXME:point is start_point or end_point
	es_distance = this->calLineDistance(start_lng, start_lat, end_lng, end_lat);
	ps_distance = this->calLineDistance(start_lng, start_lat, lng, lat);
	if(ps_distance == 0)
		cos_theta = 1;
	else
		cos_theta = ((end_lng-start_lng)*(lng-start_lng)+(end_lat-start_lat)*(lat-start_lat))/(es_distance*ps_distance);
	sign_flag = (cos_theta>=0) ? 1 : -1;
	ms_distance = fabs(cos_theta) * ps_distance;
	//distance = ps_distance * fabs(sin(acos(cos_theta)));
	map_lng = (ms_distance*(end_lng - start_lng)*sign_flag + es_distance*start_lng)/es_distance;
	map_lat = (ms_distance*(end_lat - start_lat)*sign_flag + es_distance*start_lat)/es_distance;

	ps_short_distance = this->calShortDistance(start_lng, start_lat, lng, lat);
	pe_short_distance = this->calShortDistance(end_lng, end_lat, lng, lat);
	short_distance =  this->calShortDistance(map_lng, map_lat, lng, lat);
	min_short_distance = ps_short_distance > pe_short_distance ? pe_short_distance : ps_short_distance;
	on_seg = this->onSeg(map_lng, map_lat, start_lng, start_lat, end_lng, end_lat);

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
	map.distance = short_distance;
	map.min_distance = min_short_distance;
	map.on_seg = on_seg;

	return map;
}

vector<struct seg_point_map> MapIndex::mapGridSegs(double lng, double lat, double distance, vector<seg> segs)
{
	struct seg_point_map map;
	vector<seg_point_map> seg_point_maps;

	seg_point_maps.clear();

	for(vector<seg>::iterator iter = segs.begin(); iter != segs.end(); iter++)
	{
		map = this->mapGridSeg(lng, lat, *iter);
		if(0 != distance) {

			if((map.on_seg == true && map.distance < distance) || (map.on_seg == false && map.min_distance < distance))
				seg_point_maps.push_back(map);
		} else {

			seg_point_maps.push_back(map);
		}
	}

	return seg_point_maps;
}

vector<struct seg_point_map> MapIndex::getGridSegs(double lng, double lat, double distance)
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

			if(tmp_i < 0 || tmp_i >= this->map_lng_num) {
				continue;
			}

			if(tmp_j < 0 || tmp_j >= this->map_lat_num) {
				continue;
			}

			for(vector<seg>::iterator iter = this->grid[tmp_i][tmp_j].node_segs.begin(); 
					iter != this->grid[tmp_i][tmp_j].node_segs.end(); iter++) {

				vector<seg>::iterator iter2;
				for(iter2 = segs.begin(); iter2 != segs.end(); iter2++) {
					if(iter->seg_id == iter2->seg_id)
						break;
				}

				if(iter2 == segs.end())
					segs.push_back(*iter);
			}
		}

	return this->mapGridSegs(lng, lat, distance, segs);
}


