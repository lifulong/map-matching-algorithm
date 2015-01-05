/**
 * implement of build grid index of road network for map matching
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

#include "error.h"
#include "logger.h"
#include "debug.h"


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


/*
#define GRID_BY_GAP
#define GRID_BY_LEN
#undef GRID_BY_LEN
*/

#define LINE_BUFFER_LEN		40960

struct point {
	double	lng;
	double	lat;
};

struct inode {
	int				node_index;
	struct point	point;
};

struct iedge {
	int		edge_index;
	int		start_inode_index;
	int		end_inode_index;
};

struct igeo {
	int		edge_index;
	vector<point>	points;
};

struct seg {
	int		seg_id;
	int		edge_id;
	double	start_lng;
	double	start_lat;
	double	end_lng;
	double	end_lat;
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


class MapIndex {

	public:

		MapIndex(string nodes_file, string edges_file, string geos_file, string segs_file, 
				string grid_info, string grid_file, string logfile="");

		int getErrno();
		string getErrMsg();

		vector<struct seg_point_map> getGridSegs(double lng, double lat, double distance=0);

	private:
		static const double DEF_PI = 3.14159265359; // PI
		static const double DEF_2PI = 6.28318530712; // 2*PI
		static const double DEF_PI180 = 0.01745329252; // PI/180.0
		static const double DEF_R = 6370693.5; // radius of earth

		double start_lng, end_lng, start_lat, end_lat;
		double lat_gap, lng_gap;
		int lat_num, lng_num;
		//int grid_lat_len, grid_lng_len;
		int errno;
		string errMsg;
		Logger *logger;
		vector<struct inode> inodes;
		vector<struct iedge> iedges;
		vector<struct igeo> igeos;
		vector<struct seg> segs;
		//FIXME: use map(index_stru, grid_node) instead
		struct grid_node **grid;


		void loadNodes(string nodes_file);
		void loadEdges(string edges_file);
		void loadGeos(string geos_file);
		void loadSegs(string segs_file);
		void setValue(string key, string value);
		void loadGridInfo(string grid_info);
		void dumpGridInfo();
		void loadGridData(string grid_file);

		void initGrid();

		void initMapIndex();

		//gen segs
		void genSegs(string geos_file="");
		void dumpSegs(string segs_file="");

		//gen grid
		double round(double val, double mod, char type);
		void preprocess_grid_info(string type);

		void initGenGrid();
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
		void genGrid(string segs_file="");
		void dumpGrid(string dump_file);

		//query grid
		double cal_line_distance(double start_lng, double start_lat, double end_lng, double end_lat);
		double cal_short_distance(double lon1, double lat1, double lon2, double lat2);
		double cal_long_distance(double lon1, double lat1, double lon2, double lat2);
		bool on_seg(double lng, double lat, double start_lng, double start_lat, double end_lng, double end_lat);
		seg_point_map mapGridSeg(double lng, double lat, seg seg);
		vector<struct seg_point_map> mapGridSegs(double lng, double lat, double distance, vector<seg> segs);
};

