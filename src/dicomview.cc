#include <stdio.h>
#include <stdlib.h>
#include "opengl.h"
#include "dicomview.h"
#include "rend_fast.h"
#include "opt.h"
#include "volume.h"
#include "xfer_view.h"

static int win_width, win_height;
static float cam_theta, cam_phi, cam_dist = 4;
static float pre_rot = -90;
static int splitter_y = -1;

#define SPLITTER_WIDTH			5

static Renderer *rend;
static Volume *vol;
static TransferFunc *xfer;

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

	xfer = new TransferWindow;
	rend->set_transfer_function(xfer);

	if(!xfview_init(xfer)) {
		return -1;
	}

	return 0;
}

void cleanup()
{
	xfview_destroy();

	rend->destroy();
	delete rend;
	delete vol;
	delete xfer;
}

void ev_display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	// render the main view
	glViewport(0, win_height - splitter_y, win_width, splitter_y);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (float)win_width / (float)splitter_y, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -cam_dist);
	glRotatef(cam_phi + pre_rot, 1, 0, 0);
	glRotatef(cam_theta, 0, 1, 0);

	rend->update(0);
	rend->render();

	// draw the transfer function view
	glViewport(0, 0, win_width, win_height - splitter_y);

	xfview_draw();

	// draw the GUI
	glViewport(0, 0, win_width, win_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, win_width, win_height, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex2f(0, splitter_y + SPLITTER_WIDTH / 2);
	glVertex2f(win_width, splitter_y + SPLITTER_WIDTH / 2);
	glVertex2f(win_width, splitter_y - SPLITTER_WIDTH / 2);
	glVertex2f(0, splitter_y - SPLITTER_WIDTH / 2);
	glEnd();

	swap_buffers();
}

void ev_reshape(int x, int y)
{
	if(splitter_y < 0) {	// not initialized yet
		splitter_y = (int)(y * 0.85);
	} else {
		// calculate where the splitter was relative to the window height
		// and based on that, it's new position
		float split = (float)splitter_y / (float)win_height;
		splitter_y = (int)(y * split);
	}

	win_width = x;
	win_height = y;

	glViewport(0, 0, x, y);
	if(rend) {
		rend->reshape(x, y);
	}
}

void ev_keyboard(int key, int press, int x, int y)
{
	RendererFast *fr;

	if(press) {
		switch(key) {
		case 27:
			quit();

		case '=':
			if((fr = dynamic_cast<RendererFast*>(rend))) {
				int n = fr->get_proxy_count();
				int add = n / 4;
				n += add < 1 ? 1 : add;
				printf("proxy count: %d\n", n);
				fr->set_proxy_count(n);
			}
			redisplay();
			break;

		case '-':
			if((fr = dynamic_cast<RendererFast*>(rend))) {
				int n = fr->get_proxy_count();
				int sub = n / 4;
				n -= sub < 1 ? 1 : sub;

				if(n < 1) n = 1;

				printf("proxy count: %d\n", n);
				fr->set_proxy_count(n);
			}
			redisplay();
			break;

		default:
			break;
		}
	}
}

static bool bnstate[8];
static int prev_x, prev_y;

#define ON_SPLITTER(y)	(abs(y - splitter_y) <= SPLITTER_WIDTH / 2)
static bool splitter_dragging;

void ev_mouse_button(int bn, int press, int x, int y)
{
	bnstate[bn] = press != 0;
	prev_x = x;
	prev_y = y;

	splitter_dragging = bn == 0 && press && ON_SPLITTER(y);

	if(!splitter_dragging && y > splitter_y) {
		xfview_button(bn, press, x, y);
	}
}

void ev_mouse_motion(int x, int y)
{
	int dx = x - prev_x;
	int dy = y - prev_y;
	prev_x = x;
	prev_y = y;

	if((dx | dy) == 0) return;

	if(splitter_dragging) {
		splitter_y += dy;
		redisplay();
		return;
	}

	if(y > splitter_y) {
		xfview_motion(x, y);
		return;
	}

	// main view motion handling
	if(bnstate[0]) {
		cam_theta += dx * 0.5;
		cam_phi += dy * 0.5;

		if(cam_phi < -90) cam_phi = -90;
		if(cam_phi > 90) cam_phi = 90;
		redisplay();
	}
	if(bnstate[2]) {
		cam_dist += dy * 0.1;

		if(cam_dist < 0.0) cam_dist = 0.0;
		redisplay();
	}
}

}	// extern "C"
