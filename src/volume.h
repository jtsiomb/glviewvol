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
#ifndef VOLUME_H_
#define VOLUME_H_

#include <vector>
#include "image.h"

class Volume {
public:
	virtual ~Volume();

	// returns 0 if the volume is continuously defined
	virtual int num_samples(int dim) const;

	// central differences offset will be delta / num_samples for discrete volumes
	// and delta for continuous volumes
	virtual void normalf(float *norm, float x, float y, float z, float delta = 1.0);
	virtual void normali(float *norm, int x, int y, int z);

	virtual float valuef(float x, float y, float z) const = 0;
	virtual float valuei(int x, int y, int z) const = 0;
};

class VoxelVolume : public Volume {
protected:
	int size[3];
	std::vector<Image> slices;

	float *hist;
	int num_hist_samples;
	bool hist_valid;

public:
	VoxelVolume();
	~VoxelVolume();

	bool load(const char *fname);

	int num_samples(int dim) const;

	float valuef(float x, float y, float z) const;
	float valuei(int x, int y, int z) const;

	float *calc_histogram(int hist_samples);
};

#endif	// VOLUME_H_
