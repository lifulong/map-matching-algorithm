
#include <iostream>
#include <cmath>

using namespace std;

class GenDistanceTable
{
	public:
		GenDistanceTable()
		{
		}

		double genLngDistance(double lng1, double lng2)
		{
			return 0;
		}

		double genLatDistance(double lat1, double lat2)
		{
			return 0;
		}

		double test()
		{
			double mLat1 = 39.90923; // point1纬度
			double mLon1 = 116.357428; // point1经度
			double mLat2 = 39.90923;// point2纬度
			double mLon2 = 116.397428;// point2经度
			double distance = GetShortDistance(mLon1, mLat1, mLon2, mLat2);
			cout << "LongDistance:\t" << distance << endl;
		}

	private:
		static const double DEF_PI = 3.14159265359; // PI
		static const double DEF_2PI = 6.28318530712; // 2*PI
		static const double DEF_PI180 = 0.01745329252; // PI/180.0
		static const double DEF_R = 6370693.5; // radius of earth

		double GetShortDistance(double lon1, double lat1, double lon2, double lat2)
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

		double GetLongDistance(double lon1, double lat1, double lon2, double lat2)
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

