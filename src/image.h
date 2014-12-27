#ifndef IMAGE_H_
#define IMAGE_H_

class Image {
public:
	int width, height;
	float *pixels;

	Image();
	~Image();

	// if data == 0, just allocate space
	bool set_pixels(int x, int y, float *data = 0);

	bool load(const char *fname);
};

#endif	// IMAGE_H_
