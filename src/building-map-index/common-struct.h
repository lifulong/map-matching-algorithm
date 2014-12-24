/**
 * impletation of build grid index of road network for map matching
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include <iostream>
#include <vector>

using namespace std;


struct point {
	double	lat;
	double	lng;
};

struct edge {
	struct point	start_point;
	struct point	end_point;
};

struct road {
	vector<struct edge>		edges;
};


struct inode {
	int				node_index;
	struct point	point;
};

struct iedge {
	int		edge_index;
	int		start_vnode_index;
	int		end_vnode_index;
};

struct igeo {
	int				edge_index;
	vector<struct point>	points;
};


