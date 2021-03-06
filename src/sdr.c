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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
#include <GL/glew.h>

#if defined(unix) || defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#endif	/* unix */

#include "sdr.h"

static const char *sdrtypestr(unsigned int sdrtype);

unsigned int create_vertex_shader(const char *src)
{
	return create_shader(src, GL_VERTEX_SHADER);
}

unsigned int create_pixel_shader(const char *src)
{
	return create_shader(src, GL_FRAGMENT_SHADER);
}

unsigned int create_tessctl_shader(const char *src)
{
#ifdef GL_TESS_CONTROL_SHADER
	return create_shader(src, GL_TESS_CONTROL_SHADER);
#else
	return 0;
#endif
}

unsigned int create_tesseval_shader(const char *src)
{
#ifdef GL_TESS_EVALUATION_SHADER
	return create_shader(src, GL_TESS_EVALUATION_SHADER);
#else
	return 0;
#endif
}

unsigned int create_geometry_shader(const char *src)
{
#ifdef GL_GEOMETRY_SHADER
	return create_shader(src, GL_GEOMETRY_SHADER);
#else
	return 0;
#endif
}

unsigned int create_shader(const char *src, unsigned int sdr_type)
{
	unsigned int sdr;
	int success, info_len;
	char *info_str = 0;
	GLenum err;

	sdr = glCreateShader(sdr_type);
	assert(glGetError() == GL_NO_ERROR);
	glShaderSource(sdr, 1, &src, 0);
	err = glGetError();
	assert(err == GL_NO_ERROR);
	glCompileShader(sdr);
	assert(glGetError() == GL_NO_ERROR);

	glGetShaderiv(sdr, GL_COMPILE_STATUS, &success);
	assert(glGetError() == GL_NO_ERROR);
	glGetShaderiv(sdr, GL_INFO_LOG_LENGTH, &info_len);
	assert(glGetError() == GL_NO_ERROR);

	if(info_len) {
		if((info_str = malloc(info_len + 1))) {
			glGetShaderInfoLog(sdr, info_len, 0, info_str);
			assert(glGetError() == GL_NO_ERROR);
			info_str[info_len] = 0;
		}
	}

	if(success) {
		fprintf(stderr, info_str ? "done: %s\n" : "done\n", info_str);
	} else {
		fprintf(stderr, info_str ? "failed: %s\n" : "failed\n", info_str);
		glDeleteShader(sdr);
		sdr = 0;
	}

	free(info_str);
	return sdr;
}

void free_shader(unsigned int sdr)
{
	glDeleteShader(sdr);
}

unsigned int load_vertex_shader(const char *fname)
{
	return load_shader(fname, GL_VERTEX_SHADER);
}

unsigned int load_pixel_shader(const char *fname)
{
	return load_shader(fname, GL_FRAGMENT_SHADER);
}

unsigned int load_tessctl_shader(const char *fname)
{
#ifdef GL_TESS_CONTROL_SHADER
	return load_shader(fname, GL_TESS_CONTROL_SHADER);
#else
	return 0;
#endif
}

unsigned int load_tesseval_shader(const char *fname)
{
#ifdef GL_TESS_EVALUATION_SHADER
	return load_shader(fname, GL_TESS_EVALUATION_SHADER);
#else
	return 0;
#endif
}

unsigned int load_geometry_shader(const char *fname)
{
#ifdef GL_GEOMETRY_SHADER
	return load_shader(fname, GL_GEOMETRY_SHADER);
#else
	return 0;
#endif
}

unsigned int load_shader(const char *fname, unsigned int sdr_type)
{
#if defined(unix) || defined(__unix__)
	struct stat st;
#endif
	unsigned int sdr;
	size_t filesize;
	FILE *fp;
	char *src;

	if(!(fp = fopen(fname, "rb"))) {
		fprintf(stderr, "failed to open shader %s: %s\n", fname, strerror(errno));
		return 0;
	}

#if defined(unix) || defined(__unix__)
	fstat(fileno(fp), &st);
	filesize = st.st_size;
#else
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
#endif	/* unix */

	if(!(src = malloc(filesize + 1))) {
		fclose(fp);
		return 0;
	}
	fread(src, 1, filesize, fp);
	src[filesize] = 0;
	fclose(fp);

	fprintf(stderr, "compiling %s shader: %s... ", sdrtypestr(sdr_type), fname);
	sdr = create_shader(src, sdr_type);

	free(src);
	return sdr;
}


unsigned int get_vertex_shader(const char *fname)
{
	return get_shader(fname, GL_VERTEX_SHADER);
}

unsigned int get_pixel_shader(const char *fname)
{
	return get_shader(fname, GL_FRAGMENT_SHADER);
}

unsigned int get_tessctl_shader(const char *fname)
{
#ifdef GL_TESS_CONTROL_SHADER
	return get_shader(fname, GL_TESS_CONTROL_SHADER);
#else
	return 0;
#endif
}

unsigned int get_tesseval_shader(const char *fname)
{
#ifdef GL_TESS_EVALUATION_SHADER
	return get_shader(fname, GL_TESS_EVALUATION_SHADER);
#else
	return 0;
#endif
}

unsigned int get_geometry_shader(const char *fname)
{
#ifdef GL_GEOMETRY_SHADER
	return get_shader(fname, GL_GEOMETRY_SHADER);
#else
	return 0;
#endif
}

unsigned int get_shader(const char *fname, unsigned int sdr_type)
{
	unsigned int sdr;
	if(!(sdr = load_shader(fname, sdr_type))) {
		return 0;
	}
	return sdr;
}


/* ---- gpu programs ---- */

unsigned int create_program(void)
{
	unsigned int prog = glCreateProgram();
	assert(glGetError() == GL_NO_ERROR);
	return prog;
}

unsigned int create_program_link(unsigned int sdr0, ...)
{
	unsigned int prog, sdr;
	va_list ap;

	if(!(prog = create_program())) {
		return 0;
	}

	attach_shader(prog, sdr0);
	if(glGetError()) {
		return 0;
	}

	va_start(ap, sdr0);
	while((sdr = va_arg(ap, unsigned int))) {
		attach_shader(prog, sdr);
		if(glGetError()) {
			return 0;
		}
	}
	va_end(ap);

	if(link_program(prog) == -1) {
		free_program(prog);
		return 0;
	}
	return prog;
}

unsigned int create_program_load(const char *vfile, const char *pfile)
{
	unsigned int vs = 0, ps = 0;

	if(vfile && *vfile && !(vs = get_vertex_shader(vfile))) {
		return 0;
	}
	if(pfile && *pfile && !(ps = get_pixel_shader(pfile))) {
		return 0;
	}
	return create_program_link(vs, ps, 0);
}

void free_program(unsigned int sdr)
{
	glDeleteProgram(sdr);
}

void attach_shader(unsigned int prog, unsigned int sdr)
{
	if(prog && sdr) {
		glAttachShader(prog, sdr);
		assert(glGetError() == GL_NO_ERROR);
	}
}

int link_program(unsigned int prog)
{
	int linked, info_len, retval = 0;
	char *info_str = 0;

	glLinkProgram(prog);
	assert(glGetError() == GL_NO_ERROR);
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	assert(glGetError() == GL_NO_ERROR);
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &info_len);
	assert(glGetError() == GL_NO_ERROR);

	if(info_len) {
		if((info_str = malloc(info_len + 1))) {
			glGetProgramInfoLog(prog, info_len, 0, info_str);
			assert(glGetError() == GL_NO_ERROR);
			info_str[info_len] = 0;
		}
	}

	if(linked) {
		fprintf(stderr, info_str ? "linking done: %s\n" : "linking done\n", info_str);
	} else {
		fprintf(stderr, info_str ? "linking failed: %s\n" : "linking failed\n", info_str);
		retval = -1;
	}

	free(info_str);
	return retval;
}

int bind_program(unsigned int prog)
{
	GLenum err;

	glUseProgram(prog);
	if(prog && (err = glGetError()) != GL_NO_ERROR) {
		/* maybe the program is not linked, try linking first */
		if(err == GL_INVALID_OPERATION) {
			if(link_program(prog) == -1) {
				return -1;
			}
			glUseProgram(prog);
			return glGetError() == GL_NO_ERROR ? 0 : -1;
		}
		return -1;
	}
	return 0;
}

/* ugly but I'm not going to write the same bloody code over and over */
#define BEGIN_UNIFORM_CODE \
	int loc, curr_prog; \
	glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog); \
	if((unsigned int)curr_prog != prog && bind_program(prog) == -1) { \
		return -1; \
	} \
	if((loc = glGetUniformLocation(prog, name)) != -1)

#define END_UNIFORM_CODE \
	if((unsigned int)curr_prog != prog) { \
		bind_program(curr_prog); \
	} \
	return loc == -1 ? -1 : 0

int set_uniform_int(unsigned int prog, const char *name, int val)
{
	BEGIN_UNIFORM_CODE {
		glUniform1i(loc, val);
	}
	END_UNIFORM_CODE;
}

int set_uniform_float(unsigned int prog, const char *name, float val)
{
	BEGIN_UNIFORM_CODE {
		glUniform1f(loc, val);
	}
	END_UNIFORM_CODE;
}

int set_uniform_float2(unsigned int prog, const char *name, float x, float y)
{
	BEGIN_UNIFORM_CODE {
		glUniform2f(loc, x, y);
	}
	END_UNIFORM_CODE;
}

int set_uniform_float3(unsigned int prog, const char *name, float x, float y, float z)
{
	BEGIN_UNIFORM_CODE {
		glUniform3f(loc, x, y, z);
	}
	END_UNIFORM_CODE;
}

int set_uniform_float4(unsigned int prog, const char *name, float x, float y, float z, float w)
{
	BEGIN_UNIFORM_CODE {
		glUniform4f(loc, x, y, z, w);
	}
	END_UNIFORM_CODE;
}

int set_uniform_matrix4(unsigned int prog, const char *name, float *mat)
{
	BEGIN_UNIFORM_CODE {
		glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
	}
	END_UNIFORM_CODE;
}

int set_uniform_matrix4_transposed(unsigned int prog, const char *name, float *mat)
{
	BEGIN_UNIFORM_CODE {
		glUniformMatrix4fv(loc, 1, GL_TRUE, mat);
	}
	END_UNIFORM_CODE;
}

int get_attrib_loc(unsigned int prog, const char *name)
{
	int loc, curr_prog;

	glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
	if((unsigned int)curr_prog != prog && bind_program(prog) == -1) {
		return -1;
	}

	loc = glGetAttribLocation(prog, (char*)name);

	if((unsigned int)curr_prog != prog) {
		bind_program(curr_prog);
	}
	return loc;
}

void set_attrib_float3(int attr_loc, float x, float y, float z)
{
	glVertexAttrib3f(attr_loc, x, y, z);
}

static const char *sdrtypestr(unsigned int sdrtype)
{
	switch(sdrtype) {
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "pixel";
#ifdef GL_TESS_CONTROL_SHADER
	case GL_TESS_CONTROL_SHADER:
		return "tessellation control";
#endif
#ifdef GL_TESS_EVALUATION_SHADER
	case GL_TESS_EVALUATION_SHADER:
		return "tessellation evaluation";
#endif
#ifdef GL_GEOMETRY_SHADER
	case GL_GEOMETRY_SHADER:
		return "geometry";
#endif

	default:
		break;
	}
	return "<unknown>";
}
