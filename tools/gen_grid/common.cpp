#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>

using namespace std;

#include "common.h"

vector<string> SplitBySep(string str, string sep)
{
	size_t pos, old_pos=0;
	string t_str;
	vector<string> result;
	result.clear();

	if(str.empty())
		return result;

	if(sep.empty()) {
		result.push_back(str);
		return result;
	}

	cout << "sep:" << sep << endl;
	pos = str.find(sep, 0);

	while(pos != string::npos)
	{
		cout << "old_pos:" << old_pos << "\tpos:" << pos << endl;
		t_str = str.substr(old_pos, pos-old_pos);
		cout << "t_str:" << t_str << endl;
		result.push_back(t_str);
		old_pos = pos;
		pos = str.find(sep, old_pos);
		exit(0);
	}

	t_str = str.substr(pos);
	result.push_back(t_str);

	return result;
}

void EchoRunning()
{
	static int i = 0;

	int j = i % 4;

	switch(j)
	{
		case 0:
			printf("\b\b\b---");
			break;
		case 1:
			printf("\b\b\b\\\\\\");
			break;
		case 2:
			printf("\b\b\b|||");
			break;
		case 3:
			printf("\b\b\b///");
			break;
	}
}

