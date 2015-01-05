/**
 *
 * @date: 2014-12-21
 * @author: lifulong
 */

#include <iostream>
#include <vector>

using namespace std;

#include <string.h>

#define LOG_EMEGE	0x01
#define LOG_ERROR	0x02
#define LOG_WARN	0x03
#define LOG_INFO	0x04

#define WITH_TIME


class Logger {

	public:

		Logger(string file = "", int level = LOG_INFO)
		{
			if(file == "")
			{
				_fp = stdout;
			}
			else
			{
				_fp = fopen(file.c_str(), "w+");
				if(!_fp)
					cout << "Init Logger File " + file + " Error.";
			}
			this->_level = level;
		}

		void Log(string str)
		{
			time_t raw_time;
			struct tm *timeinfo;

			time(&raw_time);
			timeinfo = localtime(&raw_time);
			string timestr = asctime(timeinfo);

#ifdef WITH_TIME
			str = timestr + str + "\n";
#endif
			fwrite(str.c_str(), str.length(), 1, _fp);
		}

		void Info(string str)
		{
			if(this->_level < LOG_INFO)
				return;
			this->Log("[INFO]" + str);
		}

	private:
		//Levels: EMEGE,ERROR,WARN,INFO
		FILE* _fp;
		int _level;

};
