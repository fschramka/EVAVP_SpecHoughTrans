#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include "Rectangle.h"
#include "Page.h"

using namespace std;

class PaperFinder {
private:
	double m_last_a;
	double m_x;
	double m_y;
	double m_w;
	double m_h;
	double m_a;
	int    m_ignore;

	Point createPoint(int pos, bool vertical);
	Point getVectorNormalized(pair<Point, Point> line, double scale = 1);
	int dotAngle(Point vec1, Point vec2);
	Point cutPoint(Point vec1, Point vec2, Point vec3, Point vec4);

public:
	PaperFinder();
	PaperFinder(int w, int h);
	PaperFinder(unsigned char* data, int w, int h);
	vector<Point> findPaper(int nX, int nY);
	void setBuffer(unsigned char* b);
	void setDimensions(int width, int height);
	int getWidth();
	int getHeight();
	vector<Point> getRectangle(vector<pair<Point, Point>> lines);
	void setPage(vector<Point> corners, Page* page);
	vector<Point> filterPoints(vector<Point> points);
};