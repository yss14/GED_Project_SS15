#include "stdafx.h"
#include "TextureGenerator.h"

//Man ist das hässlich xD
TextureGenerator::TextureGenerator(const std::wstring& texturePathLowFlat,
	const std::wstring& texturePathLowSteep,
	const std::wstring& texturePathHighFlat,
	const std::wstring& texturePathHighSteep
	):
	texturePathHighFlat(texturePathHighFlat),
	texturePathHighSteep(texturePathHighSteep),
	texturePathLowFlat(texturePathLowFlat),
	texturePathLowSteep(texturePathLowSteep)
{
	
}


TextureGenerator::~TextureGenerator()
{
}

void TextureGenerator::generateNormals(const std::vector<float>& heightfield, int resolution, _TCHAR* path, std::vector<Vec3f>& normals)
{
	//Init 3 vectors to save memory while runtime
	Vec3f normalAtCurPoint(0.0f, 0.0f, 0.0f);
	Vec3f vecXAxes(0.0f, 0.0f, 0.0f);
	Vec3f vecYAxes(0.0f, 0.0f, 0.0f);

	GEDUtils::SimpleImage normalImage(resolution, resolution);

	int heightMapRes = sqrt(heightfield.size());

	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			normalAtCurPoint.x = normalAtCurPoint.y = normalAtCurPoint.z = 0;
			vecXAxes.x = 1.0f;
			vecXAxes.y = 0.0f;
			vecYAxes.x = 0.0f;
			vecYAxes.y = 1.0f;

			//Central Differecnes
			if (x == 0)
			{
				//Forward Differences
				vecXAxes.z = (heightfield[IDX(x + 1, y, heightMapRes)] - heightfield[IDX(x, y, heightMapRes)]);
			}
			else if (x == heightMapRes - 1)
			{
				//Backward Differences
				vecXAxes.z = (heightfield[IDX(x, y, heightMapRes)] - heightfield[IDX(x - 1, y, heightMapRes)]);
			}
			else
			{
				//Central Differences
				vecXAxes.z = (heightfield[IDX(x + 1, y, heightMapRes)] - heightfield[IDX(x - 1, y, heightMapRes)]) / 2.0f;
			}
			
			if (y == 0)
			{
				//Forward Differences
				vecYAxes.z = (heightfield[IDX(x, y + 1, heightMapRes)] - heightfield[IDX(x, y, heightMapRes)]);
			}
			else if (y == heightMapRes - 1)
			{
				//Backward Differences
				vecYAxes.z = (heightfield[IDX(x, y, heightMapRes)] - heightfield[IDX(x, y - 1, heightMapRes)]);
			}
			else
			{
				//Central Differences
				vecYAxes.z = (heightfield[IDX(x, y + 1, heightMapRes)] - heightfield[IDX(x, y - 1, heightMapRes)]) / 2.0f;
			}

			normalAtCurPoint = vecXAxes.cross(vecYAxes);

			//Rescaling because of different dimensions
			normalAtCurPoint.x *= resolution;
			normalAtCurPoint.y *= resolution;

			normalAtCurPoint.normalize();

			normalImage.setPixel(x, y, (normalAtCurPoint.x/2.0f)+0.5f, (normalAtCurPoint.y/2.0f)+0.5f, (normalAtCurPoint.z/2.0f)+0.5f);

			normals.push_back(normalAtCurPoint);
		}
	}

	std::cout << "[Image] Saving normal image..." << std::endl;

	try
	{
		normalImage.save(path);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
}

void TextureGenerator::generateColors(const std::vector<float>& heightfield, const std::vector<Vec3f>& normals, int resolution,
	std::vector<Color4f>& colorsOut, _TCHAR* path)
{

	Texture textureLowFlat(this->texturePathLowFlat);
	Texture textureLowSteep(this->texturePathLowSteep);
	Texture textureHightFlat(this->texturePathHighFlat);
	Texture textureHighSteep(this->texturePathHighSteep);

	GEDUtils::SimpleImage colorImage(resolution, resolution);

	int heightMapRes = sqrt(heightfield.size());
	int normalMapRes = sqrt(normals.size());

	float alpha1 = 0.0f;
	float alpha2 = 0.0f;
	float alpha3 = 0.0f;

	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			this->calcAlphas(heightfield[IDX(x, y, heightMapRes)], 1.0f - normals[IDX(x, y, normalMapRes)].z, alpha1, alpha2, alpha3);
			
			Color4f c0 = textureLowFlat.getColorTiled(x, y); //ColorLowFlat
			Color4f c1 = textureLowSteep.getColorTiled(x, y); //ColorLowSteep
			Color4f c2 = textureHightFlat.getColorTiled(x, y); //ColorHighFlat
			Color4f c3 = textureHighSteep.getColorTiled(x, y); //ColorHighSteep

			Color4f finalColor = this->calcColor(c0, c1, c2, c3, alpha1, alpha2, alpha3);

			colorImage.setPixel(x, y, finalColor.r, finalColor.g, finalColor.b);
		}
	}

	std::cout << "[Image] Saving normal image own..." << std::endl;
	try
	{
		//TODO fixing parameter path
		colorImage.save(path);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
}

void TextureGenerator::generateAndStoreImages(std::vector<float> heightmap, int resolution, _TCHAR* normalPath,
	_TCHAR* colorPath)
{
	std::vector<Vec3f> normals;
	std::vector<Color4f>colors;

	this->generateNormals(heightmap, resolution, normalPath, normals);
	this->generateColors(heightmap, normals, resolution, colors, colorPath);
}

void TextureGenerator::calcAlphas(float height, float slope, float& alpha1, float& alpha2, float& alpha3){
	alpha1 = (1 - height) * slope;
	alpha2 = height;
	//alpha3 = height*slope;

	//Improving sharpnes at high slopes
	alpha3 = -2.08333333f * pow(slope, 3) + 3.125f * pow(slope, 2) + -0.0416666f * slope;

	float sum = alpha1 + alpha2 + alpha3;

	if (sum > 1.0f){
		alpha1 -= (sum - 1.0f) / 2.0f;
		alpha2 -= ((sum - 1.0f)*height) / 2.0f;
	}
}

Color4f TextureGenerator::calcColor(Color4f c0, Color4f c1, Color4f c2, Color4f c3, float alpha1, float alpha2, float alpha3){
	float r = alpha3 * c3.r + (1.0f - alpha3) * (alpha2 * c2.r + (1.0f - alpha2) * (alpha1 * c1.r + (1.0f - alpha1) * c0.r));
	float g = alpha3 * c3.g + (1.0f - alpha3) * (alpha2 * c2.g + (1.0f - alpha2) * (alpha1 * c1.g + (1.0f - alpha1) * c0.g));
	float b = alpha3 * c3.b + (1.0f - alpha3) * (alpha2 * c2.b + (1.0f - alpha2) * (alpha1 * c1.b + (1.0f - alpha1) * c0.b));

	Color4f finalColor(r, g, b, 1.0f);

	return finalColor;
}

void TextureGenerator::sampleHeightfieldDown(std::vector<float>& heightfield, int& newResolution){
	const int downSamplingFactor = 4;
	std::vector<float> newHeightMap;
	int oldResolution = sqrt(heightfield.size());
	newResolution = oldResolution / downSamplingFactor;

	for (int y = 0; y < newResolution; y++)
	{
		for (int x = 0; x < newResolution; x++)
		{
			float sum = heightfield[IDX(x * downSamplingFactor, y * downSamplingFactor, oldResolution)] +
				heightfield[IDX(x * downSamplingFactor + 1, y * downSamplingFactor + 1, oldResolution)] +
				heightfield[IDX(x * downSamplingFactor + 2, y * downSamplingFactor + 2, oldResolution)] +
				heightfield[IDX(x * downSamplingFactor + 3, y * downSamplingFactor + 3, oldResolution)];

			newHeightMap.push_back(sum / 4.0f);

		}
	}

	heightfield = newHeightMap;
}
