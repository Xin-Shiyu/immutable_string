#include <iostream>
#include <vector>
#include "string.h"

int main()
{
	using namespace nativa;
	string_view sv = "Across the Great Wall we can reach every corner in the world";
	std::vector<string_view> o;
	sv.split(' ', std::back_inserter(o));
	string s = string::concat(o);
	string s2 = string::join(" and ", o);
	string s3 = s2;
	string s4 = s2.clone();
	std::cout << s.c_str() << std::endl;
	std::cout << s2.c_str() << std::endl;
	std::cout << (s2.begin() == s3.begin()) << std::endl;
	std::cout << s2.ref_count() << std::endl;
	std::cout << (s2.begin() == s4.begin()) << std::endl;
}