#pragma once
#pragma_once

#include <string>

class ConfigParser
{
public:
	ConfigParser();
	~ConfigParser();

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

