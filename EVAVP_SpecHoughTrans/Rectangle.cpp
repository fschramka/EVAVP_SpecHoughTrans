#include "Rectangle.h"

vector<Point> corners = vector<Point>(4);

Rectangle::Rectangle(){ }

Rectangle::Rectangle(Point topL, Point topR, Point botR, Point botL){
	corners[0] = topL;
	corners[1] = topR;
	corners[2] = botR;
	corners[3] = botL;
}

Rectangle::Rectangle(vector<Point> in){
	corners = in;
}

vector<Point> Rectangle::getCorners(){
	return vector<Point>(corners);
}