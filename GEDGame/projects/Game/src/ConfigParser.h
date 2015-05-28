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

	void load(std::string filepath);

	//Getter
	float getSpinning();
	float getSpinSpeed();
	std::string getTerrainPath();
	float getTerrainWidth();
	float getTerrainDepth();
	float getTerrainHeight();
	std::string getTerrainTexturePath();
	std::string getTerrainNormalPath();

private:
	std::string terrainHeightPath, terrainColorPath, terrainNormalPath;
	std::string terrainPath;
	float terrainWidth;
	float terrainDepth;
	float terrainHeight;
	float spinSpeed;

};

