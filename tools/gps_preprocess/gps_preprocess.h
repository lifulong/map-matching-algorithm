#include <stdio.h>
#include <time.h>

#include <iostream>
#include <vector>

using namespace std;

#include "../../json/json.h"

//accept c_str as input parameters.
#define debug_2_console(fmt,args...)	\
	do{									\
		time_t t_time;					\
		time(&t_time);					\
		printf("%s", ctime(&t_time));	\
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



class GpsPreprocess {

	public:

		GpsPreprocess(string data_file="");
		int getErrno();
		string getErrMsg();

		void loadJsonGps(string gps_file);
		void dumpJsonGps(string dump_file="");
		void dumpJsonGpsBeauty(string dump_file="");
		void appendLongSpeed();
		void appendShortSpeed();
		void appendLineHeading();

	private:

		static constexpr double DEF_PI = 3.14159265359; // PI
		static constexpr double DEF_2PI = 6.28318530712; // 2*PI
		static constexpr double DEF_PI180 = 0.01745329252; // PI/180.0
		static constexpr double DEF_R = 6370693.5; // radius of earth

		json js;
		FILE *_logger;
		int errno;
		string errmsg;

		void init(string data_file);
		bool isEqualDouble(double val1, double val2);
		double calLineDistance(double start_lng, double start_lat, double end_lng, double end_lat);
		double calShortDistance(double lon1, double lat1, double lon2, double lat2);
		double calLongDistance(double lon1, double lat1, double lon2, double lat2);
		double calShortSpeed(double lon1, double lat1, double lon2, double lat2, double last_time, double time);
		double calLongSpeed(double lon1, double lat1, double lon2, double lat2, double last_time, double time);
		double calLineHeading(double lon1, double lat1, double lon2, double lat2);
};


