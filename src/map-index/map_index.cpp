/**
 * impletation of build grid index of road network for map matching
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include <math.h>

#include "common.h"
#include "map_index.h"

MapIndex::MapIndex(string nodes_file, string edges_file, string geos_file, string segs_file, 
				string grid_info, string grid_file, string logfile)
{
	//FIXME:init init_mode value by config genSeg, genGrid, loadGrid.
	string init_mode = "loadGrid";

	logger = new Logger(logfile);
	logger->Info("Init MapIndex...");

	this->initMapIndex();

	//FIXME: check valid.
	//regenerate segs and grid file
	if(init_mode == "genSeg" && nodes_file != "" && edges_file != "" && geos_file != "") {

		logger->Info("Loading vnode...");
		this->loadNodes(nodes_file);
		logger->Info("Loading edge...");
		this->loadEdges(edges_file);
		logger->Info("Loading geos...");
		this->loadGeos(geos_file);
		logger->Info("Gening segs...");
		this->genSegs();
		this->preprocess_grid_info("round");
		this->initGenGrid();
		logger->Info("Gening grid...");
		this->genGrid();
	} else if(init_mode == "genGrid" && segs_file != "") {

		logger->Info("Loading segs...");
		this->loadSegs(segs_file);
		this->preprocess_grid_info("round");
		this->initGenGrid();
		logger->Info("Gening grid...");
		this->genGrid();
	} else if(init_mode == "loadGrid" && segs_file != "" && grid_info != "" && grid_file != "") {

		logger->Info("Loading segs...");
		this->loadSegs(segs_file);
		logger->Info("Loading Grid...");
		this->loadGridInfo(grid_info);
		this->dumpGridInfo();
		this->initGrid();
		this->loadGridData(grid_file);
	} else {

		this->_errno = Error::ERR_PARA;
		this->errMsg = "parameter error while init map index...";
		return;
	}

	this->_errno = Error::ERR_OK;
	logger->Info("Init MapIndex Over...");
}

int MapIndex::getErrno()
{
	return this->_errno;
}

string MapIndex::getErrMsg()
{
	return this->errMsg;
}

void MapIndex::initMapIndex()
{
	//FIXME:use config file to decide which branch running to ...
	if(0) {

		start_lng = START_LNG;
		start_lat = START_LAT;
		end_lng = END_LNG;
		end_lat = END_LAT;
		lng_gap = LNG_GAP;
		lat_gap = LAT_GAP;
		lat_num = lng_num = 0;
	} else {

		start_lng = 0;
		start_lat = 0;
		end_lng = 0;
		end_lat = 0;
		lng_gap = 0;
		lat_gap = 0;
		lat_num = lng_num = 0;
	}

	this->inodes.clear();
	this->iedges.clear();
	this->igeos.clear();
}

void MapIndex::loadNodes(string node_file)
{
	char buffer[LINE_BUFFER_LEN];
	vector<string> res;
	ifstream infile;

	infile.open(node_file.c_str());

	if(!infile) {
		//Log::
		return ;
	}

	while(infile.getline(buffer, LINE_BUFFER_LEN))
	{
		res = SplitBySep(buffer, "\t");
		if(res.size() != 3)
		{
			cout << "Parse " + node_file + " error, line seg is not 3." << endl;
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

void MapIndex::loadEdges(string edge_file)
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
		iedge.start_inode_index = atoi(res[1].c_str());
		iedge.end_inode_index = atoi(res[2].c_str());
		this->iedges.push_back(iedge);
	}

	infile.close();

	return ;
}

void MapIndex::loadGeos(string geo_file)
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


void MapIndex::loadSegs(string segs_file)
{
	ifstream infile;
	char buffer[LINE_BUFFER_LEN];
	struct seg seg;
	vector<string> seg_data;

	debug_msg("[INFO]loading segs... ...\n");
	infile.open(segs_file.c_str(), ifstream::in);

	this->segs.push_back(seg);	//FIXME:First seg is padding, invalid.

	while(!infile.getline(buffer, LINE_BUFFER_LEN).eof())
	{
		seg_data = SplitBySep(buffer, "\t");
		if(seg_data.size() != 6)
			continue;
		seg.seg_id = atoi(seg_data[0].c_str());
		seg.start_lng = atof(seg_data[1].c_str());
		seg.start_lat = atof(seg_data[2].c_str());
		seg.end_lng = atof(seg_data[3].c_str());
		seg.end_lat = atof(seg_data[4].c_str());
		seg.edge_id = atoi(seg_data[5].c_str());
		this->segs.push_back(seg);
	}

	infile.close();
	debug_msg("[INFO]loading segs ok... ...\n");
}

void MapIndex::setValue(string key, string value)
{
	if(0 == strcmp(key.c_str(), "start_lng")) {

		this->start_lng = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "start_lat")) {

		this->start_lat = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "end_lng")) {

		this->end_lng = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "end_lat")) {

		this->end_lat = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "lng_gap")) {

		this->lng_gap = atof(value.c_str());
	} else if (0 == strcmp(key.c_str(), "lat_gap")) {

		this->lat_gap = atof(value.c_str());
	} else {

		debug_msg("unkown key value detect: key=%s, value=%s.\n", key.c_str(), value.c_str());
	}
}

void MapIndex::loadGridInfo(string grid_info)
{
	FILE *fp;
	char buffer[LINE_BUFFER_LEN];
	vector<string> key_value;
	string key, value;

	fp = fopen(grid_info.c_str(), "r");

	while(fgets(buffer, LINE_BUFFER_LEN, fp))
	{
		key_value = SplitBySep(buffer, ":");
		if(key_value.size() != 2)
		{
			debug_msg("load grid info error.\n");
			continue;
		}
		key = key_value[0];
		value = key_value[1];
		this->setValue(key, value);
	}

	fclose(fp);

	//FIXME: start_lng end_lng ... check
	this->lng_num = fabs(start_lng - end_lng) / lng_gap + 1;
	this->lat_num = fabs(start_lat - end_lat) / lat_gap + 1;
}

void MapIndex::dumpGridInfo()
{
	cout << "dumpGridInfo:" << endl;
	cout << "start_lng:" << start_lng << endl;
	cout << "start_lat:" << start_lat << endl;
	cout << "end_lng:" << end_lng << endl;
	cout << "end_lat:" << end_lat << endl;
	cout << "lng_gap:" << lng_gap << endl;
	cout << "lat_gap:" << lat_gap << endl;
	cout << endl;
}

void MapIndex::loadGridData(string grid_file)
{
	int i, j, pos, len, seg_id, segs_size;
	double start_lng, start_lat, end_lng, end_lat;
	FILE *fp;
	char buffer[LINE_BUFFER_LEN];
	vector<string> grid_data;
	struct seg seg;

	debug_msg("[INFO]loading grid ... ...\n");
	segs_size = this->segs.size();
	fp = fopen(grid_file.c_str(), "r");

	memset(buffer, 0, LINE_BUFFER_LEN);
	while(fgets(buffer, LINE_BUFFER_LEN, fp))
	{
		grid_data.clear();
		EchoRunning();
		grid_data = SplitBySep(buffer, "\t");
		if(grid_data.size() < 6) {
			debug_msg("grid data size error, %lu.\n", grid_data.size());
			debug_msg("grid data size error, %s.\n", buffer);
			continue;
		}
		i = atoi(grid_data[0].c_str());
		j = atoi(grid_data[1].c_str());
		start_lng = atof(grid_data[2].c_str());
		start_lat = atof(grid_data[3].c_str());
		end_lng = atof(grid_data[4].c_str());
		end_lat = atof(grid_data[5].c_str());
		
		if(i < 0 || i > this->lng_num) {
			debug_msg("grid lng index error, %d.\n", i);
			continue;
		}

		if(j < 0 || j > this->lat_num) {
			debug_msg("grid lat index error, %d.\n", j);
			continue;
		}

		this->grid[i][j].start_lng = start_lng;
		this->grid[i][j].start_lat = start_lat;
		this->grid[i][j].end_lng = end_lng;
		this->grid[i][j].end_lat = end_lat;

		pos = 6;
		len = grid_data.size();
		while(pos < len)
		{
			seg_id = atoi(grid_data[pos].c_str());
			if(seg_id <= 0 || seg_id > segs_size) {
				debug_msg("seg_id invlid, seg_id=%d,segs_size=%d.\n", seg_id, segs_size);
				pos++;
				continue;
			}
			seg = this->segs[seg_id - 1];
			this->grid[i][j].node_segs.push_back(seg);
			pos++;
		}
	}

	fclose(fp);
	debug_msg("[INFO]loading grid ok ... ...\n");
}

void MapIndex::initGrid()
{
	int i, j;

	grid = new grid_node*[lng_num];
	for(i = 0; i < lng_num; i++)
	{
		grid[i] = new grid_node[lat_num];
	}
	for(i = 0; i < lng_num; i++)
	{
		for(j = 0; j < lat_num; j++)
		{
			grid[i][j].node_segs.clear();
		}
	}
}

