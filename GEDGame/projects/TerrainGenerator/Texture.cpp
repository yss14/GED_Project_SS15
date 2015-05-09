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

	this->textureImage->getPixel(u % this->textureImage->getWidth(), 
		this->textureImage->getHeight(), colorAtPixel.r, colorAtPixel.g, colorAtPixel.b);

	return colorAtPixel;
}
