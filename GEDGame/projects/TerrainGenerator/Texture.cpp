#include "stdafx.h"
#include "Texture.h"


Texture::Texture(const std::wstring& texturePath)
{
	this->textureImage = new GEDUtils::SimpleImage(texturePath.c_str());
}


Texture::~Texture()
{
	delete this->textureImage;
}

Color4f Texture::getColorTiled(int u, int v){
	Color4f colorAtPixel;

	int width = this->textureImage->getWidth();

	this->textureImage->getPixel(u % width,
		v % width, colorAtPixel.r, colorAtPixel.g, colorAtPixel.b);

	return colorAtPixel;
}
