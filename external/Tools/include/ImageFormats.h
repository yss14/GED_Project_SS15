/*********************************************************************************
 File: ImageFormats.h
 Format conversion utilities
 Author: F. Reichl
 *********************************************************************************/


#pragma once

#include <DirectXTex.h>

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



    // Utility structs for type conversion below

    template<typename T>
    class ImageFormatUtils
    {
    public:
        bool isFormatCompatible(DXGI_FORMAT format)
        {
            assert(false && "Not implemented for this data type");
        }

        void convert(const uint8_t* from, T& to)
        {
            assert(false && "Not implemented for this data type");
        }

        void convert(const T& from, uint8_t* to)
        {
            assert(false && "Not implemented for this data type");
        }

        static const DXGI_FORMAT FORMAT = DXGI_FORMAT_UNKNOWN;
    };


    template<>
    class ImageFormatUtils<Color4f>
    {
    public:
        bool isFormatCompatible(DXGI_FORMAT format)
        {
            return format == DXGI_FORMAT_R8G8B8A8_UNORM || format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        }

        void convert(const uint8_t* from, Color4f& to)
        {
            to.r = float(from[0]) / 255.0f;
            to.g = float(from[1]) / 255.0f;
            to.b = float(from[2]) / 255.0f;
            to.a = float(from[3]) / 255.0f;
        }

        void convert(const Color4f& from, uint8_t* to)
        {
            assert(from.r <= 1.0f && "Invalid R color value, must be between 0 and 1");
            assert(from.g <= 1.0f && "Invalid G color value, must be between 0 and 1");
            assert(from.b <= 1.0f && "Invalid B color value, must be between 0 and 1");
            assert(from.a <= 1.0f && "Invalid A color value, must be between 0 and 1");

            to[0] = uint8_t(from.r * 255.0f);
            to[1] = uint8_t(from.g * 255.0f);
            to[2] = uint8_t(from.b * 255.0f);
            to[3] = uint8_t(from.a * 255.0f);
        }

        static const DXGI_FORMAT FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    };


    template<>
    class ImageFormatUtils<float>
    {
    public:
        bool isFormatCompatible(DXGI_FORMAT format)
        {
            return format == DXGI_FORMAT_R16_UNORM;
        }

        void convert(const uint8_t* from, float& to)
        {
            to = float(*(const uint16_t*)from) / 0xFFFF;
        }

        void convert(const float& from, uint8_t* to)
        {
            assert(from >= 0 && from <= 1.0f && "Invalid float value, must be between 0 and 1");
            *(uint16_t*)to = uint16_t(from * 0xFFFF);
        }

        static const DXGI_FORMAT FORMAT = DXGI_FORMAT_R16_UNORM;
    };


    template<>
    class ImageFormatUtils<Vec3f>
    {
    public:
        bool isFormatCompatible(DXGI_FORMAT format)
        {
            return format == DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        void convert(const uint8_t* from, Vec3f& to)
        {
            to.x = float(from[0]) / 255.0f;
            to.y = float(from[1]) / 255.0f;
            to.z = float(from[2]) / 255.0f;
        }

        void convert(const Vec3f& from, uint8_t* to)
        {
            assert(from.x >= -1.0f && from.x <= 1.0f && "Invalid X normal value, must be between -1 and 1");
            assert(from.y >= -1.0f && from.y <= 1.0f && "Invalid Y normal value, must be between -1 and 1");
            assert(from.z >= -1.0f && from.z <= 1.0f && "Invalid Z normal value, must be between -1 and 1");

            to[0] = uint8_t((from.x * 0.5f + 0.5f) * 255.0f);
            to[1] = uint8_t((from.y * 0.5f + 0.5f) * 255.0f);
            to[2] = uint8_t((from.z * 0.5f + 0.5f) * 255.0f);
            to[3] = 255;
        }

        static const DXGI_FORMAT FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    };
}