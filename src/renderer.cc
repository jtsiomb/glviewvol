/*
glviewvol is an OpenGL 3D volume data viewer
Copyright (C) 2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <math.h>
#include "renderer.h"

Renderer::Renderer()
{
	vol = 0;
	view_width = 512;
	view_height = 512;

	for(int i=0; i<MAX_CLIP_PLANES; i++) {
		disable_clipping_plane(i);
	}

	zscale = 1.0;
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

void Renderer::set_zscale(float zs)
{
	zscale = zs;
}

float Renderer::get_zscale() const
{
	return zscale;
}

void Renderer::set_transfer_function(TransferFunc *xfer)
{
	this->xfer = xfer;
}

TransferFunc *Renderer::get_transfer_function() const
{
	return xfer;
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
