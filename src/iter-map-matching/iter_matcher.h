/**
 *	@Thanks : The algothrim here base on
 *		"A Map Matching Method for GPS Based Real-Time Vehicle Location"
 *		Author : G. R. Jagadeesh, T. Srikanthan and X. D. Zhang
 *		Nanyang Technological University, Singapore
 *		Email : jagadeesh@pmail.ntu.edu.sg
 *
 *	@Description :
 *
 *	@Author : lifulong
 *	@Email : msl.fulong@gmail.com
 *	@Date : 2015-01-05
 **/


#include <vector>
#include <map>

using namespace std;

struct seg {
	int		seg_id;
	int		edge_id;
	double	start_lng;
	double	start_lat;
	double	end_lng;
	double	end_lat;
};

struct seg_point_map {
	int		seg_id;
	int		edge_id;
	double	start_lng;
	double	start_lat;
	double	end_lng;
	double	end_lat;

	double	lng;
	double	lat;
	double	map_lng;
	double	map_lat;
	double	distance;
	double	min_distance;
	bool	on_seg;
};


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
	double	curr_dis;
	double	curr_heading;
	double	dis_deviation;
	double	heading_deviation;
	int		orientation;			//在当前路段的行驶方向[0(unkown),1,-1]
	struct seg			current_seg;
	vector<seg>			adjacent;
	vector<seg>			candidates;
	vector<map_point>	points;
};

class Matcher {

	public:

		Matcher();

		int initMatcher();
		uint32_t genUniqId();
		int doMatch(struct position, struct map_track map_track);
		int match(uint32_t id, struct position);
		int batch_match(struct trajectory);

	private:

		uint32_t					current_id;
		uint32_t					max_id;		//assign it with (uint32_t)-1
		double						query_distance;
		map<uint32_t, map_track>	track_map;
		Logger						*logger;

		//FIXME: temp map-index here, use map-index service later
		MapIndex					*map_index;
};


