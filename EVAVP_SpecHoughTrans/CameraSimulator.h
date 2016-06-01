#pragma once

#include <math.h>
#include "SDL.h"

class CameraSimulator {
	SDL_Surface* m_image;
	int          m_count;

public:
	CameraSimulator(int width, int height);

	virtual unsigned char* getSingleImage(bool save, const char* imageName);
	virtual int getCameraResolutionX();
	virtual int getCameraResolutionY();
	virtual void drawLine(int x0, int y0, int x1, int y1, Uint8 value);

	virtual ~CameraSimulator(void);
};

