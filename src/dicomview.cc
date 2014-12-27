#include "opengl.h"
#include "dicomview.h"

static float cam_theta, cam_phi, cam_dist = 6;

extern "C" {

int init()
{
}

void cleanup()
{
}

void ev_display()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void ev_reshape(int x, int y)
{
	win_width = x;
	win_height = y;

	glViweport(0, 0, x, y);
}

void ev_keyboard(int key, int press, int x, int y);
void ev_mouse_button(int bn, int press, int x, int y);
void ev_mouse_motion(int x, int y);

}	// extern "C"
