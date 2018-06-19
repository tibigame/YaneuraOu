#include <stdlib.h>
#include "./i_to_u8.h"

std::string i_to_u8(int i) {
	char a[100];
	_itoa_s(i, a, 10);
	std::string output = a;
	return output;
}
