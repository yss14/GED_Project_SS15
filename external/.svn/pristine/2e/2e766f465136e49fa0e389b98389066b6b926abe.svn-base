/*********************************************************************************
 File: ImageUtils.h
 Helper classes for image handling
 Author: F. Reichl
 *********************************************************************************/

#pragma once

#include <DirectXTex.h>

#include <sstream>
#include <assert.h>
#include <exception>

#include <vector>

#include <wincodec.h>
#include "ImageFormats.h"


namespace GEDUtils
{
    /// Increase the Windows Imaging Components reference counter and initialize WIC if this is the first call
    void WICRef();
    /// Decrease the Windows Imaging Components reference counter and deinitialize WIC if this is the last call
    void WICUnref();

    
    /**
    Simple image utility class.
    
    To avoid the usage of templates, typedefs for the required specialized types exits:
    - GEDUtils::SimpleColorImage for the color map
    - GEDUtils::SimpleFloatImage for the heightfield
    - GEDUtils::SimpleNormalImage for the normals

    SimpleImage provides easy access on a per-pixel basis to a DirectX::ScratchImage with automatic
    type conversion for the required types. Internally, a DirectX::ScratchImage is created with a format
    based on the template type.

    Supported types are:
    - GEDUtils::Color4f, mapped to a rgba-image with 8 bits per pixel (DXGI_FORMAT_R8G8B8A8_UNORM)
    - GEDUtils::Vec3f, mapped to a 4-component 32-bit floating point image (DXGI_FORMAT_R32G32B32A32_FLOAT)
    - float, mapped to a greyscale 32-bit floating point image

    The methods getPixel() and setPixel() provide automatic type conversion to the mapped type.
    Instantiating this class with a template parameter other than the supported types will result
    in a runtime exception.

    For a more sophisticated format support, you need to use the DirectXTex library!
    **/
    template<typename T>
    class SimpleImage
    {
    public:
        /// Constructor, loads an image from disk
        SimpleImage(const std::wstring& filename);
        /** 
        Constructor, creates an image of given width and height.
        @param data Optional initialization data. The size of the vector must match width * height.
        **/
        SimpleImage(unsigned int width, unsigned int height);

        /// Copy constructor
        SimpleImage(const SimpleImage& copy);
        /// Move constructor
        SimpleImage(SimpleImage&& copy);

        /// Destructor
        ~SimpleImage();


        /// Get the pixel value at position x, y (converted to the specified type)
        T getPixel(unsigned int x, unsigned int y);
        /// Set the pixel value at position x, y (converted from the specified type)
        void setPixel(unsigned int x, unsigned int y, const T& value);
        
        /// Get the underlying DirectX::ScratchImage itself
        DirectX::ScratchImage& getImage() { return m_image; }
        /// Get the underlying DirectX::ScratchImage itself, const version
        const DirectX::ScratchImage& getImage() const { return m_image; }

        /// Save the image
        void saveToFile(const std::wstring& filename, const GUID targetFormat = GUID_ContainerFormatPng) const;


    private:
        DirectX::ScratchImage       m_image;
        ImageFormatUtils<T>         m_formatUtils;
    };


    typedef SimpleImage<Color4f> SimpleColorImage;      ///< 8bit rgba color image
    typedef SimpleImage<float> SimpleFloatImage;        ///< 32bit greyscale image
    typedef SimpleImage<Vec3f> SimpleNormalImage;       ///< 32bit rgba image




    // **************************************************************************************
    // ^^^ End of header. ^^^
    //
    // Implementation of SimpleImage
    // **************************************************************************************


    template<typename T>
    SimpleImage<T>::SimpleImage(const std::wstring& filename)
    {
        WICRef();

        HRESULT hr;

        hr = DirectX::LoadFromWICFile(filename.c_str(), 0, nullptr, m_image);

        if (hr != S_OK || m_image.GetMetadata().width == 0 || m_image.GetMetadata().height == 0)
        {
            std::wstringstream ss;
            ss << "Error opening image file \"" << filename << "\"";
            std::wstring str = ss.str();
            std::string s(str.begin(), str.end());
            throw std::exception(s.c_str());
        }
            

        // Check for matching formats
        if (!m_formatUtils.isFormatCompatible(m_image.GetMetadata().format))
        {
            std::wstringstream ss;
            ss << "Image does not contain compatible data: \"" << filename << "\"";
            std::wstring str = ss.str();
            std::string s(str.begin(), str.end());
            throw std::exception(s.c_str());
        }
    }


    template<typename T>
    SimpleImage<T>::SimpleImage(const SimpleImage<T>& copy): m_image(copy.m_image)
    {
        WICRef();
    }
        
    template<typename T>
    SimpleImage<T>::SimpleImage(SimpleImage<T>&& copy): m_image(copy.m_image)
    {
        WICRef();
    }


    template<typename T>
    SimpleImage<T>::~SimpleImage()
    {
        WICUnref();
    }


    template<typename T>
    SimpleImage<T>::SimpleImage(unsigned int width, unsigned int height)
    {
        WICRef();
        m_image.Initialize2D(ImageFormatUtils<T>::FORMAT, width, height, 1, 1);
    }


    template<typename T>
    T SimpleImage<T>::getPixel(unsigned int x, unsigned int y)
    {
        assert(x < m_image.GetMetadata().width && y < m_image.GetMetadata().height && "Pixel out of bounds");
        T result;
        const uint8_t* pPixel = m_image.GetImage(0, 0, 0)->pixels + y * m_image.GetImage(0, 0, 0)->rowPitch + x * DirectX::BitsPerPixel(m_image.GetMetadata().format) / 8;
            
        m_formatUtils.convert(pPixel, result);
        return result;
    }


    template<typename T>
    void SimpleImage<T>::setPixel(unsigned int x, unsigned int y, const T& value)
    {
        assert(x < m_image.GetMetadata().width && y < m_image.GetMetadata().height && "Pixel out of bounds");
        uint8_t* pPixel = m_image.GetImage(0, 0, 0)->pixels + y * m_image.GetImage(0, 0, 0)->rowPitch + x * DirectX::BitsPerPixel(m_image.GetMetadata().format) / 8;
            
        m_formatUtils.convert(value, pPixel);
    }


    template<typename T>
    void SimpleImage<T>::saveToFile(const std::wstring& filename, const GUID targetFormat) const
    {
        HRESULT hr = DirectX::SaveToWICFile(m_image.GetImage(0, 0, 0), 1, DirectX::WIC_FLAGS_NONE, targetFormat, filename.c_str());

        if (hr != S_OK)
        {
            std::wstringstream ss;
            ss << "Error saving image to file\"" << filename << "\"";
            std::wstring str = ss.str();
            std::string s(str.begin(), str.end());
            throw std::exception(s.c_str());
        }
    }
}