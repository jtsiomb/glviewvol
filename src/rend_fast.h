#ifndef REND_FAST_H_
#define REND_FAST_H_

#include "renderer.h"

class RendererFast : public Renderer {
private:
	unsigned int vol_tex, xfer_tex;
	bool vol_tex_valid, xfer_tex_valid;

	int proxy_count, vbo_proxy_count;
	unsigned int vbo;

public:
	RendererFast();

	bool init();
	void destroy();

	void set_volume(Volume *vol);

	void set_proxy_count(int n);
	int get_proxy_count() const;

	void update(unsigned int msec);
	void render() const;
};

#endif	// REND_FAST_H_
