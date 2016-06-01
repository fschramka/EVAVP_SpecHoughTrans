#pragma once

#include <iostream>

class Point{

private:

	int m_x, m_y;
	int m_orig;
	bool m_vert;

public:

	Point(){
		m_y = 0;
		m_x = 0;
	}

	Point(int x, int y){
		m_x = x;
		m_y = y;
		m_orig = 0;
	}

	Point(int x, int y, int orig, bool vertical){
		m_x = x;
		m_y = y;
		m_orig = orig;
		m_vert = vertical;
	}

	int getX() const{
		return m_x;
	}

	int getY() const{
		return m_y;
	}

	Point getCopy(){
		return Point(m_x, m_y);
	}

	int getOrigVal() const{
		return m_orig;
	}

	bool isVertical(){
		return m_vert;
	}

};

	inline std::ostream& operator<<(std::ostream& str, const Point& p) { 
		str << "(" << p.getX() << ":" << p.getY() << ")" << " val: " << p.getOrigVal();
		return str;
	}