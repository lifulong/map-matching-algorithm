
#include <vector>

using namespace std;

struct seg{
	int		seg_id;
	int		edge_id;
	double	lat;
	double	lng;
};

class GenMapGrid{

	public:
		GenMapGrid(string segs_file, string log_file="")
		{
		}

		void DumpGrid(string dump_file="")
		{
		}

	private:

		FILE *_logger;
		vector<struct seg> **grid;
		double start_lat, end_lat, start_lng, end_lng, lat_gap, lng_gap;
		int lat_num, lng_num;

		void _init()
		{
			*_logger = NULL;
			lat_num = lng_num = 0;
			start_lat = 39.4;
			end_lat = 41.6;
			start_lat = 115.7;
			end_lat = 117.4;
			lat_gap = lng_gap = 0.001;
		}
};


