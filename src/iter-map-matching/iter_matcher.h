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

#include "../map-index/map_index.h"


//accept c_str as input parameters.
#define debug_2_console(fmt,args...)	\
	do{									\
		time_t t_time;					\
		time(&t_time);					\
		printf("%s", ctime(&t_time));	\
		printf(fmt,##args);				\
	}while(0)

//accept c_str as input parameters.
#define debug_2_file(fmt,args...)	\
	do{								\
		FILE* fp;					\
		fp = fopen("log", "a+");	\
		fprintf(fp, fmt, ##args);	\
		fclose(fp);					\
	}while(0)

#define DEBUG_2_CONSOLE

#ifdef DEBUG_2_FILE
#define debug_msg(fmt,args...) debug_2_file(fmt,##args)
#elif defined DEBUG_2_CONSOLE
#define debug_msg(fmt,args...) debug_2_console(fmt,##args)
#else
#define debug_msg(fmt,args...)	do{} while(0)
#endif


/*
struct point {
	double		lng;
	double		lat;
};

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
*/


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

struct seg_candidate {
	struct seg		seg;
	double			seg_heading;
	double			dis_deviation;	//average distance
	double			heading_deviation;	//average heading
	int				map_point_num;
};

struct map_track {
	int				uniq_id;
	double			last_lng;
	double			last_lat;
	double			last_dis;
	double			last_heading;
	int				orientation;			//在当前路段的行驶方向[0(unkown),1,-1]
	struct seg_candidate	current_seg;
	//vector<seg>				adjacent;	//update by orientation
#define MAX_CANDIDATE_LEN	5
	vector<seg_candidate>	candidates;
	vector<map_point>		points;
};

class Matcher {

	public:

		Matcher();

		int initMatcher();
		uint32_t genUniqId();
		vector<point> doMatch(struct position position, struct map_track map_track);
		vector<point> match(uint32_t id, struct position position);
		vector<point> batch_match(struct trajectory trajectory);

	private:

		static constexpr double DEF_PI = 3.14159265359; // PI
		static constexpr double DEF_2PI = 6.28318530712; // 2*PI
		static constexpr double DEF_PI180 = 0.01745329252; // PI/180.0
		static constexpr double DEF_R = 6370693.5; // radius of earth

		uint32_t					current_id;
		uint32_t					max_id;		//assign it with (uint32_t)-1
		double						query_distance;
		map<uint32_t, map_track>	track_map;
		//Logger						*logger;

		//FIXME: temp map-index here, use map-index service later
		MapIndex					*map_index;

		double calDistance(double lng1, double lat1, double lng2, double lat2);
		double calHeading(double lng1, double lat1, double lng2, double lat2);
		double headingResemblance(double seg_heading, double heading);
		double distanceResemblance(double average_distance, double distance);
};


