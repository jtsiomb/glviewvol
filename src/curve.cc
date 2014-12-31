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
#include <algorithm>
#include "curve.h"

#define CLAMP(x, low, high)	std::min<float>(std::max<float>(x, low), high)
#define INDEX(x) ((x) * 65535.0)

void Curve::set_point(float t, float val)
{
	uint16_t x = INDEX(CLAMP(t, 0.0, 1.0));
	set_point_int(x, val);
}

static bool cpcmp(const CurvePoint &a, const CurvePoint &b)
{
	return a.t_int < b.t_int;
}

void Curve::set_point_int(uint16_t ti, float val)
{
	CurvePoint *p = get_point_at(ti);
	if(p) {
		p->value = val;
	} else {
		CurvePoint p;
		p.t_int = ti;
		p.value = val;
		cp.push_back(p);
		std::sort(cp.begin(), cp.end(), cpcmp);
	}
}

bool Curve::delete_point(uint16_t ti)
{
	int sz = (int)cp.size();
	for(int i=0; i<sz; i++) {
		if(cp[i].t_int == ti) {
			cp.erase(cp.begin() + i);
			return true;
		}
	}
	return false;
}

CurvePoint *Curve::get_point(int idx)
{
	if(idx < 0 || idx >= (int)cp.size()) {
		return 0;
	}
	return &cp[idx];
}

const CurvePoint *Curve::get_point(int idx) const
{
	if(idx < 0 || idx >= (int)cp.size()) {
		return 0;
	}
	return &cp[idx];
}

int Curve::get_num_points() const
{
	return (int)cp.size();
}

CurvePoint *Curve::get_point_at(uint16_t ti)
{
	int sz = (int)cp.size();
	for(int i=0; i<sz; i++) {
		if(cp[i].t_int == ti) {
			return &cp[i];
		}
	}
	return 0;
}

const CurvePoint *Curve::get_point_at(uint16_t ti) const
{
	int sz = (int)cp.size();
	for(int i=0; i<sz; i++) {
		if(cp[i].t_int == ti) {
			return &cp[i];
		}
	}
	return 0;
}

float Curve::value(float t) const
{
	uint16_t x = INDEX(CLAMP(t, 0.0, 1.0));
	return value_int(x);
}

float Curve::value_int(uint16_t ti) const
{
	CurvePoint p = { ti, 0 };
	std::vector<CurvePoint>::const_iterator it;
	it = std::lower_bound(cp.begin(), cp.end(), p, cpcmp);
	if(ti >= it->t_int || it == cp.begin()) {
		return it->value;
	}

	std::vector<CurvePoint>::const_iterator prev = it - 1;

	float t = (float)(ti - prev->t_int) / (float)(it->t_int - prev->t_int);
	return prev->value + (it->value - prev->value) * t;
}
