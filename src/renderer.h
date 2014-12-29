#ifndef RENDERER_H_
#define RENDERER_H_

#include "volume.h"
#include "curve.h"

#define MAX_CLIP_PLANES		4

class Renderer {
protected:
	int view_width, view_height;
	Volume *vol;

	float clip_plane[MAX_CLIP_PLANES][4];	// nx,ny,nz,dist

	Curve xfer[3];	// rgb transfer function
	float xfer_low, xfer_high;	// simple transfer function bounds

public:
	Renderer();
	virtual ~Renderer();

	virtual bool init();
	virtual void destroy();

	virtual void set_volume(Volume *vol);
	virtual Volume *get_volume() const;

	virtual Curve &transfer_curve(int color);
	virtual const Curve &transfer_curve(int color) const;

	virtual void set_simple_transfer(float low, float high);
	virtual void get_simple_transfer(float *low, float *high) const;

	virtual void set_clipping_plane(int idx, float nx, float ny, float nz, float dist);
	virtual void disable_clipping_plane(int idx);

	virtual void reshape(int x, int y);

	virtual void update(unsigned int msec);
	virtual void render() const = 0;
};

#endif	// RENDERER_H_
