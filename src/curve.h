#ifndef CURVE_H_
#define CURVE_H_

#include <vector>
#include <inttypes.h>

struct CurvePoint {
	uint16_t t_int;	// save time as an integer to allow exact lookup and change
	float value;
};

class Curve {
private:
	std::vector<CurvePoint> cp;

public:
	void set_point(float t, float val);
	void set_point_int(uint16_t ti, float val);

	bool delete_point(uint16_t ti);

	CurvePoint *get_point(int idx);
	const CurvePoint *get_point(int idx) const;
	int get_num_points() const;

	CurvePoint *get_point_at(uint16_t ti);
	const CurvePoint *get_point_at(uint16_t ti) const;

	float value(float t) const;
	float value_int(uint16_t ti) const;
};

#endif	// CURVE_H_
