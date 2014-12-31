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
#ifndef RENDERER_H_
#define RENDERER_H_

#include "volume.h"
#include "xfermap.h"

#define MAX_CLIP_PLANES		4

class Renderer {
protected:
	int view_width, view_height;
	Volume *vol;

	float clip_plane[MAX_CLIP_PLANES][4];	// nx,ny,nz,dist

	TransferFunc *xfer;
	float zscale;

public:
	Renderer();
	virtual ~Renderer();

	virtual bool init();
	virtual void destroy();

	virtual void set_volume(Volume *vol);
	virtual Volume *get_volume() const;

	virtual void set_zscale(float zs);
	virtual float get_zscale() const;

	virtual void set_transfer_function(TransferFunc *xfer);
	virtual TransferFunc *get_transfer_function() const;

	virtual void set_clipping_plane(int idx, float nx, float ny, float nz, float dist);
	virtual void disable_clipping_plane(int idx);

	virtual void reshape(int x, int y);

	virtual void update(unsigned int msec);
	virtual void render() const = 0;
};

#endif	// RENDERER_H_
