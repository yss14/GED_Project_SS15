/*********************************************************************************
 File: TextureGenerator.h
 Texture- and normal-map generation helper.
 Author: F. Reichl
 *********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>


namespace GEDUtils
{
    /// Utility struct: A 2-dimensional float vector
    struct Vec2f
    {
        float u;
        float v;

        Vec2f(float u, float v): u(u), v(v) {}
        Vec2f(): u(0), v(0) {}
    };


    /// Utility struct: A 3-dimensional float vector
    struct Vec3f
    {
        float x;
        float y;
        float z;

        Vec3f(float x, float y, float z): x(x), y(y), z(z) {}
        Vec3f(): x(0), y(0), z(0) {}
    };


    /// Utility struct: A 4-component color
    struct Color4f
    {
        float r;
        float g;
        float b;
        float a;

        Color4f(float r, float g, float b, float a = 1.0f): r(r), g(g), b(b), a(a) {}
        Color4f(): r(0), g(0), b(0), a(1) {}
    };


    class TextureGenerator
    {
    public:
        /** The constructor receives the paths to four textures which are blended together according to
        the terrain properties.
        @param texturePathLowFlat Path to a texture representing low altitude, flat terrain (e.g. grass)
        @param texturePathLowSteep Path to a texture representing low altitude, steep terrain (e.g. dirt)
        @param texturePathHighFlat Path to a texture representing high altitude, flat terrain (e.g. pebbles)
        @param texturePathHighSteep Path to a texture representing high altitude, steep terrain (e.g. rocks)
        **/
        TextureGenerator(   const std::wstring& texturePathLowFlat,
                            const std::wstring& texturePathLowSteep,
                            const std::wstring& texturePathHighFlat,
                            const std::wstring& texturePathHighSteep
        );

        ~TextureGenerator();

        /// Generate normals based on the heightfield properties. Results are stored in normalsOut.
        void generateNormals(const std::vector<float>& heightfield, int resolution, 
            std::vector<Vec3f>& normalsOut) const;

        /// Generate a color texture based on the heightfield properties and normals. Results are stored in colorsOut.
        void generateColors(const std::vector<float>& heightfield, const std::vector<Vec3f>& normals, int resolution, 
            std::vector<Color4f>& colorsOut) const;


        /**
        Generate normal and color textures based on the heightfield properties and store them to the output filenames.
        NOTE: resolution is the actual output resolution, thus the heightfield is supposed to be of size resolution + 1!
        **/
        void generateAndStoreImages(const std::vector<float>& heightfield, int resolution,
            const std::wstring& colorFilename, const std::wstring& normalsFilename);


    private:

        // Implementation is hidden
        class Impl;
        std::unique_ptr<Impl>   m_pImpl;
    };
}