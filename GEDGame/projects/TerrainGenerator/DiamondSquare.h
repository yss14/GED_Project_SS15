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
	float roughnessIteration;
	std::vector<float> field;
	std::default_random_engine randomizer;

	float getRandom(float min, float max);
	void diamondStep(int x, int y, int s, float ri);
	void squareStep(int x, int y, int s, float ri);
	void normalizeHeightField();
	void smoothHeightField();
};

