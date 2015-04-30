#define IDX(x, y, w)((x) + (y) * (w))
#define RAND_MAX = 1.0f;

#include <cstdlib>
#include <time.h>
#include <iostream>

float* smoothArray(float* arr, const int width, const int height);
void printArray(float* arr, int width, int height);

int main(){

	int const width = 10;
	int const height = 10;

	float myArray[width * height];

	srand(time(0));

	for (int i = 0; i < width * height; i++){
		myArray[i] = (rand() % 11) / 10.0f;
	}

	float* smoothedArray = smoothArray(myArray, width, height);
	printArray(smoothedArray, width, height);

	delete smoothedArray;

	system("pause");

	return 0;
}

float* smoothArray(float* arr, const int width, const int height){
	float *smooth = new float[width * height];
	for (int x = 0; x < width; x++){
		for (int z = 0; z < width; z++){
			float sum = 0.0f;
			int count = 0;
			for (int i = x-1; i < x+1; i++){
				for (int j = z - 1; j < z+1 + 1; j++){
					if (i >= 0 && i < width && j >= 0 && j < height){
						sum += arr[IDX(i, j, width)];
						count++;
					}
				}
			}

			smooth[IDX(x, z, width)] = sum / count;
		}
	}

	return smooth;
}


void printArray(float* arr, int width, int height){
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++){
			std::cout << arr[IDX(i, j, width)] << " ";
		}
		std::cout << "\n";
	}
}