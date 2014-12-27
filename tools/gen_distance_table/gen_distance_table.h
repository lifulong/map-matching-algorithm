
#include <iostream>
#include <cmath>

using namespace std;

#include <stdio.h>

class GenDistanceTable
{
	public:
		GenDistanceTable()
		{
		}

		/**
		 * 在一定的纬度内，固定纬度，计算一定经度间隔的的距离。
		 *
		 * @author:lifulong
		 */
		void genLngDistance(double start_lat, double end_lat, double lat_gap, string file_name = "")
		{
			int i;
			FILE *fp = NULL;
			double current_lat = start_lat, distance;
			double gaps_array[] = {0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1};
			int gaps_array_len = sizeof(gaps_array) / sizeof(gaps_array[0]);

			if(start_lat > end_lat || lat_gap <= 0)
				return;

			if(file_name == "")
				fp = stdout;
			else {
				fp = fopen(file_name.c_str(), "w");
				if(NULL == fp)
				{
					cout << "[Error]Open " + file_name + "Error.\n" << endl;
					return;
				}
			}

			fprintf(fp, "lat\t");
			for(i = 0; i < gaps_array_len; i++)
				fprintf(fp, "%0.4f\t", gaps_array[i]);
			fprintf(fp, "\n");

			while(current_lat <= end_lat)
			{
				fprintf(fp, "%0.4f\t", current_lat);
				for(i = 0; i < gaps_array_len; i++)
				{
					distance = this->getLongDistance(0, current_lat, gaps_array[i], current_lat);
					fprintf(fp, "%0.4f\t", distance);
				}
				current_lat += lat_gap;
				fprintf(fp, "\n");
			}

			if(stdout != fp)
				fclose(fp);

			return;
		}

		/**
		 * 固定经度，在某个纬度基数上，计算一定纬度间隔的距离。
		 *
		 *
		 */
		void genLatDistance(double lng, double lat, string file_name="")
		{
			int i;
			FILE *fp = NULL;
			double current_lat = lat, distance;
			double gaps_array[] = {0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1};
			int gaps_array_len = sizeof(gaps_array) / sizeof(gaps_array[0]);

			if(file_name == "")
				fp = stdout;
			else {
				fp = fopen(file_name.c_str(), "w");
				if(NULL == fp)
				{
					cout << "[Error]Open " + file_name + "Error.\n" << endl;
					return;
				}
			}

			fprintf(fp, "lat\t");
			for(i = 0; i < gaps_array_len; i++)
				fprintf(fp, "%0.4f\t", gaps_array[i]);
			fprintf(fp, "\n");

			fprintf(fp, "%0.4f\t", current_lat);
			for(i = 0; i < gaps_array_len; i++)
			{
				distance = this->getLongDistance(lng, current_lat, lng, current_lat+gaps_array[i]);
				fprintf(fp, "%0.4f\t", distance);
			}
			fprintf(fp, "\n");

			if(stdout != fp)
				fclose(fp);

			return;
		}

	private:
		static const double DEF_PI = 3.14159265359; // PI
		static const double DEF_2PI = 6.28318530712; // 2*PI
		static const double DEF_PI180 = 0.01745329252; // PI/180.0
		static const double DEF_R = 6370693.5; // radius of earth

		double getShortDistance(double lon1, double lat1, double lon2, double lat2)
		{
				double ew1, ns1, ew2, ns2;
				double dx, dy, dew;
				double distance;
				// 角度转换为弧度
				ew1 = lon1 * DEF_PI180;
				ns1 = lat1 * DEF_PI180;
				ew2 = lon2 * DEF_PI180;
				ns2 = lat2 * DEF_PI180;
				// 经度差
				dew = ew1 - ew2;
				// 若跨东经和西经180 度，进行调整
				if (dew > DEF_PI)
					dew = DEF_2PI - dew;
				else if (dew < -DEF_PI)
					dew = DEF_2PI + dew;
				dx = DEF_R * cos(ns1) * dew; // 东西方向长度(在纬度圈上的投影长度)
				dy = DEF_R * (ns1 - ns2); // 南北方向长度(在经度圈上的投影长度)
				// 勾股定理求斜边长
				distance = sqrt(dx * dx + dy * dy);
				return distance;
		}

		double getLongDistance(double lon1, double lat1, double lon2, double lat2)
		{
				double ew1, ns1, ew2, ns2;
				double distance;
				// 角度转换为弧度
				ew1 = lon1 * DEF_PI180;
				ns1 = lat1 * DEF_PI180;
				ew2 = lon2 * DEF_PI180;
				ns2 = lat2 * DEF_PI180;
				// 求大圆劣弧与球心所夹的角(弧度)
				distance = sin(ns1) * sin(ns2) + cos(ns1) * cos(ns2) * cos(ew1 - ew2);
				// 调整到[-1..1]范围内，避免溢出
				if (distance > 1.0)
					distance = 1.0;
				else if (distance < -1.0)
					distance = -1.0;
				// 求大圆劣弧长度
					distance = DEF_R * acos(distance);
				return distance;
		}
};

