#include <stdio.h>
#include "opengl.h"
#include "rend_fast.h"
#include "sdr.h"

static inline float smoothstep(float a, float b, float x);

#define XFER_MAP_SZ		1024

static unsigned int sdr;
static bool have_tex_float;

RendererFast::RendererFast()
{
	vol_tex = xfer_tex = 0;
	vol_tex_valid = xfer_tex_valid = false;
}

bool RendererFast::init()
{
	if(!sdr) {
		if(!(sdr = create_program_load("sdr/fast.v.glsl", "sdr/fast.p.glsl"))) {
			return false;
		}
		have_tex_float = GLEW_ARB_texture_float;
	}

	glGenTextures(1, &vol_tex);
	glBindTexture(GL_TEXTURE_3D, vol_tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenTextures(1, &xfer_tex);
	glBindTexture(GL_TEXTURE_1D, xfer_tex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, have_tex_float ? GL_RGBA16F : GL_RGBA, XFER_MAP_SZ, 0, GL_RGB, GL_FLOAT, 0);

	return true;
}

void RendererFast::destroy()
{
	glDeleteTextures(1, &vol_tex);
	glDeleteTextures(1, &xfer_tex);
}

void RendererFast::set_volume(Volume *vol)
{
	vol_tex_valid = false;
	Renderer::set_volume(vol);
}

void RendererFast::update(unsigned int msec)
{
	if(!vol) return;

	// make sure the 3D volume texture is up to date
	if(!vol_tex_valid) {
		int xsz, ysz, zsz;

		if((xsz = vol->num_samples(0)) > 0) {
			ysz = vol->num_samples(1);
			zsz = vol->num_samples(2);
		} else {
			xsz = ysz = zsz = 256;
		}

		printf("updating 3D texture data (%dx%dx%d) ... ", xsz, ysz, zsz);
		fflush(stdout);

		int int_fmt = GLEW_ARB_texture_float ? GL_RGBA16F_ARB : GL_RGBA;

		glBindTexture(GL_TEXTURE_3D, vol_tex);
		glTexImage3D(GL_TEXTURE_3D, 0, int_fmt, xsz, ysz, zsz, 0, GL_RGBA, GL_FLOAT, 0);

		float *slice = new float[xsz * ysz * 4];

		for(int i=0; i<zsz; i++) {
			float z = (float)i;
			float *pptr = slice;

			for(int j=0; j<ysz; j++) {
				float y = (float)j;
				for(int k=0; k<xsz; k++) {
					float x = (float)k;

					// value in alpha channel
					pptr[3] = vol->valuef(x, y, z);
					// normal in rgb
					vol->normalf(pptr, x, y, z);
					// shift normal to the [0,1] range in case we don't have tex_float
					pptr[0] = pptr[0] * 0.5 + 0.5;
					pptr[1] = pptr[1] * 0.5 + 0.5;
					pptr[2] = pptr[2] * 0.5 + 0.5;

					pptr += 4;
				}
			}

			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, i, xsz, ysz, 1, GL_RGBA, GL_FLOAT, slice);
		}

		printf("done\n");

		delete [] slice;

		vol_tex_valid = true;
	}

	if(1) {//if(!xfer_tex_valid) {
		float pixels[XFER_MAP_SZ * 4];
		float *pptr = pixels;

		for(int i=0; i<XFER_MAP_SZ; i++) {
			float x = (float)i / (float)(XFER_MAP_SZ - 1);

			if(xfer) {
				xfer->map(x, pptr);
			} else {
				pptr[0] = pptr[1] = pptr[2] = pptr[3] = x;
			}
			pptr += 4;
		}

		glBindTexture(GL_TEXTURE_1D, xfer_tex);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, XFER_MAP_SZ, GL_RGBA, GL_FLOAT, pixels);

		xfer_tex_valid = true;
	}
}

void RendererFast::render() const
{
	if(!vol) return;

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(sdr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, vol_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, xfer_tex);

	set_uniform_int(sdr, "vol_tex", 0);
	set_uniform_int(sdr, "xfer_tex", 1);

	glBegin(GL_QUADS);
	glTexCoord3f(0, 0, 0.5); glVertex2f(-1, -1);
	glTexCoord3f(1, 0, 0.5); glVertex2f(1, -1);
	glTexCoord3f(1, 1, 0.5); glVertex2f(1, 1);
	glTexCoord3f(0, 1, 0.5); glVertex2f(-1, 1);
	glEnd();

	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static inline float smoothstep(float a, float b, float x)
{
	if(x < a) return 0.0;
	if(x >= b) return 1.0;

	x = (x - a) / (b - a);
	return x * x * (3.0 - 2.0 * x);
}
