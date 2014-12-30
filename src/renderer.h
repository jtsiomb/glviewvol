#ifndef RENDERER_H_
#define RENDERER_H_

#include "volume.h"
#include "xfermap.h"

#define MAX_CLIP_PLANES		4

class Renderer {
protected:
	int view_width, view_height;
	Volume *vol;

	float clip_plane[MAX_CLIP_PLANES][4];	// nx,ny,nz,dist

	TransferFunc *xfer;

public:
	Renderer();
	virtual ~Renderer();

	virtual bool init();
	virtual void destroy();

	virtual void set_volume(Volume *vol);
	virtual Volume *get_volume() const;

	virtual void set_transfer_function(TransferFunc *xfer);
	virtual TransferFunc *get_transfer_function() const;

	virtual void set_clipping_plane(int idx, float nx, float ny, float nz, float dist);
	virtual void disable_clipping_plane(int idx);

	virtual void reshape(int x, int y);

	virtual void update(unsigned int msec);
	virtual void render() const = 0;
};

#endif	// RENDERER_H_
