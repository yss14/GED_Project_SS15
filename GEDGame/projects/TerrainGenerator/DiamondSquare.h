#pragma once
#include <vector>
#include <iostream>
#include <random>
#include <time.h>
//#include <vector>

class DiamondSquare
{
public:
	DiamondSquare(int resolution, float roughness, int smoothCount, int smoothRange);
	~DiamondSquare();
	std::vector<float> doDiamondSquare();
	void printField();

private:
	int resolution;
	float roughness;
	int smoothCount;
	int smoothRange;
	float ri;
	std::vector<float> field;
	std::default_random_engine randomizer;

	float getRandom(float max, float min);
	void diamond(int x, int y, int s, float ri);
	void square(int x, int y, int s, float ri);
	void normalize();
	void smooth();
};

