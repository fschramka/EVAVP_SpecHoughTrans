#include "PaperFinder.h"

unsigned char* databuffer;
int w, h;
const int eps = 3;

PaperFinder::PaperFinder(){
	databuffer = nullptr;
	w = 0;
	h = 0;
	m_a = -1;
	m_ignore = 1000;
}

PaperFinder::PaperFinder(int width, int height){
	databuffer = nullptr;
	w = width;
	h = height;
	m_a = -1;
	m_ignore = 1000;
}

PaperFinder::PaperFinder(unsigned char* data, int width, int height){
	databuffer = data;
	w = width;
	h = height;
	m_a = -1;
	m_ignore = 1000;
}

vector<Point> PaperFinder::findPaper(int nX, int nY){

	int xScale = w / (nX+1);
	int yScale = h / (nY+1);

	// epsilon for slope calculation
	int eps = 1;
	// slope
	int slope = 0;
	// treshhold
	int tresh = 60;
	// found deltas
	vector<Point> reportedPoints = vector<Point>();
	int offset = 4; // border offset in multiples of eps

	if(nY != 0){
		// horizontal scan
		for(int i = 1; i<=nY; ++i){
			for(int j = offset*eps; j<w-offset*eps; j+=eps){
				slope = (int)databuffer[yScale*i*w+(j-eps)] - (int)databuffer[yScale*i*w+(j+eps)];
				if(slope > tresh || slope < tresh*(-1))
					reportedPoints.push_back(createPoint(yScale*i*w+j, false));
			}
		}
	}

	if(nX != 0){
		//vertical scan
		for(int i = 1; i<=nX; ++i){
			for(int j = offset*eps; j<h-offset*eps; j+=eps){
				slope = (int)databuffer[xScale*i+w*(j-eps)] - (int)databuffer[xScale*i+w*(j+eps)];
				if(slope > tresh || slope < tresh*(-1))
					reportedPoints.push_back(createPoint(xScale*i+w*j, true));
			}
		}
	}
	return reportedPoints;
}

// this is very innefficient (Point copies, O(n^3), ...) improve!
vector<Point> PaperFinder::getRectangle(vector<pair<Point, Point>> lines){

	Point currVec;
	Point tmpVec;

	vector<double> xs;
	vector<double> ys;
	vector<double> cs;

	for(int i = 0; i<lines.size(); ++i){
		currVec = getVectorNormalized(lines.at(i),1000);
		for(int j = i+1; j<lines.size(); ++j){
			tmpVec = getVectorNormalized(lines.at(j),1000);

			// check if orthogonal
			//if(90 - abs(dotAngle(currVec, tmpVec)) < eps) {
			int s = abs(currVec.getX()*tmpVec.getX()+currVec.getY()*tmpVec.getY()); // scalar product of normalized vectors 
			if(s > 0 && s< 10000) { // some values that work (experimental)
				Point p = cutPoint(lines.at(i).first, lines.at(i).second, lines.at(j).first, lines.at(j).second);
				if(p.getX() == 0 && p.getY() == 0) continue;

				bool merged = false;
				double x = p.getX();
				double y = p.getY();

				for(int k = 0; k < xs.size(); k++) {
					double dx = x - xs[k]; 
					double dy = y - ys[k];
					// if distance between points is below threshold, merge them
					if(dx*dx+dy*dy<200) {
						double w  = ++cs[k]; // increment merge count for wieghted sum
						double w0 = 1 / w;
						double w1 = 1 - w0;
						// new point is weghted sum of w1*oldPoint+w0*newPoint
						xs[k] = xs[k] * w1 + w0 * x;
						ys[k] = ys[k] * w1 + w0 * y;
						merged = true;
						break;
					}
				}

				// if no points were merged, add them to the list
				if(!(merged)) {
					xs.push_back(x);
					ys.push_back(y);
					cs.push_back(1);
				}
			}
		}
	}

	// create result vector
	vector<Point> ret;
	for(int i = 0; i < xs.size(); i++)
		ret.push_back(Point((int)xs[i],(int)ys[i]));
	return ret;
}

static double normalize( const double value, const double start, const double end )  {
  const double width       = end   - start;
  const double offsetValue = value - start;
  return ( offsetValue - ( floor( offsetValue / width ) * width ) ) + start ;
}

static double deltaAngle(double a, double b) {
	double d = normalize(a-b, 0, 360);
    return d > 180 ? 360 - d : d;
}

static Point distanceAngleCenter(Point& p0, Point& p1, double* d, double* angle) {
	double dx = p0.getX() - p1.getX();
	double dy = p0.getY() - p1.getY();
	*angle = normalize((atan2(dx, -dy) * 180) / M_PI - 90, 0, 360);
	*d     = sqrt(dx*dx+dy*dy);
	return Point((p0.getX()+p1.getX())/2,(p0.getY()+p1.getY())/2);
}

void PaperFinder::setPage(vector<Point> points, Page* page) {
	// heuristic: simply take the first three points, boldly assume they are corners of a page and set the transformation accordingly.
	// just for demo, fix it!
	if(points.size() > 2) {
		Point  c[3];
		double a[3];
		double d[3];
		c[0] = distanceAngleCenter(points[0],points[1], &d[0], &a[0]);		
		c[1] = distanceAngleCenter(points[1],points[2], &d[1], &a[1]);		
		c[2] = distanceAngleCenter(points[2],points[0], &d[2], &a[2]);

		// find min, max and mid side of triangle
		int min = 0; double dmin = d[0];
		int max = 0; double dmax = d[0];
		for(int i = 1; i < 3; i++) {
			if(d[i] < dmin) {dmin = d[i];min=i;}
			if(d[i] > dmax) {dmax = d[i];max=i;}
		}
		int mid = 3 - (max+min);

		// filter aspect ratio 
		double aspect = d[mid] / d[min];
		if(aspect < 1.3 || aspect > 1.5) {
			m_ignore++; 
			m_a = -1;
			goto done;
		}

		m_ignore = 0;
		page->setVisible(true);

		// avoid flipping by 180°
		if(deltaAngle(a[min], m_last_a) > 90) {
			if(deltaAngle(a[min]+180, m_last_a) < 90)
				a[min] = normalize(a[min] + 180, 0, 360);
		}
		m_last_a = a[min];

		// smooth the values
		double w0 = m_a < 0 ? 0 : 0.3;
		double w1 = 1-w0;
		m_x = m_x * w0 + c[max].getX() * w1;
		m_y = m_y * w0 + c[max].getY() * w1;
		m_w = m_w * w0 + d[min]        * w1;
		m_h = m_h * w0 + d[mid]        * w1;
		m_a = m_a * w0 + a[min]        * w1;

		page->setScale((int)m_w, (int)m_h);
		page->setRotation(m_a);
		page->setPosition((int)m_x, (int)m_y);
	} else {
		m_ignore++; 
		m_a = -1;
	}
done:
	page->setVisible(m_ignore < 40);
}

vector<Point> PaperFinder::filterPoints(vector<Point> points){

	vector<Point> ret;
	int d;

	/*for(int i = 0; i<points.size(); ++i){
		for(int j = i; j<points.size(); ++j){
			
			
		}
	}*/
	return points;
}

Point PaperFinder::cutPoint(Point a, Point b, Point c, Point d){

	int dis = (a.getX()-b.getX())*(c.getY()-d.getY()) - (a.getY()-b.getY())*(c.getX()-d.getX());
    if(dis == 0)
		return Point();
    
	int xi = ((c.getX()-d.getX())*(a.getX()*b.getY()-a.getY()*b.getX())-(a.getX()-b.getX())*(c.getX()*d.getY()-c.getY()*d.getX()))/dis;
    int yi = ((c.getY()-d.getY())*(a.getX()*b.getY()-a.getY()*b.getX())-(a.getY()-b.getY())*(c.getX()*d.getY()-c.getY()*d.getX()))/dis;
    
    return Point(xi,yi);
}

int PaperFinder::dotAngle(Point vec1, Point vec2){
	return (int)(acos((vec1.getX()*vec2.getX() + vec1.getY()*vec2.getY()) / (sqrt(vec1.getX()*vec1.getX() + vec1.getY() * vec1.getY()) * sqrt(vec2.getX()*vec2.getX() + vec2.getY() * vec2.getY()))) / M_PI * 180.0);
}

Point PaperFinder::getVectorNormalized(pair<Point, Point> line, double scale) {
	double x  = line.second.getX() - line.first.getX();
	double y  = line.second.getY() - line.first.getY();
	double l  = sqrt(x*x+y*y);

	return Point((scale*x)/l,(scale*y)/l);
}

void PaperFinder::setBuffer(unsigned char* b){
	databuffer = b;
}

void PaperFinder::setDimensions(int width, int height){
	w = width;
	h = height;
}


Point PaperFinder::createPoint(int pos, bool vertical){
	if(pos >= w*h)
		throw exception("pos out of bound");

	return Point(pos%w,pos/w, pos, vertical);
}

int PaperFinder::getWidth(){
	return w;
}

int PaperFinder::getHeight(){
	return h;
}
