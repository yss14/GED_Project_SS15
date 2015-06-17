#include "ConfigParser.h"


ConfigParser::ConfigParser(){
	objectsData;
}


ConfigParser::~ConfigParser(){
	for (auto iterator = meshPathes.begin(); iterator != meshPathes.end(); iterator++) {
		delete iterator->second;
	}
}

void ConfigParser::load(std::string filepath){
	std::ifstream ifs(filepath);

	if (ifs.is_open()){
		std::string firstWord;
		while (ifs >> firstWord){
			std::string readValue;
			ifs >> readValue;
			std::stringstream sstr;

			if (firstWord.compare(std::string("TerrainPath")) == 0){
				terrainHeightPath = readValue;
				ifs >> this -> terrainColorPath;
				ifs >> this -> terrainNormalPath;
			}else if (firstWord.compare(std::string("spinSpeed")) == 0){
				sstr << readValue;
				sstr >> this->spinSpeed;
			}else if (firstWord.compare(std::string("TerrainWidth")) == 0){
				sstr << readValue;
				sstr >> this->terrainWidth;
			}else if (firstWord.compare(std::string("TerrainDepth")) == 0){
				sstr << readValue;
				sstr >> this->terrainDepth;
			}else if (firstWord.compare(std::string("TerrainHeight")) == 0){
				sstr << readValue;
				sstr >> this->terrainHeight;
			}
			else if (firstWord.compare(std::string("Mesh")) == 0){
				std::string tmpModelName = readValue;
				MeshFiles *meshFilePaths = new MeshFiles();
				ifs >> meshFilePaths->modelPath;
				ifs >> meshFilePaths->diffuseTexturePath;
				ifs >> meshFilePaths->specularTexturePath;
				ifs >> meshFilePaths->glowTexturePath;
				ifs >> meshFilePaths->normalTexturePath;
				meshPathes[readValue] = meshFilePaths;
			}
			else if (firstWord.compare(std::string("CockpitObject")) || firstWord.compare(std::string("GroundObject"))){
				TransformData *tmpData = new TransformData();
				tmpData->type = firstWord;
				tmpData->name = readValue;
				ifs >> tmpData->scale;
				ifs >> tmpData->rotX;
				ifs >> tmpData->rotY;
				ifs >> tmpData->rotZ;

				ifs >> tmpData->posX;
				ifs >> tmpData->posY;
				ifs >> tmpData->posZ;

				objectsData.push_back(tmpData);

			}else{
				std::cout << "Can't read property >" << firstWord << "< from file " << filepath;
			}
		}
	}

	ifs.close();
	std::cout << "Loaded " << filepath << "\n";
}

float ConfigParser::getSpinning(){
	return this->spinSpeed;
}

float ConfigParser::getSpinSpeed(){
	return this->spinSpeed;
}

std::string ConfigParser::getTerrainPath(){
	return this->terrainHeightPath;
}

float ConfigParser::getTerrainWidth(){
	return this->terrainWidth;
}

float ConfigParser::getTerrainDepth(){
	return this->terrainDepth;
}

std::string ConfigParser::getTerrainTexturePath(){
	return this->terrainColorPath;
}

std::string ConfigParser::getTerrainNormalPath(){
	return this->terrainNormalPath;
}

float ConfigParser::getTerrainHeight(){
	return this->terrainHeight;
}
