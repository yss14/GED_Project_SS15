// Toms Vector feature
#include "Vec3f.h"

// Constructor
// Pass values for vectors x,y,z
Vec3f::Vec3f(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

// Destructor
Vec3f::~Vec3f()
{
	// TODO
}

// calculate the cross pproduct
// returns a new resulting vector
Vec3f Vec3f::cross(Vec3f otherVec)
{
	Vec3f solVec(0.0f, 0.0f, 0.0f);

	solVec.x = (this->y * otherVec.z) - (this->z * otherVec.y);
	solVec.y = (this->z * otherVec.x) - (this->x * otherVec.z);
	solVec.z = (this->x * otherVec.y) - (this->y * otherVec.x);

	return solVec;
}

float Vec3f::getMagnitude()
{
	return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}

void Vec3f::normalize()
{
	float tmpMag = getMagnitude();

	this->x /= tmpMag;
	this->y /= tmpMag;
	this->z /= tmpMag;
}