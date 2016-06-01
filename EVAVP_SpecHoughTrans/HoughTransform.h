#pragma once

#include "Point.h"
#include <vector>
#include <memory>
#include <queue>
#include <set>
#include <utility>

using namespace std;

class HoughTransform{

private:

	pair<Point, Point> houghPointToEuklidLine(Point houghPoint);
	int getX(Point houghPoint, int y);
	int getY(Point houghPoint, int x);

public:

	HoughTransform(int x, int y);
	HoughTransform(vector<Point> input, int x, int y);
	~HoughTransform();

	vector<pair<Point, Point>> transform();
	void setInput(vector<Point> input);
	unsigned char* getData();
	int getHoughHeight();

};