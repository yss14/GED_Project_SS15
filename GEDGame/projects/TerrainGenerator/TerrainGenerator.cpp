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

	/*
	 * Unnecessary part of Assingment02
	 *

	float* heightMapData = new float[resolution * resolution];

	std::default_random_engine random(time(0));
	std::normal_distribution<float> distribution(0.5, 0.5);

	std::cout << "Generating random values for heightmap..." << std::endl;
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			float value = distribution(random);
			while (value > 1 || value < 0)
				value = distribution(random);

			heightMapData[IDX(x, y, resolution)] = value;
			//std::cout << imgData[IDX(x, y, resolution)] << std::endl;
		}
	}
	delete heightMapData; */


	// Play around with this values ;)
	float roughness = 0.6f; // "spikyness", the closer to zero, the flatter
	int smoothCount = 25;	// smoothCount times smoothed
	int smoothRange = 2;	// "Smoothing Radius" 

	GEDUtils::SimpleImage img(resolution, resolution);

	DiamondSquare ds(resolution, roughness, smoothCount, smoothRange);
	std::vector<float> pic = ds.doDiamondSquare();

	std::cout << "[Image] Writing image..." << std::endl;
	for (int x = 0; x < resolution; x++)
	{
		for (int y = 0; y < resolution; y++)
		{
			img.setPixel(x, y, pic[IDX(x, y, resolution + 1)]);
		}
	}
	std::cout << "[Image] Saving image..." << std::endl;
	try
	{
		//_TCHAR c[128];
		//swprintf(c, sizeof(c), L"%.1f_%s", roughness, pathHeightfield);
		img.save(pathHeightfield);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}

	std::cout << "[Image] Generating color&normal..." << std::endl;
	GEDUtils::TextureGenerator tg(std::wstring(L"../../../../external/textures/gras15.jpg"),
		std::wstring(L"../../../../external/textures/ground02.jpg"),
		std::wstring(L"../../../../external/textures/pebble03.jpg"),
		std::wstring(L"../../../../external/textures/rock5.jpg"));

	tg.generateAndStoreImages(pic, resolution, pathColor, pathNormal);
	
	system("pause");
	return 0;
}