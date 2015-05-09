#pragma once

#include <SimpleImage.h>
#include <string>
#include "Color4f.h"

class Texture
{
public:
	Texture(const std::wstring& texturePath);
	~Texture();

	Color4f getColorTiled(int u, int v);

private:
	GEDUtils::SimpleImage *textureImage;
};

