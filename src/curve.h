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
#ifndef CURVE_H_
#define CURVE_H_

#include <vector>
#include <inttypes.h>

struct CurvePoint {
	uint16_t t_int;	// save time as an integer to allow exact lookup and change
	float value;
};

class Curve {
private:
	std::vector<CurvePoint> cp;

public:
	void set_point(float t, float val);
	void set_point_int(uint16_t ti, float val);

	bool delete_point(uint16_t ti);

	CurvePoint *get_point(int idx);
	const CurvePoint *get_point(int idx) const;
	int get_num_points() const;

	CurvePoint *get_point_at(uint16_t ti);
	const CurvePoint *get_point_at(uint16_t ti) const;

	float value(float t) const;
	float value_int(uint16_t ti) const;
};

#endif	// CURVE_H_
