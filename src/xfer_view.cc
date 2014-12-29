#include <stdio.h>
#include "opengl.h"
#include "xfer_view.h"
#include "dicomview.h"

static Renderer *rend;

static int act_color = 3;
static CurvePoint *cpsel[3];

bool xfview_init(Renderer *rendarg)
{
	rend = rendarg;
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

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw selection outline
	if(act_color < 3) {
		glPointSize(13.0);
		glLineWidth(5.0);

		const Curve &sel_curve = rend->transfer_curve(act_color);
		glColor3f(0.7, 0.7, 0.7);
		glBegin(GL_LINE_STRIP);
		for(int i=0; i<nsamples; i++) {
			float t = (float)i / (float)(nsamples - 1);
			float val = sel_curve.value(t);
			glVertex2f(t * 2.0 - 1.0, val * 2.0 - 1.0);
		}
		glEnd();
		glBegin(GL_POINTS);
		for(int i=0; i<sel_curve.get_num_points(); i++) {
			const CurvePoint *p = sel_curve.get_point(i);
			float x = 2.0 * (float)p->t_int / 65535.0 - 1.0;
			glVertex2f(x, p->value * 2.0 - 1.0);
		}
		glEnd();
	}


	// draw curves and points
	glPointSize(9.0);
	glLineWidth(2.0);

	for(int i=0; i<3; i++) {
		int idx;
		if(act_color < 3) {
			idx = (i + act_color + 1) % 3;
		} else {
			idx = i;
		}

		const Curve &xfer = rend->transfer_curve(idx);
		glColor3fv(line_color[idx]);

		glBegin(GL_LINE_STRIP);
		for(int j=0; j<nsamples; j++) {
			float t = (float)j / (float)(nsamples - 1);
			float val = xfer.value(t);
			glVertex2f(t * 2.0 - 1.0, val * 2.0 - 1.0);
		}
		glEnd();

		glBegin(GL_POINTS);
		for(int j=0; j<xfer.get_num_points(); j++) {
			const CurvePoint *p = xfer.get_point(j);
			float x = 2.0 * (float)p->t_int / 65535.0 - 1.0;
			glVertex2f(x, p->value * 2.0 - 1.0);
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

	if(press) {
	}
}

void xfview_motion(int x, int y)
{
}
