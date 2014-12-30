#include <stdio.h>
#include <math.h>
#include "opengl.h"
#include "xfer_view.h"
#include "dicomview.h"

static TransferFunc *xfer;

static int act_color = -1;
static int grabbed_handle = -1;
static int mousex, mousey;

bool xfview_init(TransferFunc *xferarg)
{
	xfer = xferarg;
	return true;
}

void xfview_destroy()
{
}

void xfview_draw()
{
	float line_color[][3] = {
		{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 }
	};

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int xsz, ysz;
	get_window_size(&xsz, &ysz);
	int nsamples = xsz / 4;

	// paint the background a faint version of the selected color
	glBegin(GL_QUADS);
	int cidx = act_color == -1 ? 3 : act_color;
	glColor3f(line_color[cidx][0] * 0.1, line_color[cidx][1] * 0.1, line_color[cidx][2] * 0.1);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();

	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw handles on the selected curve
	TransferWindow *xfwin = dynamic_cast<TransferWindow*>(xfer);
	if(xfwin) {
		float dx = 1.0 / (float)xsz;
		float cursor = (float)mousex / (float)xsz;
		float low[3], high[3];
		xfwin->get_interval_rgba(low, high);

		if(act_color == -1) {	// all curves
			int nearest = xfwin->nearest_handle(-1, cursor);

			if(grabbed_handle != -1) {
				glBegin(GL_LINES);
				glColor3f(0.8, 0.8, 0.8);
				for(int i=0; i<3; i++) {
					float x = xfwin->get_handle(i, nearest);
					float rad = xfwin->get_soft_radius();
					glVertex2f(x - rad, 0.5);
					glVertex2f(x + rad, 0.5);
					glVertex2f(x - rad, 0.4);
					glVertex2f(x - rad, 0.6);
					glVertex2f(x + rad, 0.4);
					glVertex2f(x + rad, 0.6);
				}
				glEnd();
			}

			// draw handles on all lines, highlighting the nearest side of all of them
			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);

			for(int i=0; i<3; i++) {
				float x = nearest == TransferWindow::HANDLE_LOW ? low[i] : high[i];
				glVertex2f(x - 2.0 * dx, -1);
				glVertex2f(x + 2.0 * dx, -1);
				glVertex2f(x + 2.0 * dx, 1);
				glVertex2f(x - 2.0 * dx, 1);
			}
			for(int i=0; i<3; i++) {
				glColor3fv(line_color[i]);
				glVertex2f(low[i] - dx, -1);
				glVertex2f(low[i] + dx, -1);
				glVertex2f(low[i] + dx, 1);
				glVertex2f(low[i] - dx, 1);
				glVertex2f(high[i] - dx, -1);
				glVertex2f(high[i] + dx, -1);
				glVertex2f(high[i] + dx, 1);
				glVertex2f(high[i] - dx, 1);
			}
			glEnd();

		} else {
			int nearest = xfwin->nearest_handle(act_color, cursor);
			float x = nearest == TransferWindow::HANDLE_LOW ? low[act_color] : high[act_color];

			if(grabbed_handle != -1) {
				float x = xfwin->get_handle(act_color, nearest);
				float rad = xfwin->get_soft_radius();

				glBegin(GL_LINES);
				glColor3f(0.8, 0.8, 0.8);
				glVertex2f(x - rad, 0.5);
				glVertex2f(x + rad, 0.5);
				glVertex2f(x - rad, 0.4);
				glVertex2f(x - rad, 0.6);
				glVertex2f(x + rad, 0.4);
				glVertex2f(x + rad, 0.6);
				glEnd();
			}


			glBegin(GL_QUADS);
			glColor3f(1, 1, 1);
			glVertex2f(x - 2.0 * dx, -1);
			glVertex2f(x + 2.0 * dx, -1);
			glVertex2f(x + 2.0 * dx, 1);
			glVertex2f(x - 2.0 * dx, 1);

			glColor3fv(line_color[act_color]);
			for(int i=0; i<2; i++) {
				glVertex2f(x - dx, -1);
				glVertex2f(x + dx, -1);
				glVertex2f(x + dx, 1);
				glVertex2f(x - dx, 1);
				x = nearest == TransferWindow::HANDLE_LOW ? high[act_color] : low[act_color];
			}
			glEnd();
		}
	}

	// draw curve
	glLineWidth(2.0);

	for(int i=0; i<4; i++) {
		int idx;
		if(act_color == -1) {
			idx = i;
		} else {
			idx = (i + act_color + 1) % 4;
		}

		glColor3fv(line_color[idx]);

		glBegin(GL_LINE_STRIP);
		for(int j=0; j<nsamples; j++) {
			float t = (float)j / (float)(nsamples - 1);
			float vval[4];

			if(idx < 3) {
				xfer->map(t, vval);
			} else {
				vval[3] = xfer->map(t);
			}

			glVertex2f(t, vval[idx]);
		}
		glEnd();
	}

	glDisable(GL_BLEND);
}

static int prev_x, prev_y;

void xfview_button(int bn, int press, int x, int y)
{
	prev_x = x;
	prev_y = y;

	if(bn == 2 && press && grabbed_handle == -1) {
		act_color = (act_color + 2) % 4 - 1;
		redisplay();
		return;
	}

	if(bn == 0) {
		int xsz, ysz;
		get_window_size(&xsz, &ysz);

		TransferWindow *xfwin = dynamic_cast<TransferWindow*>(xfer);
		if(xfwin && press) {
			float cursor = (float)x / (float)xsz;
			float low[3], high[3];
			xfwin->get_interval_rgba(low, high);

			// grab the nearest handle
			grabbed_handle = xfwin->nearest_handle(act_color, cursor);
		} else {
			grabbed_handle = -1;
		}
		redisplay();
	}
}

void xfview_motion(int x, int y)
{
	mousex = x;
	mousey = y;

	int dx = x - prev_x;
	int dy = y - prev_y;
	prev_x = x;
	prev_y = y;

	if(grabbed_handle != -1) {
		TransferWindow *xfwin = dynamic_cast<TransferWindow*>(xfer);
		if(!xfwin) return;

		int xsz, ysz;
		get_window_size(&xsz, &ysz);

		if(get_modifiers()) {
			float soft = xfwin->get_soft_radius() + dy * 0.01;
			if(soft < 0.0) soft = 0.0;
			xfwin->set_soft_radius(soft);
		} else {
			float pos = (float)x / (float)xsz;
			xfwin->set_handle(act_color, grabbed_handle, pos);
		}
	}

	redisplay();
}
