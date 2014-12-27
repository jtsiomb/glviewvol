#ifndef REND_FAST_H_
#define REND_FAST_H_

#include "renderer.h"

class RendererFast : public Renderer {
private:
	unsigned int vol_tex;
	bool vol_tex_valid;

public:
	RendererFast();

	bool init();
	void destroy();

	void update(unsigned int msec);
	void render() const;
};

#endif	// REND_FAST_H_
