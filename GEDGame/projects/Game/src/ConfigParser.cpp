#include "ConfigParser.h"


ConfigParser::ConfigParser(){

}


ConfigParser::~ConfigParser(){

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
				

				if (readValue.compare(std::string("Cockpit")) == 0){
					ifs >> cockpitModelFiles.modelPath;
					ifs >> cockpitModelFiles.diffuseTexturePath;
					ifs >> cockpitModelFiles.specularTexturePath;
					ifs >> cockpitModelFiles.glowTexturePath;
				}
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

MeshFiles* ConfigParser::getCockpitMeshFiles(){
	return &(this->cockpitModelFiles);
}
