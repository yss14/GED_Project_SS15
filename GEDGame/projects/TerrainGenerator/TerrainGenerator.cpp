// TerrainGenerator.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include "stdafx.h"
#include <cstdlib>
#include <string>
#include <random>
#include <iostream>
#include <ctime>
#include <SimpleImage.h>
#include <TextureGenerator.h>
#include "DiamondSquare.h"
#include "TextureGenerator.h"

#include "Vec3f.h"

#define IDX(x, y, w)((x) + (y) * (w))

int _tmain(int argc, _TCHAR* argv[])
{


	int resolution;
	_TCHAR* pathHeightfield = L"";
	_TCHAR* pathColor = L"";
	_TCHAR* pathNormal = L"";

	// Invalid parameters
	if (argc != 9)
	{
		std::cerr << "Usage: -r <resolution> -o_height <output heightfield filename> -o_color <output color filename> -o_normal <output normal filename>" << std::endl;
		system("pause");
		return 1;
	}

	// Read parameters
	for (int i = 0; i < argc; i++)
	{
		std::wstring arg(argv[i]);
		if (arg == L"-r")
		{
			resolution = _tstoi(argv[i + 1]);
			i++;
			if (resolution <= 0) // Invalid resolution
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

	// Play around with these values ;)
	float roughness = 0.52f; // "spikyness", the closer to zero, the flatter
	int smoothCount = 4;	 // smoothCount times smoothed
	int smoothRange = 2;	 // "Smoothing Radius" 

	DiamondSquare ds(resolution, roughness, smoothCount, smoothRange);
	std::vector<float> heightmap = ds.doDiamondSquare();

	std::cout << "[Image] Generating color&normal..." << std::endl;

	//Own implementation
	TextureGenerator tg_own(std::wstring(L"../../../../external/textures/gras15.jpg"),
		std::wstring(L"../../../../external/textures/ground02.jpg"),
		std::wstring(L"../../../../external/textures/pebble03.jpg"),
		std::wstring(L"../../../../external/textures/rock5.jpg"));
	
	tg_own.generateAndStoreImages(heightmap, resolution, pathNormal, pathColor);
	std::cout << "[Image] Downsampling heightfield..." << std::endl;
	tg_own.sampleHeightfieldDown(heightmap, resolution);

	std::cout << "[Image] Saving heightfield..." << std::endl;
	GEDUtils::SimpleImage heightImage(resolution, resolution);
	for (int y = 0; y < resolution; y++)
	{
		for (int x = 0; x < resolution; x++)
		{
			heightImage.setPixel(x, y, heightmap[IDX(x, y, resolution)]);
		}
	}

	try
	{
		heightImage.save(pathHeightfield);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}

	system("pause");
	
	return 0;
}