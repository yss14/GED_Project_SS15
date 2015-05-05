/*********************************************************************************
 File: DiamondSquare.h
 Implementation of the Diamond Square Algorithm
 Author: I. Demir, F. Reichl
 *********************************************************************************/

#pragma once

#include <random>
#include <memory>

namespace GEDUtils
{
    class DiamondSquare
    {
    public:

	    /// Constructor, initializes with a heightfield resolution and roughness parameter
	    DiamondSquare(
		    int			resolution,
		    float		roughness);

        /// Copy constructor
        DiamondSquare(DiamondSquare& copy);
        /// Move constructor
        DiamondSquare(DiamondSquare&& copy);
			
	    /// Destructor
	    ~DiamondSquare(void);

	    /// Returns the resolution of the heightfield
	    int getResolution() const;

	    /// Perform the diamond square algorithm
	    void generateHeightfield();

	    /// Smooth the generated heightfield with a given number of iterations and a kernel size
	    void smoothHeightfield(int iterations = 1, int kernel = 3);

        const std::vector<float>& getHeightfield() const;


    private:
        // Implementation is hidden
        class Impl;
        std::unique_ptr<Impl>   m_pImpl;
    };
}

