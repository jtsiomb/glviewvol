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
#ifndef XFERMAP_H_
#define XFERMAP_H_

class TransferFunc {
public:
	virtual ~TransferFunc();

	virtual float map(float x) const = 0;
	virtual void map(float x, float *rgba_value) const = 0;
};

class TransferWindow : public TransferFunc {
private:
	float soft_rad;
	float low[3], high[3];	// rgb

public:
	enum { HANDLE_LOW = 0, HANDLE_HIGH = 1};

	TransferWindow();

	// handle: 0 or HANDLE_LOW is low, 1 or HANDLE_HIGH is high
	// if channel == -1, change all channels simultaneously
	void set_handle(int channel, int handle, float val);
	float get_handle(int channel, int handle) const;

	int nearest_handle(int channel, float pos) const;

	void set_interval(float a, float b);
	void set_interval(float *rgba_low, float *rgba_high);
	void set_interval_rgba(int channel, float a, float b);

	void get_interval(float *aptr, float *bptr) const;
	void get_interval_rgba(float *rgba_low, float *rgba_high) const;
	void get_interval_rgba(int channel, float *aptr, float *bptr) const;

	void set_soft_radius(float s);
	float get_soft_radius() const;

	float map(float x) const;
	void map(float x, float *rgba_value) const;
};

#endif	// XFERMAP_H_
