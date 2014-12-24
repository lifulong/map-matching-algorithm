/**
 * impletation of robust map match algorithm.
 *
 * @date: 2014-12-21
 * @author: lifulong
 **/

#include <iostream>

using namespace std;

#define NUM_ROADS_THRESHOLD	5
#define MinNumOfMatchesThreshold 5

struct point {
	double	lat;
	double	lng;
};


struct trajectory {
	struct	point;
	int		timestamp;
};


struct edge {
	struct	start_point;
	struct	end_point;
};


struct road {
};

struct match {
	struct	point	raw_point;
	int		timestamp;
	struct	point	map_point;
	struct edge		edge;
};


/**
 *
 *
 *
 */
ProcessTransitions(vector<struct road> roads, struct match match, vector<match> prev)
{
	vector<match> match_set;

	//match_set = {m | m -> prev && m connected to match}

	if(match_set.length() < MinNumOfMatchesThreshold)
		match_set = prev;

	order(match_set) by desc cost(m)
	
	for(vector<match>::iterator iter = match_set.start(); iter < match_set.end(); iter++)
	{
		if(cost(iter->second) > cost(match))
			break;
		tc = cost(iter->second) + TransitionsCost(iter->second, match);
		if(tc < cost(match))
		{
			cost(match) = tc;
			list(match) = l(iter->second) U [iter->second]
		}
	}
}


/**
 *
 *
 *
 */
vector<match> MapMatcher(vector<struct road> roads, vector<struct trajectory> trajectorys)
{
	int i, j, points_num;
	double distance;
	double cost[points_num][NUM_ROADS_THRESHOLD];
	struct point map_point;
	vector<match> list;
	vector<match> prev, next;
	vector<edge> nearby;

	list.clear();
	prev.clear();
	next.clear();
	points_num = trajectorys.length();

	for(i = 0; i < points_num; i++)
	{
		struct point current_point = trajectorys[i];
		nearby.clear();
		while(nearby.length() < NUM_ROADS_THRESHOLD)
		{
			nearby.push_back();
			distance *= 2;
		}

		next.clear();

		for(j = 1; j < nearby.length(); j++)
		{
			current_edge = nearby[j];
			map_point = projection(current_point, current_edge);
			struct match match = {};
			next.push_back(match);
			if(i > 1)
			{
				cost[i][j] = MAX_VALUE;
				ProcessTransition(roads, match, prev);
			}
			else
			{
				cost[i][j] = 0;
				list = {};
			}
		}

		prev = next;
	}


	return list;
}




