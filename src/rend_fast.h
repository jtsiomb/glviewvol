#ifndef REND_FAST_H_
#define REND_FAST_H_

#include "renderer.h"

class RendererFast : public Renderer {
private:
	unsigned int vol_tex, xfer_tex;
	bool vol_tex_valid, xfer_tex_valid;

public:
	RendererFast();

	bool init();
	void destroy();

	void set_volume(Volume *vol);
	Curve &transfer_curve(int color);
	void set_simple_transfer(float low, float high);

	void update(unsigned int msec);
	void render() const;
};

#endif	// REND_FAST_H_
