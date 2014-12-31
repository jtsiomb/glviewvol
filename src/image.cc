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
#include <string.h>
#include <imago2.h>
#include "image.h"

Image::Image()
{
	width = height = 0;
	pixels = 0;
}

void Image::destroy()
{
	delete [] pixels;
}

bool Image::set_pixels(int x, int y, float *data)
{
	float *new_pixels;
	try {
		new_pixels = new float[x * y];
	}
	catch(...) {
		return false;
	}

	width = x;
	height = y;

	delete [] pixels;
	pixels = new_pixels;

	if(data) {
		memcpy(pixels, data, x * y * sizeof *data);
	}
	return true;
}

bool Image::load(const char *fname)
{
	// TODO dicom loader ... ?

	int x, y;
	float *pixels = (float*)img_load_pixels(fname, &x, &y, IMG_FMT_GREYF);
	if(!pixels) {
		return false;
	}
	bool res = set_pixels(x, y, pixels);
	img_free_pixels(pixels);
	return res;
}
