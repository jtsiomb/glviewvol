#include <stdio.h>
#include "opengl.h"
#include "dicomview.h"
#include "rend_fast.h"
#include "opt.h"
#include "volume.h"

static int win_width, win_height;
static float cam_theta, cam_phi, cam_dist = 6;

static Renderer *rend;
static Volume *vol;

extern "C" {

int init()
{
	if(!opt.fname) {
		fprintf(stderr, "you must specify the volume data filename\n");
		return -1;
	}

	switch(opt.rend_type) {
	case REND_FAST:
		rend = new RendererFast;
		break;
	default:
		return -1;
	}

	if(!rend->init()) {
		fprintf(stderr, "renderer initialization failed\n");
		return -1;
	}

	VoxelVolume *voxvol = new VoxelVolume;
	if(!voxvol->load(opt.fname)) {
		fprintf(stderr, "failed to load volume data from: %s\n", opt.fname);
		return -1;
	}
	vol = voxvol;
	rend->set_volume(vol);

	return 0;
}

void cleanup()
{
	rend->destroy();
	delete rend;
	delete vol;
}

void ev_display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	rend->update(0);
	rend->render();
}

void ev_reshape(int x, int y)
{
	win_width = x;
	win_height = y;

	glViewport(0, 0, x, y);
}

void ev_keyboard(int key, int press, int x, int y)
{
	if(press) {
		switch(key) {
		case 27:
			quit();
		}
	}
}

void ev_mouse_button(int bn, int press, int x, int y)
{
}

void ev_mouse_motion(int x, int y)
{
}

}	// extern "C"
