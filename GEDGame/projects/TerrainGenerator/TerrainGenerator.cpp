// TerrainGenerator.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <SimpleImage.h>
#include <TextureGenerator.h>

#define IDX(x, y, w)((x) + (y) * (w))

int _tmain(int argc, _TCHAR* argv[])
{
	int resolution;
	_TCHAR* pathHeightfield = L"";
	_TCHAR* pathColor = L"";
	_TCHAR* pathNormal = L"";

	/*std::cout << argc <<std::endl;

	for (int i = 0; i < argc; i++)
	{
		std::wcout << argv[i] << std::endl;
	}*/

	if (argc != 9)
	{
		std::cerr << "Usage: -r <resolution> -o_height <output heightfield filename> -o_color <output color filename> -o_normal <output normal filename>" << std::endl;
		system("pause");
		return 1;
	}

	for (int i = 0; i < argc; i++)
	{
		std::wstring arg(argv[i]);
		if (arg == L"-r")
		{
			resolution = _tstoi(argv[i + 1]);
			i++;
			if (resolution <= 0)
			{
				std::cerr << "Usage: -r <resolution> -o_height <output heightfield filename> -o_color <output color filename> -o_normal <output normal filename>" << std::endl;
				std::cerr << "resolution must be > 0" << std::endl;
				system("pause");
				return 1;
			}
		}
		else if (arg == L"-o_height")
		{
			pathHeightfield = argv[i + 1];
			i++;
		}
		else if (arg == L"-o_color")
		{
			pathColor = argv[i + 1];
			i++;
		}
		else if (arg == L"-o_normal")
		{
			pathNormal = argv[i + 1];
			i++;
		}
	}

	float* heightMapData = new float[resolution * resolution];

	std::srand(std::time(0));

	std::cout << "Generating random values for heightmap..." << std::endl;
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			heightMapData[IDX(x, y, resolution)] = (std::rand() % 1001) / 1000.0f;
			//std::cout << imgData[IDX(x, y, resolution)] << std::endl;
		}
	}

	GEDUtils::SimpleImage img(resolution, resolution);

	std::cout << "Writing image..." << std::endl;
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			img.setPixel(x, y, heightMapData[IDX(x, y, resolution)]);
		}
	}
	std::cout << "Saving image..." << std::endl;
	img.save(pathHeightfield);

	//GEDUtils::TextureGenerator::generateAndStoreImages(heightMapData, resolution, pathColor, pathNormal);

	delete heightMapData;
	
	
	system("pause");
	return 0;
}