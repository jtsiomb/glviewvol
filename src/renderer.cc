#include <math.h>
#include "renderer.h"

Renderer::Renderer()
{
	vol = 0;
	view_width = 512;
	view_height = 512;

	for(int i=0; i<3; i++) {
		xfer[i].set_point(0, 0);
		xfer[i].set_point(1, 1);
	}

	xfer_low = 0.0;
	xfer_high = 1.0;

	for(int i=0; i<MAX_CLIP_PLANES; i++) {
		disable_clipping_plane(i);
	}
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

Curve &Renderer::transfer_curve(int color)
{
	return xfer[color];
}

const Curve &Renderer::transfer_curve(int color) const
{
	return xfer[color];
}

void Renderer::set_simple_transfer(float low, float high)
{
	xfer_low = std::min(low, high);
	xfer_high = std::max(low, high);
}

void Renderer::get_simple_transfer(float *low, float *high) const
{
	*low = xfer_low;
	*high = xfer_high;
}

void Renderer::set_clipping_plane(int idx, float nx, float ny, float nz, float dist)
{
	float len = sqrt(nx * nx + ny * ny + nz * nz);
	if(len != 0.0) {
		nx /= len;
		ny /= len;
		nz /= len;
	}
	clip_plane[idx][0] = nx;
	clip_plane[idx][1] = ny;
	clip_plane[idx][2] = nz;
	clip_plane[idx][3] = dist;
}

void Renderer::disable_clipping_plane(int idx)
{
	clip_plane[idx][0] = clip_plane[idx][2] = 0;
	clip_plane[idx][1] = 1;
	clip_plane[idx][3] = -1;
}

void Renderer::reshape(int x, int y)
{
	view_width = x;
	view_height = y;
}

void Renderer::update(unsigned int msec)
{
}
