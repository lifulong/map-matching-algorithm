
#include <math.h>

#include "common.h"
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

double Matcher::calHeading(double lng1, double lat1, double lng2, double lat2)
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

	distance1 = this->calDistance(lng1, lat1, lng2, lat2);
	distance2 = this->calDistance(lng1, lat1, fake_lng, fake_lat);

	//FIXME: check if distance1 or distance2 equal zero
	if(isEqualDouble(distance1, 0) || isEqualDouble(distance2, 0)) {
		debug_msg("dis1:%f\tdis2:%f\n", distance1, distance2);
		return 0.0;
	}
	cos_theta = ((lng2-lng1)*(fake_lng-lng1)+(lat2-lat1)*(fake_lat-lat1)) / (distance1*distance2);

	theta = acos(cos_theta);
	if(lat2 < lat1)
		theta = DEF_2PI - theta;
	//debug_msg("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", lng2-lng1, fake_lng-lng1, lat2-lat1, fake_lat-lat1, distance1, distance2, cos_theta, theta);

	return theta;
}

/**
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
 * @seg_heading: degree as the parameter
 * @heading: degree as the parameter
 *
 **/

double Matcher::headingResemblance(double seg_heading, double heading)
{
	double heading_diff;
	double resemblance;

	heading_diff = fabs(heading - seg_heading);

	resemblance = (90.0 - heading_diff) / 90.0;

	return resemblance;
}

/**
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
 **/

double Matcher::distanceResemblance(double average_distance, double distance)
{
	double resemblance;

	resemblance = (50 - distance) / 50.0;

	return resemblance;
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
	double heading, seg_heading;
	double distance;
	double heading_resemblance, distance_resemblance;
	double max_resemblance;
	double map_lng, map_lat;
	struct point map_point;
	struct seg_point_map seg_map;
	vector<seg_candidate> candidates;
	vector<point> map_points;

	last_lng = map_track.last_lng;
	last_lat = map_track.last_lat;
	lng = position.lng;
	lat = position.lat;
	max_resemblance = 0;
	map_lng = map_lat = 0.0;
	map_points.clear();
	candidates.clear();
	candidates = map_track.candidates;

	heading = this->calHeading(last_lng, last_lat, lng, lat);

	//seg_maps = this->map_index->queryGrid(lng, lat, this->query_distance);
	//update map_track candidates

	//seg_maps = this->map_index->batchMapPoint2Seg();

	for(vector<seg_candidate>::iterator iter = candidates.begin(); iter != candidates.end(); iter++) {

		seg_heading = iter->seg_heading;
		heading_resemblance = this->headingResemblance(seg_heading, heading);
		seg_map = this->map_index->mapGridSeg(lng, lat, iter->seg);
		if(seg_map.on_seg)
			distance = seg_map.distance;
		else
			distance = seg_map.min_distance;
		distance_resemblance = this->distanceResemblance(distance, distance);
		double tmp = Min(heading_resemblance, distance_resemblance);
		if(tmp > max_resemblance) {
			max_resemblance = tmp;
			map_lng = seg_map.map_lng;
			map_lat = seg_map.map_lat;
		}
	}

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

	//FIXME: update track_map here
	/*
	for(vector<point>::iterator iter = match_map_points.begin(); iter != match_map_points.end(); iter++)
	{
		map_points.push_back(*iter);
	}
	*/

	return map_points;
}

