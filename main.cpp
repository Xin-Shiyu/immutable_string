#include "format.h"
#include <stdio.h>

int main()
{
	for (size_t i = 0; i != (size_t)-1; ++i)
		puts(nativa::format("The statement that {0} is odd is {1}", i, i % 2 != 0).c_str());
};