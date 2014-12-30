#include <stdio.h>
#include "opengl.h"
#include "xfer_view.h"
#include "dicomview.h"

static TransferFunc *xfer;

static int act_color = 3;

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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int xsz, ysz;
	get_window_size(&xsz, &ysz);
	int nsamples = xsz / 4;

	// paint the background a faint version of the selected color
	glBegin(GL_QUADS);
	glColor3f(line_color[act_color][0] * 0.1, line_color[act_color][1] * 0.1, line_color[act_color][2] * 0.1);
	glVertex2f(-1, -1);
	glVertex2f(1, -1);
	glVertex2f(1, 1);
	glVertex2f(-1, 1);
	glEnd();

	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw curve
	glLineWidth(2.0);

	for(int i=0; i<3; i++) {
		int idx;
		if(act_color < 3) {
			idx = (i + act_color + 1) % 3;
		} else {
			idx = i;
		}

		glColor3fv(line_color[idx]);

		glBegin(GL_LINE_STRIP);
		for(int j=0; j<nsamples; j++) {
			float t = (float)j / (float)(nsamples - 1);
			float vval[4];
			xfer->map(t, vval);

			glVertex2f(t * 2.0 - 1.0, vval[i] * 2.0 - 1.0);
		}
		glEnd();
	}

	glDisable(GL_BLEND);
}

void xfview_button(int bn, int press, int x, int y)
{
	if(bn == 2 && press) {
		act_color = (act_color + 1) % 4;
		redisplay();
		return;
	}

	if(bn == 1) {
		if(press) {
		} else {
		}
	}
}

void xfview_motion(int x, int y)
{
}
