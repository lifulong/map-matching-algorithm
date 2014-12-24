/**
 * impletation of build grid index of road network for map matching
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "common-struct.h"
#include "common.h"
#include "logger.h"

using namespace std;



#define GRID_BY_GAP
#define GRID_BY_LEN
#undef GRID_BY_LEN

#define LINE_BUFFER_LEN		4096


class MapIndex {

	public:

		MapIndex(string vnode_file, string edge_file, string geo_file, string logfile="")
		{
			logger = new Logger(logfile);
			logger->Info("Init MapIndex...");

			this->inodes.clear();
			this->iedges.clear();
			this->igeos.clear();
			this->grid.clear();

			logger->Info("Loading vnode...");
			this->_loading_inodes(vnode_file);
			logger->Info("Loading edge...");
			this->_loading_iedges(edge_file);
			logger->Info("Loading geos...");
			this->_loading_igeos(geo_file);

			logger->Info("Init MapIndex Over...");
		}

	private:
		double start_lat, end_lat, start_lng, end_lng;
		double lat_gap, lng_gap;
		int grid_lat_len, grid_lng_len;
		Logger *logger;

		vector<struct inode> inodes;
		vector<struct iedge> iedges;
		vector<struct igeo> igeos;

		vector<struct edge> grid;

		int _loading_inodes(string vnode_file)
		{
			char buffer[LINE_BUFFER_LEN];
			vector<string> res;
			ifstream infile;

			infile.open(vnode_file.c_str());

			if(!infile) {
				//Log::
				return 1;
			}

			while(infile.getline(buffer, LINE_BUFFER_LEN))
			{
				res = SplitBySep(buffer, "\\t");
				if(res.size() != 3)
				{
					cout << "Parse " + vnode_file + " error, line seg is not 3." << endl;
					continue;
				}
				cout << res[0] << ":" << res[1] << ":" << res[2] << endl;
				struct inode inode;
				inode.node_index = atoi(res[0].c_str());
				inode.point.lat = atof(res[1].c_str());
				inode.point.lng = atof(res[2].c_str());
				this->inodes.push_back(inode);
			}

			infile.close();

			return 0;
		}

		int _loading_iedges(string edge_file)
		{
			char buffer[LINE_BUFFER_LEN];
			vector<string> res;
			ifstream infile;

			infile.open(edge_file.c_str());

			if(!infile) {
				//Log::
				return 1;
			}

			while(infile.getline(buffer, LINE_BUFFER_LEN))
			{
				res = SplitBySep(buffer, "\t");
				if(res.size() != 3)
				{
					cout << "Parse " + edge_file + " error, line seg is not 3." << endl;
					continue;
				}
				struct iedge iedge;
				iedge.edge_index = atoi(res[0].c_str());
				iedge.start_vnode_index = atoi(res[1].c_str());
				iedge.end_vnode_index = atoi(res[2].c_str());
				this->iedges.push_back(iedge);
			}

			infile.close();

			return 0;
		}

		int _loading_igeos(string geo_file)
		{
			int i = 0;
			char buffer[LINE_BUFFER_LEN];
			vector<string> res;
			ifstream infile;

			infile.open(geo_file.c_str());

			if(!infile) {
				//Log::
				return 1;
			}

			while(infile.getline(buffer, LINE_BUFFER_LEN))
			{
				res = SplitBySep(buffer, "\t");
				if(res.size() < 5)
				{
					cout << "Parse " + geo_file + " error, line seg is less than 5." << endl;
					continue;
				}
				struct igeo igeo;
				igeo.points.clear();
				for(vector<string>::iterator iter = res.begin(); iter != res.end(); iter++, i++)
				{
					if(0 == i)
						igeo.edge_index = atoi((*iter).c_str());
					else
					{
						struct point point;
						point.lat = atof((*iter).c_str());
						iter++;
						point.lat = atof((*iter).c_str());
						igeo.points.push_back(point);
					}
				}
				this->igeos.push_back(igeo);
			}

			infile.close();

			return 0;
		}

		int convert_to_standard()
		{
			return 0;
		}

		int build_grid_index()
		{
			return 0;
		}

};



#ifdef TEST_MAPINDEX

int main()
{
	cout << "Test is running..." << endl;
	MapIndex *map_index = new MapIndex("../../mapdata/vertices.txt", "../../mapdata/edges.txt", "../../mapdata/geos.txt");

	return 0;
}

#endif


