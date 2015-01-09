
#include "iter_matcher.h"

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

double Matcher::calHeading(double lng1, double lat1, double lng2, double lat2)
{
	double theta;
	double fake_lng, fake_lat;
	double distance1, distance2;

	fake_lng = lng2;
	fake_lat = lat1;

	distance1 = this->calDistance(lng1, lat1, lng2, lat2);
	distance2 = this->calDistance(lng1, lat1, fake_lng, fake_lat);

	//FIXME: check if distance1 or distance2 equal zero
	cos_theta = ((lng2-lng1)*(fake_lng-lng1)+(lat2-lat1)*(fake_lat-lat1)) / (distance1*distance2);

	theta = acos(cos_theta);

	return theta;
}

/**
 *
 *
 *			^
 *			|
 *		1	|\
 *			| \
 *		0.8	|_ \
 *			|  |\
 *			|  | \
 *			|  |  \
 *			|  |   \
 *		--------------------> degrees
 *		0	|  18*  90*
 *
 *
 *
 *			^
 *			|
 *		1	|\
 *			| \
 *		0.7	|_ \
 *			|  |\
 *			|  | \
 *			|  |  \
 *			|  |   \
 *		--------------------> distance
 *		0	|  15m  50m
 *
 *
 * @InfoNeededForMatcher:
 *		on_road: true or false
 *		candidate_segs:[vector]
 *			has update_candidate: true or false
 *			current_seg: seg_id, distance_ratio_from_begin
 *			road seg: seg_stru, heading, average_distance, successor_seg
 *		:
 **/

int Matcher::doMatch(struct position, struct map_track map_track)
{
	double heading;
	double distance;
	double curr_lng, curr_lat, lng, lat;
	vector<seg> candidates;
	vector<seg_point_map> seg_maps;

	curr_lng = map_track.curr_lng;
	curr_lat = map_track.curr_lat;
	candidates.clear();
	candidates = map_track.candidates;
	lng = position.lng;
	lat = position.lat;

	heading = this->calHeading(curr_lng, curr_lat, lng, lat);

	//seg_maps = this->map_index->queryGrid(lng, lat, this->query_distance);
	//update map_track candidates

	//seg_maps = this->map_index->batchMapPoint2Seg();

	for(vector<seg>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {

		seg_map = this->map_index->mapPoint2Seg(lng, lat, *iter);
	}

	return 0;
}

int Matcher::match(uint32_t id, struct position)
{
	struct map_track map_track;

	map_track = this->track_map[id];

	this->doMatch(position, map_track);

	//FIXME: update track_map here

	return 0;
}

