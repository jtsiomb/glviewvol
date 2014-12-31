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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <alloca.h>
#include <errno.h>
#include <math.h>
#include "volume.h"

static char *strip_space(char *s);


Volume::~Volume()
{
}

int Volume::num_samples(int dim) const
{
	return 0;
}

void Volume::normalf(float *norm, float x, float y, float z, float delta)
{
	float dx, dy, dz;
	dx = dy = dz = delta;

	if(num_samples(0) > 0) {
		// discrete volume
		dx /= (float)num_samples(0);
		dy /= (float)num_samples(1);
		dz /= (float)num_samples(2);
	}

	norm[0] = valuef(x + dx, y, z) - valuef(x - dx, y, z);
	norm[1] = valuef(x, y + dy, z) - valuef(x, y - dy, z);
	norm[2] = valuef(x, y, z + dz) - valuef(x, y, z - dz);

	float len = sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
	if(len != 0.0) {
		norm[0] /= len;
		norm[1] /= len;
		norm[2] /= len;
	}
}

void Volume::normali(float *norm, int x, int y, int z)
{
	int sz[3];

	if((sz[0] = num_samples(0)) <= 0) {
		// if it's a continuous volume, just call normalf
		normalf(norm, x, y, z);
		return;
	}
	sz[1] = num_samples(1);
	sz[2] = num_samples(2);

	int prevx = x <= 0 ? 0 : x;
	int nextx = x >= sz[0] - 1 ? sz[0] - 1 : x;
	int prevy = y <= 0 ? 0 : y;
	int nexty = y >= sz[1] - 1 ? sz[1] - 1 : y;
	int prevz = z <= 0 ? 0 : z;
	int nextz = z >= sz[2] - 1 ? sz[2] - 1 : z;

	norm[0] = valuei(nextx, y, z) - valuei(prevx, y, z);
	norm[1] = valuei(x, nexty, z) - valuei(x, prevy, z);
	norm[2] = valuei(x, y, nextz) - valuei(x, y, prevz);

	float len = sqrt(norm[0] * norm[0] + norm[1] * norm[1] + norm[2] * norm[2]);
	if(len != 0.0) {
		norm[0] /= len;
		norm[1] /= len;
		norm[2] /= len;
	}
}

// ---- VoxelVolume (discrete) implementation ----
VoxelVolume::VoxelVolume()
{
	size[0] = size[1] = size[2] = 0;
}

VoxelVolume::~VoxelVolume()
{
	for(size_t i=0; i<slices.size(); i++) {
		slices[i].destroy();
	}
}

bool VoxelVolume::load(const char *fname)
{
	if(!fname) return false;

	char *prefix = (char*)alloca(strlen(fname) + 1);
	strcpy(prefix, fname);
	char *slash = strrchr(prefix, '/');
	if(slash) {
		*slash = 0;
	} else {
		prefix = 0;
	}

	printf("loading volume dataset: %s\n", fname);
	FILE *fp = fopen(fname, "r");
	if(!fp) {
		fprintf(stderr, "failed to open file: %s: %s\n", fname, strerror(errno));
		return false;
	}

	char buf[256], path[300];
	while(fgets(buf, sizeof buf, fp)) {
		char *line = strip_space(buf);
		sprintf(path, "%s/%s", prefix, line);

		printf("  loading slice %d: %s\n", (int)slices.size(), path);
		Image img;
		if(!img.load(path)) {
			slices.clear();
			return false;
		}

		if(slices.empty()) {
			size[0] = img.width;
			size[1] = img.height;
		} else {
			if(img.width != size[0] || img.height != size[1]) {
				fprintf(stderr, "slice %d \"%s\" size mismatch (%dx%d, %dx%d expected)\n",
						(int)slices.size(), line, img.width, img.height, size[0], size[1]);
				slices.clear();
				return false;
			}
		}

		slices.push_back(img);
	}

	size[2] = slices.size();
	fclose(fp);
	return true;
}

int VoxelVolume::num_samples(int dim) const
{
	return size[dim];
}

static inline int clamp(int x, int low, int high)
{
	return x < low ? low : (x > high ? high : x);
}

static inline float lookup(int x0, int y0, int x1, int y1, float tx, float ty,
		float *pixels, int width, int height)
{
	float v00, v01, v10, v11;

	v00 = pixels[y0 * width + x0];
	v01 = pixels[y1 * width + x0];
	v10 = pixels[y0 * width + x1];
	v11 = pixels[y1 * width + x1];

	float v0 = v00 + (v01 - v00) * ty;
	float v1 = v10 + (v11 - v10) * ty;

	return v0 + (v1 - v0) * tx;
}

float VoxelVolume::valuef(float x, float y, float z) const
{
	if(slices.empty()) {
		return 0.0f;
	}

	float floor_x = floor(x);
	float ceil_x = ceil(x);
	float tx = x - floor_x;

	float floor_y = floor(y);
	float ceil_y = ceil(y);
	float ty = y - floor_y;

	float floor_z = floor(z);
	float ceil_z = ceil(z);
	float tz = z - floor_z;

	int x0 = clamp(floor_x, 0, size[0] - 1);
	int x1 = clamp(ceil_x, 0, size[0] - 1);

	int y0 = clamp(floor_y, 0, size[1] - 1);
	int y1 = clamp(ceil_y, 0, size[1] - 1);

	int s0 = clamp(floor_z, 0, size[2] - 1);
	int s1 = clamp(ceil_z, 0, size[2] - 1);

	float val_s0 = lookup(x0, y0, x1, y1, tx, ty, slices[s0].pixels, size[0], size[1]);
	float val_s1 = lookup(x0, y0, x1, y1, tx, ty, slices[s1].pixels, size[0], size[1]);

	return val_s0 + (val_s1 - val_s0) * tz;
}

float VoxelVolume::valuei(int x, int y, int z) const
{
	x = clamp(x, 0, size[0] - 1);
	y = clamp(y, 0, size[1] - 1);
	z = clamp(z, 0, size[2] - 1);

	return slices[z].pixels[y * size[0] + x];
}

static char *strip_space(char *s)
{
	while(*s && isspace(*s)) s++;
	if(!*s) return 0;

	char *end = s + strlen(s) - 1;
	while(end > s && isspace(*end)) end--;
	end[1] = 0;

	return s;
}
