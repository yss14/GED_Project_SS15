#pragma once

#define IDX(x, y, w)((x) + (y) * (w))

#include <string>
#include <vector>
#include "Vec3f.h"
#include <SimpleImage.h>
#include <iostream>
#include "Texture.h"
#include "Color4f.h"

class TextureGenerator
{
public:
	TextureGenerator(	const std::wstring& texturePathLowFlat,
						const std::wstring& texturePathLowSteep,
						const std::wstring& texturePathHighFlat,
						const std::wstring& texturePathHighSteep
	);

	~TextureGenerator();

	void generateAndStoreImages(std::vector<float> heightmap, int resolution, _TCHAR* normalPath,
		_TCHAR* colorPath);

private:
	std::wstring texturePathLowFlat;
	std::wstring texturePathLowSteep;
	std::wstring texturePathHighFlat;
	std::wstring texturePathHighSteep;
	
	/// Generate normals based on the heightfield properties. Results are stored in normalsOut.
	void generateNormals(const std::vector<float>& heightfield, int resolution,
		_TCHAR* path, std::vector<Vec3f> normals);

	/// Generate a color texture based on the heightfield properties and normals. Results are stored in colorsOut.
	void generateColors(const std::vector<float>& heightfield, const std::vector<Vec3f>& normals, int resolution,
		std::vector<Color4f>& colorsOut);

	
};

