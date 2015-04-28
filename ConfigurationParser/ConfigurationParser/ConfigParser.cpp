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

			if (firstWord.compare(std::string("spinning")) == 0){
				sstr << readValue;
				sstr >> this->spinning;
			}else if (firstWord.compare(std::string("spinSpeed")) == 0){
				sstr << readValue;
				sstr >> this->spinSpeed;
			}else if(firstWord.compare(std::string("backgroundColor")) == 0){
				sstr << readValue;
				sstr >> this->backgroundColor.r;

				ifs >> readValue;
				sstr << readValue;
				sstr >> this->backgroundColor.g;

				ifs >> readValue;
				sstr << readValue;
				sstr >> this->backgroundColor.b;
			}else if (firstWord.compare(std::string("terrainWidth")) == 0){
				sstr << readValue;
				sstr >> this->terrainWidth;
			}else if (firstWord.compare(std::string("terrainDepth")) == 0){
				sstr << readValue;
				sstr >> this->terrainDepth;
			}else if (firstWord.compare(std::string("terrainHeight")) == 0){
				sstr << readValue;
				sstr >> this->terrainHeight;
			}else if (firstWord.compare(std::string("terrainPath")) == 0){
				this->terrainPath = readValue;
			}else{
				std::cout << "Can't read property >" << firstWord << "< from file " << filepath;
			}
		}
	}

	ifs.close();
	std::cout << "Loaded " << filepath << "\n";
}

float ConfigParser::getSpinning(){
	return this->spinning;
}

float ConfigParser::getSpinSpeed(){
	return this->spinSpeed;
}

ConfigParser::Color ConfigParser::getBackgroundColor(){
	return this->backgroundColor;
}

std::string ConfigParser::getTerrainPath(){
	return this->terrainPath;
}

float ConfigParser::getTerrainWidth(){
	return this->terrainWidth;
}

float ConfigParser::getTerrainDepth(){
	return this->terrainDepth;
}

float ConfigParser::getTerrainHeight(){
	return this->terrainHeight;
}
