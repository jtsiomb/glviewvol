#include "opengl.h"
#include "dicomview.h"
#include "rend_fast.h"

static int win_width, win_height;
static float cam_theta, cam_phi, cam_dist = 6;

static Renderer *rend;

extern "C" {

int init()
{
	rend = new RendererFast;
	if(!rend->init()) {
		return -1;
	}

	return 0;
}

void cleanup()
{
	rend->destroy();
	delete rend;
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
