#include "stdafx.h"
#include "TextureGenerator.h"


TextureGenerator::TextureGenerator(const std::wstring& texturePathLowFlat,
	const std::wstring& texturePathLowSteep,
	const std::wstring& texturePathHighFlat,
	const std::wstring& texturePathHighSteep
	)
{

}


TextureGenerator::~TextureGenerator()
{
}

void TextureGenerator::generateNormals(const std::vector<float>& heightfield, int resolution, _TCHAR* path)
{

	//std::cout << "Resolution: " << resolution << "\n";

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
			
			if (x == resolution - 1){
				//Special treatment for border values concerning x-axes
			}
			else if (y == resolution - 1){
				//Special treatment for border values concerning x-axes
			}
			else{
				//Normal treatment
				vecXAxes.z = (heightfield[IDX(x + 1, y, resolution)] - heightfield[IDX(x, y, resolution)]) / (1.0f);
				vecYAxes.z = (heightfield[IDX(x, y + 1, resolution)] - heightfield[IDX(x, y, resolution)]) / (1.0f);
				//std::cout << "Calc: " << heightfield[IDX(x, y + 1, resolution)] << " - " << heightfield[IDX(x, y, resolution)] << "\n";
			}

			std::cout << "X-Vector: " << vecXAxes.printVec() << "\n";
			std::cout << "Y-Vector: " << vecYAxes.printVec() << "\n";

			normalAtCurPoint = vecXAxes.cross(vecYAxes);
			std::cout << "Normal: " << normalAtCurPoint.printVec() << "\n";
			normalAtCurPoint.normalize();

			std::cout << "Normalized normal: " << normalAtCurPoint.printVec() << "\n\n";

			normalImage.setPixel(x, y, (normalAtCurPoint.x/2.0f)+0.5f, (normalAtCurPoint.y/2.0f)+0.5f, normalAtCurPoint.z);
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

void TextureGenerator::generateAndStoreImages(std::vector<float> heightmap, int resolution, _TCHAR* normalPath,
	_TCHAR* colorPath)
{
	this->generateNormals(heightmap, resolution, normalPath);
}
