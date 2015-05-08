#pragma once

#include <string>
#include <vector>

class TextureGenerator
{
public:
	TextureGenerator(	const std::wstring& texturePathLowFlat,
						const std::wstring& texturePathLowSteep,
						const std::wstring& texturePathHighFlat,
						const std::wstring& texturePathHighSteep
	);

	~TextureGenerator();

private:
	/*
	
	/// Generate normals based on the heightfield properties. Results are stored in normalsOut.
	void generateNormals(const std::vector<float>& heightfield, int resolution,
		std::vector<Vec3f>& normalsOut) const;

	/// Generate a color texture based on the heightfield properties and normals. Results are stored in colorsOut.
	void generateColors(const std::vector<float>& heightfield, const std::vector<Vec3f>& normals, int resolution,
		std::vector<Color4f>& colorsOut) const;

	*/
};

