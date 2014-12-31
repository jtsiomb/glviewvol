/*
glviewvol is an OpenGL 3D volume data viewer
Copyright (C) 2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
// GLUT frontend
#ifdef USE_GLUT

#include <stdlib.h>
#include "opengl.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "viewer.h"
#include "opt.h"

static void display();
static void reshape(int x, int y);
static void key_down(unsigned char key, int x, int y);
static void key_up(unsigned char key, int x, int y);
static void mouse(int bn, int state, int x, int y);
static void motion(int x, int y);

static int win_width, win_height;
static unsigned int mod;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	if(parse_args(argc, argv) == -1) {
		return 1;
	}

	glutInitWindowSize(opt.xsz, opt.ysz);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("dicom viewer");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_down);
	glutKeyboardUpFunc(key_up);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);

	glewInit();

	if(init() == -1) {
		return 1;
	}

	glutMainLoop();
	return 0;
}

void swap_buffers()
{
	glutSwapBuffers();
}

void redisplay()
{
	glutPostRedisplay();
}

void quit()
{
	cleanup();
	exit(0);
}

void get_window_size(int *xsz, int *ysz)
{
	*xsz = win_width;
	*ysz = win_height;
}

unsigned int get_modifiers()
{
	return mod;
}

static void display()
{
	ev_display();
}

static void reshape(int x, int y)
{
	win_width = x;
	win_height = y;
	ev_reshape(x, y);
}

static void key_down(unsigned char key, int x, int y)
{
	mod = glutGetModifiers();

	switch(key) {
	case 'i':
		{
			static bool fullrate;
			fullrate = !fullrate;
			glutIdleFunc(fullrate ? glutPostRedisplay : 0);
		}
		break;
	}

	ev_keyboard(key, 1, x, y);
}

static void key_up(unsigned char key, int x, int y)
{
	mod = glutGetModifiers();
	ev_keyboard(key, 0, x, y);
}

static void mouse(int bn, int state, int x, int y)
{
	mod = glutGetModifiers();
	ev_mouse_button(bn - GLUT_LEFT_BUTTON, state == GLUT_DOWN ? 1 : 0, x, y);
}

static void motion(int x, int y)
{
	ev_mouse_motion(x, y);
}

#endif	// USE_GLUT
