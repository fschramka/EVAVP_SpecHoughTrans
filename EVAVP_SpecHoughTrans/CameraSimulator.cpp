#include "CameraSimulator.h"


CameraSimulator::CameraSimulator(int width, int height) {
	m_image     = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);

	SDL_Color colors[256];
	for(int i = 0; i < 256; i++)
		colors[i].r = colors[i].g = colors[i].b = (Uint8)i;

	SDL_SetPaletteColors(m_image->format->palette, colors, 0, 256);

	m_count     = 0;
}


unsigned char* CameraSimulator::getSingleImage(bool /*save*/, const char* /*imageName*/) {

	SDL_FillRect(m_image, NULL, 0);

	double t = m_count; t /= 180 * M_PI;
	double x = m_image->w / 2;
	double y = m_image->h / 2;
	double a = (M_PI * 35.3) / 180;
	double s = m_image->h / 3;

	int points[8] = {
		(int)(x+s*sin(t+(a))),       (int)(y+s*cos(t+(a))),
		(int)(x+s*sin(t+(M_PI-a))),  (int)(y+s*cos(t+(M_PI-a))),
		(int)(x+s*sin(t+(M_PI+a))),  (int)(y+s*cos(t+(M_PI+a))),
		(int)(x+s*sin(t+(2*M_PI-a))),(int)(y+s*cos(t+(2*M_PI-a))),
	};

	SDL_LockSurface(m_image);
	drawLine(points[0], points[1], points[2], points[3], 0xff);
	drawLine(points[2], points[3], points[4], points[5], 0xff);
	drawLine(points[4], points[5], points[6], points[7], 0xff);
	drawLine(points[6], points[7], points[0], points[1], 0xff);
	SDL_UnlockSurface(m_image);

	m_count++;

	return (unsigned char*)m_image->pixels;
}

CameraSimulator::~CameraSimulator(void) {
	if(m_image) {
		SDL_FreeSurface(m_image);
		m_image = NULL;
	}
}

void CameraSimulator::drawLine(int x0, int y0, int x1, int y1, Uint8 value) {
	int xinc, yinc;
    Uint8* buffer = (Uint8*)m_image->pixels;
    int xspan = x1-x0+1;
    int yspan = y1-y0+1;
    if (xspan < 0) {
		xinc = -1;
		xspan = -xspan;
    } else xinc = 1;

    if (yspan < 0) {
		yinc = -m_image->w;
		yspan = -yspan;
    } else yinc = m_image->w;
	
    int i = 0;
    int sum = 0;
    int drawpos = m_image->w * y0 + x0;
    if (xspan < yspan) {
		for (i = 0; i < yspan; i++) {
			buffer[drawpos] = value;
			sum += xspan;
			if (sum >= yspan) {
				drawpos += xinc;
				sum     -= yspan;
			}
			drawpos += yinc;
		}
    } else {
		for (i = 0; i < xspan; i++) {
			buffer[drawpos] = value;
			sum += yspan;
			if (sum >= xspan) {
			drawpos += yinc;
			sum -= xspan;
			}
			drawpos += xinc;
		}
    }
}

int CameraSimulator::getCameraResolutionX() {return m_image->w;}
int CameraSimulator::getCameraResolutionY() {return m_image->h;}