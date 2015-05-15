#pragma once
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

class ConfigParser{
public:
	ConfigParser();
	~ConfigParser();

	struct Color{
		float r;
		float g;
		float b;
	};

	void load(std::string filepath);

	//Getter
	float getSpinning();
	float getSpinSpeed();
	ConfigParser::Color getBackgroundColor();
	std::string getTerrainPath();
	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();

private:
	std::string terrainHeightPath, terrainColorPath, terrainNormalPath;
	Color backgroundColor;
	std::string terrainPath;
	float terrainWidth;
	float terrainDepth;
	float terrainHeight;
	float spinSpeed;
};

