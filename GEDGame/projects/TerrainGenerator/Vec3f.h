#pragma once

class Vec3f
{
public:
	Vec3f(float x, float y, float z);
	~Vec3f();

	Vec3f multiply(Vec3f factorVec);
	void multiply(float scalar);
	void normalize();

	float x;
	float y;
	float z;
};

