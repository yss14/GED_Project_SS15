#include <vector>
#include <iostream>

int main(){

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

	return 0;
}