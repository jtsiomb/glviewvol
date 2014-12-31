#include <stdio.h>
#include "opengl.h"
#include "rend_fast.h"
#include "sdr.h"

#define DEF_PROXY_COUNT	512
#define XFER_MAP_SZ		512

static unsigned int sdr;
static bool have_tex_float;

RendererFast::RendererFast()
{
	vol_tex = xfer_tex = 0;
	vol_tex_valid = xfer_tex_valid = false;
	proxy_count = DEF_PROXY_COUNT;
	vbo_proxy_count = 0;
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
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &xfer_tex);
	glBindTexture(GL_TEXTURE_1D, xfer_tex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, have_tex_float ? GL_RGBA16F : GL_RGBA, XFER_MAP_SZ, 0, GL_RGB, GL_FLOAT, 0);

	glGenBuffers(1, &vbo);
	return true;
}

void RendererFast::destroy()
{
	glDeleteTextures(1, &vol_tex);
	glDeleteTextures(1, &xfer_tex);
	glDeleteBuffers(1, &vbo);
}

void RendererFast::set_volume(Volume *vol)
{
	vol_tex_valid = false;
	Renderer::set_volume(vol);
}

void RendererFast::set_proxy_count(int n)
{
	proxy_count = n;
}

int RendererFast::get_proxy_count() const
{
	return proxy_count;
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

#pragma omp parallel for schedule(dynamic)
			for(int j=0; j<ysz; j++) {
				float y = (float)j;

				float *pptr = slice + (j * xsz) * 4;
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
				pptr[3] = std::max(pptr[0], std::max(pptr[1], pptr[2]));
			} else {
				pptr[0] = pptr[1] = pptr[2] = pptr[3] = x;
			}
			pptr += 4;
		}

		glBindTexture(GL_TEXTURE_1D, xfer_tex);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, XFER_MAP_SZ, GL_RGBA, GL_FLOAT, pixels);

		xfer_tex_valid = true;
	}

	// make sure the proxy object is up to date
	if(proxy_count != vbo_proxy_count) {
		static const float pat[][3] = {{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};

		int nverts = proxy_count * 4;
		float *verts = new float[nverts * 3];
		float *vptr = verts;

		for(int i=0; i<proxy_count; i++) {
			float z = 2.0 * (float)i / (float)(proxy_count - 1) - 1.0;

			for(int j=0; j<4; j++) {
				*vptr++ = pat[j][0];
				*vptr++ = pat[j][1];
				*vptr++ = z;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, nverts * 3 * sizeof(float), verts, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		delete [] verts;
		vbo_proxy_count = proxy_count;
	}
}

void RendererFast::render() const
{
	if(!vol) return;

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(sdr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, vol_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, xfer_tex);

	set_uniform_int(sdr, "vol_tex", 0);
	set_uniform_int(sdr, "xfer_tex", 1);
	set_uniform_float(sdr, "zscale", zscale);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, vbo_proxy_count * 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDisable(GL_BLEND);

	glUseProgram(0);
}
