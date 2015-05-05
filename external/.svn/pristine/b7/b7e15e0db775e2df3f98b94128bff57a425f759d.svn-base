/************************************************************************* 
	Example: How to use the SimpleImage Class
	Author: I. Demir
	**********************************************************************
*/

#include <iostream>
#include <SimpleImage.h>

int main()
{
	try {

		// Create an empty image of size 640 x 480
		GEDUtils::SimpleImage my_first_image(640, 480);

		// Fill the image with a color gradient
		for(UINT y=0; y<my_first_image.getHeight(); ++y) {
			for(UINT x=0; x<my_first_image.getWidth(); ++x) {
				// Set RGB Values at each pixel
				my_first_image.setPixel(x,y,
					x / (float)my_first_image.getWidth(), /* Red */
					y / (float)my_first_image.getHeight(), /* Green */
					0. /* Blue */);
			}
		}

		 // Save my_first_image into "test.png"
		 // Note: Only PNG Format is supported
		if(!my_first_image.save("test.png")) {
			throw "Could not save first image";
		}

		// Load "test.png" into the image test
		GEDUtils::SimpleImage test("test.png");

		// Read the RGB values at pixel (100,200)
		float r,g,b;
		test.getPixel(100,200,r,g,b);

		// Show RGB values on the screen
		printf("R: %f, G: %f, B: %f\n", r, g, b);

		// Create another image of size 640 x 480
		GEDUtils::SimpleImage my_gray_image(640, 480);

		// Fill with a gray color gradient
		for(UINT y=0; y<my_gray_image.getHeight(); ++y) {
			for(UINT x=0; x<my_gray_image.getWidth(); ++x) {
				// Set Gray Values at each pixel
				my_gray_image.setPixel(x,y,
					x / (float)my_gray_image.getWidth() /* Gray*/ );
			}
		}

		// Save my_gray_image into "test_gray.png"
		// Note: Only PNG Format is supported
		if(!my_gray_image.save("test_gray.png")) {
			throw "Could not save gray image";
		}

		// Load "test_gray.png" into the image test_gray
		GEDUtils::SimpleImage test_gray("test_gray.png");

		// Read the Gray Value at pixel (100,200)
		float gray = test_gray.getPixel(100,200);

		// Show Gray Value on the screen
		printf("Gray: %f\n", gray);

	} catch(char* exception) {
		printf("Error: %s\n", exception);
	}

	system("pause");


}