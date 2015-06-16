#pragma once
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

struct MeshFiles{

	std::string modelPath;
	std::string diffuseTexturePath;
	std::string specularTexturePath;
	std::string glowTexturePath;
	std::string normalTexturePath;
};

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
	MeshFiles* getCockpitMeshFiles();
	std::map<std::string, MeshFiles*> meshPathes;

private:
	std::string terrainHeightPath, terrainColorPath, terrainNormalPath;
	std::string terrainPath;
	float terrainWidth;
	float terrainDepth;
	float terrainHeight;
	float spinSpeed;
	

};

