#define _ISOC99_SOURCE
#include <math.h>

#include "common.h"
#include "simple_matcher.h"

/*
struct position {
	int		timestamp;
	int		gps_time;
	double	lng;
	double	lat;
	double	accuracy;
	//string	gps_type;
};

struct map_point {
	int		seg_id;
	int		edge_id;
	double	lng;
	double	lat;
	double	dis_deviation;			//deviation
	double	heading_deviation;		//deviation
};

struct trajectory {
	int		uniq_id;
	vector<position>	positions;
};

struct map_track {
	int		uniq_id;
	double	curr_lng;
	double	curr_lat;
	double	curr_dis_deviation;
	double	curr_heading_deviation;
	double	dis_deviation;
	double	heading_deviation;
	vector<map_point>	points;
};
*/


/*
uint32_t					current_id;
uint32_t					max_id;		//assign it with (uint32_t)-1
map<uint32_t, map_track>	track_hash;
*/


Matcher::Matcher()
{
	this->initMatcher();
}

int Matcher::initMatcher()
{
	this->max_id = (uint32_t)-1;
	this->current_id = 0;

	return 0;
}


uint32_t Matcher::genUniqId()
{
	//Warn: if max_id is not (uint32_t)-1, remeber to fix mod max_id here.
	return this->current_id++;
}

double Matcher::calDistance(double lng1, double lat1, double lng2, double lat2)
{
	double sq_distance, distance;

	sq_distance = (lng2-lng1)*(lng2-lng1) + (lat2-lat1)*(lat2-lat1);
	distance = sqrt(sq_distance);

	return distance;
}

/**
 *
 * @algorithm:
 *		1.if the heading diffrence is small and tht distance between the road and the current gps estimated position is 
 *			small then resemblance of the road is high
 *		2.if the previous resemblance of the road is high and the current resemblance is high then the travel likelihood 
 *			of the road is high
 *		3.if the previous maximum resemblance of all roads in the candidate road list is low and the current maximum
 *			resemblance of all the roads in the list is low then the vehicle is off-road
 *
 *		4.when run 1/2 the seg , update candidates?
 *
 * @InfoNeededForMatcher:
 *		on_road: true or false
 *		candidate_segs:[vector]
 *			has update_candidate: true or false
 *			current_seg: seg_id, distance_ratio_from_begin
 *			road seg: seg_stru, heading, average_distance, successor_seg
 *
 **/

vector<point> Matcher::doMatch(struct position position, struct map_track map_track)
{
	double last_lng, last_lat, lng, lat;
	double map_lng, map_lat;
	struct point map_point;
	vector<seg_point_map> seg_maps;
	vector<point> map_points;

	last_lng = map_track.last_lng;
	last_lat = map_track.last_lat;
	lng = position.lng;
	lat = position.lat;
	map_lng = map_lat = 0.0;
	map_points.clear();

	//first time map point to road, map it to the latest road
	double tmp = INFINITY;
	seg_maps = this->map_index->getGridSegs(lng, lat, this->query_distance);
	//update candidates
	for(vector<seg_point_map>::iterator iter = seg_maps.begin(); iter != seg_maps.end(); iter++)
	{
		if(!iter->on_seg && iter->min_distance > this->query_distance)
			continue;
		if((iter->on_seg && iter->distance < tmp) || (!iter->on_seg && iter->min_distance < tmp))
		{
			map_lng = iter->map_lng;
			map_lat = iter->map_lat;
		}
	}

	if(map_lng == 0 && map_lat == 0)
		return map_points;

	map_point.lng = map_lng;
	map_point.lat = map_lat;
	map_points.push_back(map_point);

	return map_points;
}

vector<point> Matcher::match(uint32_t id, struct position position)
{
	vector<point> map_points;
	struct map_track map_track;

	map_points.clear();

	map_track = this->track_map[id];

	map_points = this->doMatch(position, map_track);

	return map_points;
}

