/************************************************************************* 
	Class: SimpleImage
	Author: I. Demir
	**********************************************************************
*/

#pragma once
#include <cstdint>
#pragma warning(push)
#pragma warning(disable:4005)
#include <wincodec.h>
#pragma warning(pop)
#include "ComPtr.h"

#pragma comment(lib, "Windowscodecs.lib") 

namespace GEDUtils
{

class SimpleImage
{        
public:
	// Create an empty image
	SimpleImage(UINT width, UINT height);

	// Create Image from a image File using WIC
	SimpleImage(const char* filename);
	SimpleImage(const wchar_t* filenameW);

	~SimpleImage(void);

	SimpleImage(const SimpleImage& x);

	SimpleImage& operator=(SimpleImage& x);

	SimpleImage clone() const;

	// Save Image to PNG File
	bool save(const char* filename) const;
	bool save(const wchar_t* filenameW) const;

	UINT getHeight() const;

	UINT getWidth() const;

	// set RGB Values to pixel (x,y)
	// r,g,b must lie in [0;1]
	// Note: After setting RGB values, you cannot set or get gray values anymore
	void setPixel(UINT x, UINT y, const float r, const float g, const float b);

	// set Gray Value to pixel (x,y)
	// gray must lie in [0;1]
	// Note: After setting gray values, you cannot set or get RGB values anymore
	void setPixel(UINT x, UINT y, const float gray);

	// get RGB Values at pixel (x,y)
	// r,g,b will lie in [0;1]
	void getPixel(UINT x, UINT y, float& r, float& g, float& b) const;

	// get Gray Value at pixel (x,y)
	// result will lie in [0;1]
	float getPixel(UINT x, UINT y) const;

	// get pointer to raw image data
	const BYTE * getRawImageData() const;

private:

	typedef struct {
		UINT			height;
		UINT			width;
		WORD*			img;
		int				gray;
		unsigned int	num_ref;
	} Data;

	Data* data;

	void unref();

	void ref(int width, int height);

	bool wic_LoadImg(const wchar_t* filenameW);

	bool wic_SaveImg(const wchar_t* filenameW, int bpp) const;
};

}