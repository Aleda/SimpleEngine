#include "GMath.h"
#include <cmath>

double GMath::Astar_GetDistance(double sx, double sy, double ex, double ey)
{
	double dx = sx - ex;
	double dy = sy - ey;
	return sqrt(pow(dx,2) + pow(dy,2));
}

double GMath::Astar_GetAngle(double sx, double sy, double ex, double ey)
{
	double dx = ex - sx;
	double dy = ey - sy;
	
	double angle;
	double degree;

	angle = atan(dy / dx);

	degree = RadianToDegree(angle);


	if (dx < 0 && dy >= 0) {
		degree += 180;
		if(fabs(dx) <=0.001f && dy >= 0)
		{
			degree = 270;
		}
	}
	else if (dx <=0 && dy<0) {
		degree += 180;

	}
	else if (dx > 0 && dy <0) {
		degree += 360;

	}
	else if (dx>=0 && dy>=0) {
		if(fabs(dx) <=0.001f && dy >= 0 && fabs(dy)<=0.001f)
		{
			degree = 270;
		}
	}
	return degree;
}

int GMath::Astar_GetDir(double degree) {
  //右下，左下，左上，右上，下，左，上，右
  // 0    1    2    3   4  5   6  7
  //  2 3 0 1       6 7   4 5
  //int innerDirs[8] = { 7,3,6,2,5,1,4,0 };
	int innerDirs[8] = { 7,0,4,1,5,2,6,3};
	if (360 - 22.5 <= degree || degree < 22.5) {
		return innerDirs[0];
	}
	else {
		for (int i = 1; i < 8; i++) {
			if (i * 45 - 22.5 <= degree && degree < i * 45 + 22.5) {
				return innerDirs[i];
			}
		}
		return innerDirs[0];
	}
}


