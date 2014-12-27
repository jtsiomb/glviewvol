#include "opengl.h"
#include "rend_fast.h"

RendererFast::RendererFast()
{
	vol_tex = 0;
	vol_tex_valid = false;
}

bool RendererFast::init()
{
	glGenTextures(1, &vol_tex);
	glBindTexture(GL_TEXTURE_3D, vol_tex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void RendererFast::destroy()
{
	glDeleteTextures(1, &vol_tex);
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

		int int_fmt = GLEW_ARB_texture_float ? GL_RGBA16F_ARB : GL_RGBA;

		glBindTexture(GL_TEXTURE_3D, vol_tex);
		glTexImage3D(GL_TEXTURE_3D, 0, int_fmt, xsz, ysz, zsz, 0, GL_RGBA, GL_FLOAT, 0);

		float *slice = new float[xsz * ysz * 4];

		for(int i=0; i<zsz; i++) {
			float z = (float)i / (float)(zsz - 1);
			float *pptr = slice;

			for(int j=0; j<ysz; j++) {
				float y = (float)j / (float)(ysz - 1);
				for(int k=0; k<xsz; k++) {
					float x = (float)k / (float)(xsz - 1);

					// value in alpha channel
					pptr[3] = vol->valuef(x, y, z);
					// normal in rgb
					vol->normalf(pptr, x, y, z);
					// shift normal to the [0,1] range in case we don't have tex_float
					pptr[0] = pptr[0] * 0.5 + 0.5;
					pptr[1] = pptr[1] * 0.5 + 0.5;
					pptr[2] = pptr[2] * 0.5 + 0.5;
				}
			}

			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, i, xsz, ysz, 1, GL_RGBA, GL_FLOAT, slice);
		}

		delete [] slice;

		vol_tex_valid = true;
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

	glBindTexture(GL_TEXTURE_3D, vol_tex);
	glBegin(GL_QUADS);
	glTexCoord3f(0, 0, 0); glVertex2f(-1, -1);
	glTexCoord3f(1, 0, 0); glVertex2f(1, -1);
	glTexCoord3f(1, 1, 0); glVertex2f(1, 1);
	glTexCoord3f(0, 1, 0); glVertex2f(-1, 1);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
