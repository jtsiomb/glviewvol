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
