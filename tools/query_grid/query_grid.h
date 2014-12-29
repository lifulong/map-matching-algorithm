#include <stdio.h>
#include <time.h>

#include <iostream>
#include <vector>

using namespace std;

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


#define BJ_START_LNG	115.25
#define BJ_START_LAT	39.26
#define BJ_END_LNG		117.4
#define BJ_END_LAT		41.1

#define LNG_GAP		0.005
#define LAT_GAP		0.005

#define START_LNG	BJ_START_LNG
#define START_LAT	BJ_START_LAT
#define END_LNG		BJ_END_LNG
#define END_LAT		BJ_END_LAT

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
	bool	on_seg;
};

struct grid_index {
	int i, j;
};

struct grid_node {
	double start_lng, end_lng;
	double start_lat, end_lat;
	double gap_lng, gap_lat;
	vector<struct seg> node_segs;
};

class QueryGrid {

	public:

		QueryGrid(string segs_file, string grid_info, string grid_file);
		int getErrno();
		string getErrMsg();
		void loadSegs(string segs_file);
		void loadGrid(string grid_info, string grid_file);
		void setValue(string key, string value);
		vector<struct seg_point_map> getGridSegs(double lng, double lat);

	private:

		FILE *_logger;
		vector<seg> segs;
		//map(index_stru, grid_node)
		struct grid_node **grid;
		double start_lat, end_lat, start_lng, end_lng, lat_gap, lng_gap;
		int lat_num, lng_num;
		int errno;
		string errmsg;

		void init();
		void loadGridInfo(string grid_info);
		void dumpGridInfo();
		void loadGridData(string grid_file);
		struct grid_index getGridIndexByPoint(double lng, double lat);
		double cal_line_distance(double start_lng, double start_lat, double end_lng, double end_lat);
		bool on_seg(double lng, double lat, double start_lng, double start_lat, double end_lng, double end_lat);
		seg_point_map mapGridSeg(double lng, double lat, seg seg);
		vector<struct seg_point_map> mapGridSegs(double lng, double lat, vector<seg> segs);
};


