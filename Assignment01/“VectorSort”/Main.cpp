#include <vector>
#include <iostream>
#include <algorithm>

void printVectorInt(std::vector<int> vect);
bool descSort(int a, int b);

int main(){

	std::cout << "Please type in some numbers (0 to esc):\n";

	std::vector<int> myVector;
	std::vector<int>::iterator it;

	int number = 0;

	do{
		std::cin >> number;

		if (number != 0){
			it = myVector.end();
			myVector.insert(it, number);
		}
	} while (number != 0);

	std::cout << "Unsorted:\n";
	printVectorInt(myVector);

	std::sort(myVector.begin(), myVector.end(), descSort);

	std::cout << "Sorted:\n";
	printVectorInt(myVector);

	system("pause");

	return 0;
}

void printVectorInt(std::vector<int> vect){
	for (std::vector<int>::const_iterator i = vect.begin(); i != vect.end(); i++){
		std::cout << *i << ' ';
	}
	std::cout << "\n";
}

bool descSort(int a, int b){
	return a > b;
}

