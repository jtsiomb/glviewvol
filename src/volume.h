#ifndef VOLUME_H_
#define VOLUME_H_

#include <vector>
#include "image.h"

class Volume {
public:
	virtual ~Volume();

	// returns 0 if the volume is continuously defined
	virtual int num_samples(int dim) const;

	// central differences offset will be delta / num_samples for discrete volumes
	// and delta for continuous volumes
	virtual void normalf(float *norm, float x, float y, float z, float delta = 1.0);
	virtual void normali(float *norm, int x, int y, int z);

	virtual float valuef(float x, float y, float z) const = 0;
	virtual float valuei(int x, int y, int z) const = 0;
};

class VoxelVolume : public Volume {
protected:
	int size[3];
	std::vector<Image> slices;

public:
	VoxelVolume();
	~VoxelVolume();

	bool load(const char *fname);

	int num_samples(int dim) const;

	float valuef(float x, float y, float z) const;
	float valuei(int x, int y, int z) const;
};

#endif	// VOLUME_H_
