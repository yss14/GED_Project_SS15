// TerrainGenerator.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <string>
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	int resolution = 0;
	std::string pathHeightfield = "C:\\Users";
	std::string pathColor = "C:\\Users";
	std::string pathNormal = "C:\\Users";

	std::cout << argc <<std::endl;

	for (int i = 0; i < argc; i++)
	{
		std::wcout << argv[i] << std::endl;
	}

	if (argc != 9)
	{
		std::cerr << "Usage: -r <resolution> -o_height <output heightfield filename> -o_color <output color filename> -o_normal <output normal filename>" << std::endl;
		system("pause");
		return 1;
	}

	for (int i = 0; i < argc; i++)
	{
		std::wstring arg = argv[i];
		if (arg.compare("-r") == 0)
		{
			resolution = (int)argv[i + 1];
			i++;
			if (resolution <= 0)
			{
				std::cerr << "Usage: -r <resolution> -o_height <output heightfield filename> - o_color <output color filename> -o_normal <output normal filename>" << std::endl;
				std::cerr << "resolution must be > 0" << std::endl;
				system("pause");
				return 1;
			}
		}
		else if ()
	}
	
	system("pause");
	return 0;
}