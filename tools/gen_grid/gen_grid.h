#include <stdio.h>

#include <iostream>
#include <vector>

using namespace std;

//accept c_str as input parameters.
#define debug_2_console(fmt,args...)	\
	do{									\
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


#define BJ_START_LAT	39.4
#define BJ_END_LAT		41.6
#define BJ_START_LNG	115.7
#define BJ_END_LNG		117.4

#define LNG_GAP		0.005
#define LAT_GAP		0.005

#define START_LAT	BJ_START_LAT
#define END_LAT		BJ_END_LAT
#define START_LNG	BJ_START_LNG
#define END_LNG		BJ_END_LNG

struct seg {
	int		seg_id;
	int		edge_id;
	double	start_lat;
	double	start_lng;
	double	end_lat;
	double	end_lng;
};

struct grid_index {
	int i, j;
};

struct grid_node {
	double start_lat, end_lat;
	double start_lng, end_lng;
	vector<struct seg> node_segs;
};

class GenMapGrid {

	public:

		GenMapGrid(string segs_file);
		int getErrno();
		string getErrMsg();
		void genGrid(string segs_file);
		void dumpGrid(string dump_file);

	private:

		FILE *_logger;
		struct grid_node **grid;
		double start_lat, end_lat, start_lng, end_lng, lat_gap, lng_gap;
		int lat_num, lng_num;
		int errno;
		string errmsg;

		void _init();
		double _round(double val, double mod, char type);
		void _preprocess(string type);
		void _initGrid();
		struct grid_index getGridIndexByPoint(double lng, double lat);
		void updateGrid(struct grid_index index, struct seg seg);
		void updateHorizontalGrids(double k, double start_lng, double start_lat, 
				double end_lng, double end_lat, struct seg seg);
		void updateVerticalGrids(double k, double start_lng, double start_lat, 
				double end_lng, double end_lat, struct seg seg);
		void updateHorizOrientGrids(double k, double start_lng, double start_lat, 
				double end_lng, double end_lat, struct seg seg);
		void updateVertOrientGrids(double k, double start_lng, double start_lat, 
				double end_lng, double end_lat, struct seg seg);
		void updateGrids(struct seg seg);
};


