// GLUT frontend
#ifdef USE_GLUT

#include <stdlib.h>
#include "opengl.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "dicomview.h"
#include "opt.h"

static void display();
static void reshape(int x, int y);
static void key_down(unsigned char key, int x, int y);
static void key_up(unsigned char key, int x, int y);
static void mouse(int bn, int state, int x, int y);
static void motion(int x, int y);

static int win_width, win_height;

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
	ev_keyboard(key, 1, x, y);
}

static void key_up(unsigned char key, int x, int y)
{
	ev_keyboard(key, 0, x, y);
}

static void mouse(int bn, int state, int x, int y)
{
	ev_mouse_button(bn - GLUT_LEFT_BUTTON, state == GLUT_DOWN ? 1 : 0, x, y);
}

static void motion(int x, int y)
{
	ev_mouse_motion(x, y);
}

#endif	// USE_GLUT
