#include <algorithm>
#include "xfermap.h"

TransferFunc::~TransferFunc()
{
}


// ---- TransferWindow ----

TransferWindow::TransferWindow()
{
	soft_rad = 0.5;
	for(int i=0; i<4; i++) {
		low[i] = 0.5;
		high[i] = 2.0;
	}
}

void TransferWindow::set_interval(float a, float b)
{
	float v0 = std::min(a, b);
	float v1 = std::max(a, b);

	for(int i=0; i<4; i++) {
		low[i] = v0;
		high[i] = v1;
	}
}

void TransferWindow::set_interval(float *rgba_low, float *rgba_high)
{
	for(int i=0; i<4; i++) {
		low[i] = std::min(rgba_low[i], rgba_high[i]);
		high[i] = std::max(rgba_low[i], rgba_high[i]);
	}
}

void TransferWindow::set_interval_rgba(int channel, float a, float b)
{
	low[channel] = std::min(a, b);
	high[channel] = std::max(a, b);
}

void TransferWindow::set_soft_radius(float s)
{
	soft_rad = s;
}

float TransferWindow::get_soft_radius() const
{
	return soft_rad;
}

static inline float smoothstep(float a, float b, float x)
{
	if(x < a) return 0.0f;
	if(x >= b) return 1.0f;

	float t = (x - a) / (b - a);
	return t * t * (3.0f - 2.0f * t);
}

float TransferWindow::map(float x) const
{
	return smoothstep(low[3] - soft_rad, high[3] - soft_rad, x) *
		(1.0 - smoothstep(low[3] + soft_rad, high[3] + soft_rad, x));
}

void TransferWindow::map(float x, float *rgba_value) const
{
	for(int i=0; i<4; i++) {
		float val = smoothstep(low[i] - soft_rad, high[i] - soft_rad, x);
		val *= 1.0 - smoothstep(low[i] + soft_rad, high[i] + soft_rad, x);
		rgba_value[i] = val;
	}
}
