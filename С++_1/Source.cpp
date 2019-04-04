#include <iostream>
#include <memory>
#include <cstdint>
#include <iterator>
#include "StackAllocator.h"
#include "XorList.h"




int main() {
	XorList<int> a;
	a.push_back(1);
	a.erase(a.begin());
	system("pause");
}