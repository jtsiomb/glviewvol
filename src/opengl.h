#ifndef OPENGL_H_
#define OPENGL_H_

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#endif

#endif	/* OPENGL_H_ */
