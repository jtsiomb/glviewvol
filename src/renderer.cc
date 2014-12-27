#include "renderer.h"

Renderer::Renderer()
{
	vol = 0;
	view_width = 512;
	view_height = 512;
}

Renderer::~Renderer()
{
	destroy();
}

bool Renderer::init()
{
	return true;
}

void Renderer::destroy()
{
}

void Renderer::set_volume(Volume *vol)
{
	this->vol = vol;
}

Volume *Renderer::get_volume() const
{
	return vol;
}

void Renderer::reshape(int x, int y)
{
	view_width = x;
	view_height = y;
}

void Renderer::update(unsigned int msec)
{
}
