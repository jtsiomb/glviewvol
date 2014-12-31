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
#ifndef REND_FAST_H_
#define REND_FAST_H_

#include "renderer.h"

class RendererFast : public Renderer {
private:
	unsigned int vol_tex, xfer_tex;
	bool vol_tex_valid, xfer_tex_valid;

	int proxy_count, vbo_proxy_count;
	unsigned int vbo;

public:
	RendererFast();

	bool init();
	void destroy();

	void set_volume(Volume *vol);

	void set_proxy_count(int n);
	int get_proxy_count() const;

	void update(unsigned int msec);
	void render() const;
};

#endif	// REND_FAST_H_
