#define _USE_MATH_DEFINES
#include <math.h>
#include "HoughTransform.h"

vector<Point> points;
unsigned char* dataptr;
const int teta = 180;
const int slots = 25;
int cols, rows;
int max_d;

HoughTransform::HoughTransform(int x, int y){
	if((y%teta) == 0) y -= 3;
	if((x%teta) == 0) x -= 3;
	cols = x;
	rows = y;
	max_d = (int)(sqrt(x*x + y*y)*2);
	dataptr = new unsigned char[teta*max_d];
}

HoughTransform::HoughTransform(vector<Point> input, int x, int y){
	//input
	points = input;
	//houghRoom
	HoughTransform(x, y);
}

HoughTransform::~HoughTransform(){
	delete(dataptr);
	dataptr = nullptr;
}

bool compare(Point a, Point b)
{
	return (a.getOrigVal()<b.getOrigVal());
}

vector<pair<Point,Point>> HoughTransform::transform(){
	 //reset houghRoom
	memset(dataptr, 0, teta*max_d*sizeof(unsigned char));

	int p;
	set<int> index;

	// draw houghRoom
	for(Point in: points){
		for(int i = 0 ; i<teta ; ++i){
			p = (int)(in.getX() * cos(i/180.0*M_PI) + in.getY() * sin(i/180.0*M_PI));
			
			// overflow handling
			if(dataptr[(teta*p + i)+(teta*max_d/2)] < 255-4){
				if (dataptr[(teta*p + i) + (teta*max_d / 2)] > 30) {
					index.insert((teta*p + i) + (teta*max_d / 2));
				}
				dataptr[(teta*p + i)+(teta*max_d/2)] +=4;
			}
		}
	}

	// find peaks
	vector<pair<Point, Point>> peaks;
	priority_queue<Point, vector<Point>, decltype(&compare)> peakQueue(&compare);

	for(int i : index){
		if(dataptr[i] > 0)	{
			peakQueue.push(Point(i%teta, (i - (teta*max_d/2) -i%teta) / teta , dataptr[i], false));
		}
	}

	// compute lines
	Point tmpP;
	int queueSize = peakQueue.size();

	for(int i = 0; i < min(slots, queueSize) ; ++i){
		peaks.push_back(houghPointToEuklidLine(peakQueue.top()));
		peakQueue.pop();
	}

	return peaks;
}

int HoughTransform::getX(Point houghPoint, int y){
	return (int)((houghPoint.getY() - y * sin(houghPoint.getX() /180.0 *M_PI)) / (cos(houghPoint.getX() / 180.0 * M_PI)));
}

int HoughTransform::getY(Point houghPoint, int x){
	return (int)((houghPoint.getY() - x * cos(houghPoint.getX() /180.0 *M_PI)) / (sin(houghPoint.getX() / 180.0 * M_PI)));
}

pair<Point, Point> HoughTransform::houghPointToEuklidLine(Point houghPoint){
	int tmpIfMin = getX(houghPoint, 0);
	int tmpIfMax = getX(houghPoint, rows);

	// overflow case &&  WE || EW
	if(tmpIfMin == INT_MIN || tmpIfMin == INT_MAX || tmpIfMin < 0 && tmpIfMax > cols){
		tmpIfMin = getY(houghPoint, 0);
		tmpIfMax = getY(houghPoint, cols);

		return pair<Point, Point>(Point(0, tmpIfMin), Point(cols, tmpIfMax));
	}

	// case WS
	if(tmpIfMin < 0){
		tmpIfMin = getY(houghPoint, 0);
		return pair<Point, Point>(Point(0, tmpIfMin), Point(tmpIfMax, rows));
	}
	// case ES
	else if(tmpIfMin > cols){
		tmpIfMin = getY(houghPoint, cols);
		return pair<Point, Point>(Point(cols, tmpIfMin), Point(tmpIfMax, rows));
	}
	// case NW
	else if(tmpIfMax < 0){
		tmpIfMax = getY(houghPoint, 0);
		return pair<Point, Point>(Point(tmpIfMin, 0), Point(0, tmpIfMax));
	}
	// case NE
	else if(tmpIfMax > cols){
		tmpIfMax = getY(houghPoint, cols);
		return pair<Point, Point>(Point(tmpIfMin, 0), Point(cols, tmpIfMax));
	}

	return pair<Point, Point>(Point(tmpIfMin, 0), Point(tmpIfMax, rows));
}

void HoughTransform::setInput(vector<Point> in){
	points = in;
}

unsigned char* HoughTransform::getData(){
	return dataptr;
}

int HoughTransform::getHoughHeight()
{
	return max_d;
}
