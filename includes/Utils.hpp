#pragma once

#include <vector>
#include <iostream>

namespace Utils {
	template<typename T>
	void displayVector(const std::vector<T> &arr) {
		for (size_t i = 0; i < arr.size(); i++)
			std::cout << arr[i] << std::endl;
	}
};
