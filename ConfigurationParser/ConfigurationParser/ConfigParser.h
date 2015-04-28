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

	struct Color{
		float r;
		float g;
		float b;
	};

private:
	float spinning;
	float spinSpeed;
	Color backgroundColor;
	std::string terrainPath;
	float terrainWidth;
	float terrainDepth;
	float terrainHeight;
};

