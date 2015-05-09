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

void TextureGenerator::generateNormals(const std::vector<float>& heightfield, int resolution, _TCHAR* path, std::vector<Vec3f> normals)
{
	//Init 3 vectors to save memory while runtime
	Vec3f normalAtCurPoint(0.0f, 0.0f, 0.0f);
	Vec3f vecXAxes(0.0f, 0.0f, 0.0f);
	Vec3f vecYAxes(0.0f, 0.0f, 0.0f);

	GEDUtils::SimpleImage normalImage(resolution, resolution);

	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			normalAtCurPoint.x = normalAtCurPoint.y = normalAtCurPoint.z = 0;
			vecXAxes.x = 1.0f;
			vecXAxes.y = 0.0f;
			vecYAxes.x = 0.0f;
			vecYAxes.y = 1.0f;

			int heightMapRes = sqrt(heightfield.size());

			//Central Differecnes
			if (x == 0)
			{
				//Forward Differences
				vecXAxes.z = (heightfield[IDX(x + 1, y, heightMapRes)] - heightfield[IDX(x, y, heightMapRes)]);
			}
			else if (x == heightMapRes - 2)
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
			else if (y == heightMapRes - 2)
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

			normals.insert(normals.end(), normalAtCurPoint);
		}
	}


	std::cout << "[Image] Saving normal image..." << std::endl;
	try
	{
		std::string nPath("own_normal.png");
		normalImage.save(nPath.c_str());
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
}

void TextureGenerator::generateColors(const std::vector<float>& heightfield, const std::vector<Vec3f>& normals, int resolution,
	std::vector<Color4f>& colorsOut)
{
	Texture textureLowFlat(this->texturePathLowFlat);
	Texture textureLowSteeo(this->texturePathLowSteep);
	Texture textureHightFlat(this->texturePathHighFlat);
	Texture textureHighSteep(this->texturePathHighSteep);
}

void TextureGenerator::generateAndStoreImages(std::vector<float> heightmap, int resolution, _TCHAR* normalPath,
	_TCHAR* colorPath)
{
	std::vector<Vec3f> normals;

	this->generateNormals(heightmap, resolution, normalPath, normals);
}
