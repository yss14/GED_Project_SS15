#pragma once

#include <istream>
#include "ConfigParser.h"
#include <string>

int main(){

	ConfigParser *cfgParser = new ConfigParser();


	cfgParser->load(std::string("game.cfg"));

	delete cfgParser;

	system("pause");

	return 0;
}