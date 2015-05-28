#pragma once

#include <math.h>
#include <string>
#include <sstream>
#include <iomanip>

class Vec3f
{
public:
	Vec3f(float x, float y, float z);
	~Vec3f();

	Vec3f cross(Vec3f otherVec);
	void normalize();
	float getMagnitude();
	std::string printVec();

	float x;
	float y;
	float z;
};

