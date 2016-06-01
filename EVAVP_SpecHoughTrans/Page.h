#pragma once

#include "SDL.h"
#include "Point.h"

class Page
{
private:
protected:
	SDL_Texture*  m_texture;
	SDL_Color	  m_background;
	SDL_Renderer* m_renderer;
	int           m_width;
	int           m_height;
	SDL_Point     m_position;
	double        m_scale_x;
	double        m_scale_y;
	double        m_rotation;
	bool          m_visible;

	virtual bool project(int x, int y, SDL_Point* result);
public:
	Page(SDL_Renderer* renderer, int width = -1, int height = -1, double scale = 1);
	virtual void setPosition(int x, int y);
	virtual void setRotation(double degrees);
	virtual void setScale(double scale);
	virtual void setScale(int width, int height);
	virtual void setBackground(SDL_Color color);
	virtual void clear();
	virtual void setVisible(bool visible);
	virtual void render(SDL_Renderer* renderer);
	virtual SDL_Texture* getTexture();
	virtual ~Page(void);
};

