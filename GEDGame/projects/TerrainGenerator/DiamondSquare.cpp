#include "stdafx.h"
#include "DiamondSquare.h"

#define IDX(x, y, w)((x) + (y) * (w))

DiamondSquare::DiamondSquare(int inputResolution, float roughness, int smoothCount, int smoothRange):
field((inputResolution % 2 == 0 ? inputResolution + 1 : inputResolution)*(inputResolution % 2 == 0 ? inputResolution + 1 : inputResolution))
{
	this->calcResolution = inputResolution % 2 == 0 ? inputResolution + 1 : inputResolution; // +1 because (2^n + 1)^2
	this->inputResolution = inputResolution;
	this->roughness = roughness;
	this->roughnessIteration = roughness;
	this->smoothCount = smoothCount;
	this->smoothRange = smoothRange;
	this->randomizer = std::default_random_engine(42);
}


DiamondSquare::~DiamondSquare()
{
}

std::vector<float> DiamondSquare::doDiamondSquare()
{
	std::cout << "[DiamondSquare] Creating Heightmap..." << std::endl;
	// start-values
	field[IDX(0, 0, calcResolution)] = getRandom(0, 1);
	field[IDX(calcResolution - 1, 0, calcResolution)] = getRandom(0, 1);
	field[IDX(0, calcResolution - 1, calcResolution)] = getRandom(0, 1);
	field[IDX(calcResolution - 1, calcResolution - 1, calcResolution)] = getRandom(0, 1);

	int res = this->calcResolution-1;

	for (int step = res; step >= 2; step /= 2) 
	{
		int halfstep = step / 2;

		for (int y = halfstep; y < res; y += step)
		{
			for (int x = halfstep; x < res; x += step)
			{
				diamondStep(x, y, halfstep, roughnessIteration);
			}
		}

		for (int y = 0; y <= res; y += step)
		{
			for (int x = halfstep; x < res; x += step)
			{
				squareStep(x, y, halfstep, roughnessIteration);
			}
		}

		for (int y = halfstep; y < res; y += step)
		{
			for (int x = 0; x <= res; x += step)
			{
				squareStep(x, y, halfstep, roughnessIteration);
			}
		}

		roughnessIteration *= roughness;
	}

	normalizeHeightField();

	if (this->inputResolution != this->calcResolution)
	{
		std::vector<float> cuttedField(inputResolution * inputResolution);

		for (int y = 0; y < this->inputResolution; y++)
		{
			for (int x = 0; x < this->inputResolution; x++)
			{
				cuttedField[IDX(x, y, this->inputResolution)] = field[IDX(x, y, this->calcResolution)];
			}
		}
		field = cuttedField;
	}

	std::cout << "[DiamondSquare] Smoothing Heightmap..." << std::endl;

	for (int i = 0; i < smoothCount; i++)
	{
		smoothHeightField();
	}

	return field;
}

void DiamondSquare::normalizeHeightField()
{
	std::cout << "[DiamondSquare] Normalizing Heightmap..." << std::endl;

	float min = FLT_MAX;
	float max = -FLT_MAX;

	for (float val : field)
	{
		if (val < min)
		{
			min = val;
		}

		if (val > max)
		{
			max = val;
		}
	}

	float diff = max - min;

	for (float &val : field)
	{
		val = (val - min) / diff;
	}
}

void DiamondSquare::smoothHeightField()
/*
* Smoothes the Vector, by averaging
*/
{
	std::vector<float> tempHeightField(inputResolution*inputResolution);

	for (int x = 0; x < inputResolution; x++)
	{
		for (int y = 0; y < inputResolution; y++)
		{
			float sum = 0.0f;
			int count = 0;

			for (int i = -smoothRange; i <= smoothRange; i++)
			{
				int tmp = x + i;

				if (tmp >= 0 && tmp < inputResolution)
				{
					sum += field[IDX(tmp, y, inputResolution)];
					count++;
				}
			}

			tempHeightField[IDX(x, y, inputResolution)] = sum / count;
		}
	}

	for (int x = 0; x < inputResolution; x++)
	{
		for (int y = 0; y < inputResolution; y++)
		{
			float sum = 0.0f;
			int count = 0;

			for (int j = -smoothRange; j <= smoothRange; j++)
			{
				int tmp = y + j;

				if (tmp >= 0 && tmp < inputResolution)
				{
					sum += tempHeightField[IDX(x, tmp, inputResolution)];
					count++;
				}
			}

			field[IDX(x, y, inputResolution)] = sum / count;
		}
	}
}


void DiamondSquare::diamondStep(int x, int y, int s, float roughnessIteration)
/*
* "Diamon-step" of the DiamondSquare-Algorythm
*/
{
	field[IDX(x, y, calcResolution)] = (
		field[IDX(x - s, y - s, calcResolution)] +
		field[IDX(x + s, y - s, calcResolution)] +
		field[IDX(x - s, y + s, calcResolution)] +
		field[IDX(x + s, y + s, calcResolution)]
		) / 4;

	field[IDX(x, y, calcResolution)] += getRandom(-1, 1) * roughnessIteration;
}

void DiamondSquare::squareStep(int x, int y, int s, float roughnessIteration)
/*
 * "Square-step" of the DiamondSquare-Algorythm
*/
{
	int counter = 0;
	float sum = 0;

	if (x - s >= 0)
	{
		sum += field[IDX(x - s, y, calcResolution)];
		counter++;
	}

	if (y - s >= 0)
	{
		sum += field[IDX(x, y - s, calcResolution)];
		counter++;
	}

	if (x + s < calcResolution)
	{
		sum += field[IDX(x + s, y, calcResolution)];
		counter++;
	}

	if (y + s < calcResolution)
	{
		sum += field[IDX(x, y + s, calcResolution)];
		counter++;
	}

	field[IDX(x, y, calcResolution)] = sum / counter;
	field[IDX(x, y, calcResolution)] += getRandom(-1, 1) * roughnessIteration;
}

float DiamondSquare::getRandom(float min, float max)
/*
 * Gets a random value between min and max
*/
{
	float center = (max - min) / 2.0f;

	std::normal_distribution<float> distribution(center, (max - min));
	float value = distribution(this->randomizer);

	while (value > max || value < min)
	{
		value = distribution(this->randomizer);
	}

	return value;
}

void DiamondSquare::printField()
{
	for (int y = 0; y < calcResolution; y++)
	{
		for (int x = 0; x < calcResolution; x++)
		{
			printf("%.6f\t", field[IDX(x, y, calcResolution)]);
		}

		std::cout << std::endl;
	}

	std::cout << std::endl;
}