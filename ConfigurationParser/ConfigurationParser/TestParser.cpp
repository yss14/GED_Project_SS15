#pragma once

#include <istream>
#include "ConfigParser.h"
#include <string>

int main(){

	ConfigParser *cfgParser = new ConfigParser();


	cfgParser->load(std::string("game.cfg"));

	//Test output
	std::cout << "Spinning: " << cfgParser->getSpinning() << "\n";
	std::cout << "SpinSpeed: " << cfgParser->getSpinSpeed() << "\n";
	std::cout << "BackgroundColor: rgb(" << cfgParser->getBackgroundColor().r << ",";
	std::cout << cfgParser->getBackgroundColor().g << ",";
	std::cout << cfgParser->getBackgroundColor().b << ")\n";
	std::cout << "TerrainPath: " << cfgParser->getTerrainPath() << "\n";
	std::cout << "TerrainWidth: " << cfgParser->getTerrainWidth() << "\n";
	std::cout << "TerrainHeight: " << cfgParser->getTerrainHeight() << "\n";
	std::cout << "TerrainDepth: " << cfgParser->getTerrainDepth() << "\n";

	delete cfgParser;

	system("pause");

	return 0;
}