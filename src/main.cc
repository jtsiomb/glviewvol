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

static void display();
static void reshape(int x, int y);
static void key_down(unsigned char key, int x, int y);
static void key_up(unsigned char key, int x, int y);
static void mouse(int bn, int state, int x, int y);
static void motion(int x, int y);

static float cam_theta, cam_phi, cam_dist = 6;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1280, 800);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("dicom viewer");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyb);
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

static void display()
{
	ev_display();
}

static void reshape(int x, int y)
{
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
