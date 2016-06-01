#pragma once

#include "Point.h"
#include <vector>

using namespace std;

class Rectangle{

public:

	Rectangle();
	Rectangle(Point topL, Point topR, Point botR, Point botL);
	Rectangle(vector<Point> in);

	vector<Point> getCorners();
};