#ifndef XFERMAP_H_
#define XFERMAP_H_

class TransferFunc {
public:
	virtual ~TransferFunc();

	virtual float map(float x) const = 0;
	virtual void map(float x, float *rgba_value) const = 0;
};

class TransferWindow : public TransferFunc {
private:
	float soft_rad;
	float low[3], high[3];	// rgb

public:
	enum { HANDLE_LOW = 0, HANDLE_HIGH = 1};

	TransferWindow();

	// handle: 0 or HANDLE_LOW is low, 1 or HANDLE_HIGH is high
	// if channel == -1, change all channels simultaneously
	void set_handle(int channel, int handle, float val);
	float get_handle(int channel, int handle) const;

	int nearest_handle(int channel, float pos) const;

	void set_interval(float a, float b);
	void set_interval(float *rgba_low, float *rgba_high);
	void set_interval_rgba(int channel, float a, float b);

	void get_interval(float *aptr, float *bptr) const;
	void get_interval_rgba(float *rgba_low, float *rgba_high) const;
	void get_interval_rgba(int channel, float *aptr, float *bptr) const;

	void set_soft_radius(float s);
	float get_soft_radius() const;

	float map(float x) const;
	void map(float x, float *rgba_value) const;
};

#endif	// XFERMAP_H_
