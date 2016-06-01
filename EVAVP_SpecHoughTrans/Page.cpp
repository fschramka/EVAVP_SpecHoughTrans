#include <cmath>
#include "Page.h"

Page::Page(SDL_Renderer* renderer, int width, int height, double scale) {
	m_renderer    = renderer;
	m_width       = width  < 0 ? 1024           : width;
	m_height      = (int)(height < 0 ? m_width * 1.41 : height);
	m_texture     = NULL;
	m_rotation    = 0;
	m_scale_x     = scale;
	m_scale_y     = scale;
	m_position.x  = 0;
	m_position.y  = 0;

	m_background.r = 0;
	m_background.g = 0;
	m_background.b = 0;
	m_background.a = 0;

	m_visible      = true;
}

void Page::clear() {}

void Page::setPosition(int x, int y) {
	m_position.x = x;
	m_position.y = y;
}

void Page::setScale(double scale) {
	m_scale_x = m_scale_y = scale;
}

void Page::setScale(int width, int height) {
	m_scale_x  = width;
	m_scale_x /= m_width;
	m_scale_y  = height;
	m_scale_y /= m_height;
}

void Page::setRotation(double degrees) {
	m_rotation = degrees;
}

void Page::setVisible(bool visible) {
	m_visible = visible;
}

void Page::render(SDL_Renderer* renderer) {
	if(!(m_visible)) return;
	int       w    = (int)(m_width  * m_scale_x);
	int       h    = (int)(m_height * m_scale_y);
	SDL_Rect  dstr = {m_position.x-w/2,m_position.y-h/2,w,h};
	SDL_Point c    = {w/2,h/2};
	SDL_RenderCopyEx(renderer, getTexture(), NULL, &dstr, m_rotation, &c, SDL_FLIP_NONE);
}

bool Page::project(int x, int y, SDL_Point* result) {
	double dx = x;
	double dy = y;
	dx -= m_position.x;
	dy -= m_position.y;
	double a  = atan2(dx,dy) + (M_PI*m_rotation/180);
	double l  = sqrt(dx*dx+dy*dy);
	dx = sin(a) * (l/m_scale_x);
	dy = cos(a) * (l/m_scale_y);
	dx += m_width  / 2;
	dy += m_height / 2;
	result->x = (int)dx;
	result->y = (int)dy;
	return dx >= 0 && dy >= 0 && dx < m_width && dy < m_height;
}

SDL_Texture* Page::getTexture() {
	return m_texture;
}

void Page::setBackground(SDL_Color color) {
	m_background = color;
	clear();
}

Page::~Page(void) {}
