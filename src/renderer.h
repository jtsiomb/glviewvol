#ifndef RENDERER_H_
#define RENDERER_H_

#include "volume.h"

class Renderer {
protected:
	int view_width, view_height;
	Volume *vol;

public:
	Renderer();
	virtual ~Renderer();

	virtual bool init();
	virtual void destroy();

	virtual void set_volume(Volume *vol);
	virtual Volume *get_volume() const;

	virtual void reshape(int x, int y);

	virtual void update(unsigned int msec);
	virtual void render() const = 0;
};

#endif	// RENDERER_H_
