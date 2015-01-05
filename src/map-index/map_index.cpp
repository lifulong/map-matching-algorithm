/**
 * impletation of build grid index of road network for map matching
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include "common-struct.h"
#include "common.h"
#include "map_index.h"


MapIndex::MapIndex(string vnode_file, string edge_file, string geo_file, string logfile="")
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

void MapIndex::loading_inodes(string vnode_file)
{
	char buffer[LINE_BUFFER_LEN];
	vector<string> res;
	ifstream infile;

	infile.open(vnode_file.c_str());

	if(!infile) {
		//Log::
		return ;
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

	return ;
}

void MapIndex::loading_iedges(string edge_file)
{
	char buffer[LINE_BUFFER_LEN];
	vector<string> res;
	ifstream infile;

	infile.open(edge_file.c_str());

	if(!infile) {
		//Log::
		return ;
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

	return ;
}

void MapIndex::loading_igeos(string geo_file)
{
	int i = 0;
	char buffer[LINE_BUFFER_LEN];
	vector<string> res;
	ifstream infile;

	infile.open(geo_file.c_str());

	if(!infile) {
		//Log::
		return ;
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

	return ;
}


