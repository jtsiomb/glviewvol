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
#include <algorithm>
#include "xfermap.h"

TransferFunc::~TransferFunc()
{
}


// ---- TransferWindow ----

TransferWindow::TransferWindow()
{
	soft_rad = 0.5;
	for(int i=0; i<3; i++) {
		low[i] = 0.5;
		high[i] = 1.5;
	}
}

void TransferWindow::set_handle(int channel, int handle, float val)
{
	float *dest = handle == HANDLE_LOW ? low : high;

	if(channel == -1) {
		dest[0] = dest[1] = dest[2] = val;
	} else {
		dest[channel] = val;
	}
}

float TransferWindow::get_handle(int channel, int handle) const
{
	const float *src = handle == HANDLE_LOW ? low : high;

	if(channel == -1) {
		return src[0];	// XXX this doens't make much sense
	}
	return src[channel];
}

int TransferWindow::nearest_handle(int channel, float pos) const
{
	float ldist = 0, hdist = 0;

	if(channel == -1) {
		for(int i=0; i<3; i++) {
			ldist += fabs(low[i] - pos);
			hdist += fabs(high[i] - pos);
		}
	} else {
		ldist = fabs(low[channel] - pos);
		hdist = fabs(high[channel] - pos);
	}
	return ldist <= hdist ? HANDLE_LOW : HANDLE_HIGH;
}

void TransferWindow::set_interval(float a, float b)
{
	float v0 = std::min(a, b);
	float v1 = std::max(a, b);

	for(int i=0; i<3; i++) {
		low[i] = v0;
		high[i] = v1;
	}
}

void TransferWindow::set_interval(float *rgba_low, float *rgba_high)
{
	for(int i=0; i<3; i++) {
		low[i] = std::min(rgba_low[i], rgba_high[i]);
		high[i] = std::max(rgba_low[i], rgba_high[i]);
	}
}

void TransferWindow::set_interval_rgba(int channel, float a, float b)
{
	low[channel] = std::min(a, b);
	high[channel] = std::max(a, b);
}

void TransferWindow::get_interval(float *aptr, float *bptr) const
{
	*aptr = low[0];
	*bptr = high[0];
}

void TransferWindow::get_interval_rgba(float *rgba_low, float *rgba_high) const
{
	for(int i=0; i<3; i++) {
		rgba_low[i] = low[i];
		rgba_high[i] = high[i];
	}
}

void TransferWindow::get_interval_rgba(int channel, float *aptr, float *bptr) const
{
	*aptr = low[channel];
	*bptr = high[channel];
}

void TransferWindow::set_soft_radius(float s)
{
	soft_rad = s;
}

float TransferWindow::get_soft_radius() const
{
	return soft_rad;
}

static inline float smoothstep(float a, float b, float x)
{
	if(x < a) return 0.0f;
	if(x >= b) return 1.0f;

	float t = (x - a) / (b - a);
	return t * t * (3.0f - 2.0f * t);
}

float TransferWindow::map(float x) const
{
	float rgb[3];
	map(x, rgb);

	return std::max(rgb[0], std::max(rgb[1], rgb[2]));
}

void TransferWindow::map(float x, float *rgb_value) const
{
	for(int i=0; i<3; i++) {
		float val = smoothstep(low[i] - soft_rad, low[i] + soft_rad, x);
		val *= 1.0 - smoothstep(high[i] - soft_rad, high[i] + soft_rad, x);
		rgb_value[i] = val;
	}
}
