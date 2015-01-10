#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <iostream>
#include <sstream>
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

	pos = str.find(sep, 0);

	while(pos != string::npos)
	{
		t_str = str.substr(old_pos, pos-old_pos);
		result.push_back(t_str);
		old_pos = pos+1;
		pos = str.find(sep, old_pos);
	}

	t_str = str.substr(old_pos);
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

double String2Double(string str)
{
	size_t i, len;
	size_t first_pos, last_pos;

	if("" == str)
		return 0.0;

	first_pos = str.find_first_of("\"");
	last_pos = str.find_last_of("\"");

	if(string::npos != first_pos && string::npos != last_pos && first_pos != last_pos)
		str = str.substr(first_pos+1, last_pos-1);

	len = str.length();
	for(i = 0; i < len; i++)
	{
		if(!isdigit(str[i]) && '.' != str[i])
			return 0.0;
	}

	return strtold(str.c_str(), NULL);
}


string Double2String(double val)
{
	stringstream ss;
	string str;

	ss << val;
	str = ss.str();

	return str;
}

double Min(double val1, double val2)
{
	if(val1 <= val2)
		return val1;
	return val2;
}

#define DEVIATION	1.E-10
bool isEqualDouble(double val1, double val2)
{
	if(fabs(val1-val2) <= DEVIATION)
		return true;
	return false;
}

