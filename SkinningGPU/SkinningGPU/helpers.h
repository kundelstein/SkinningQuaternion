#pragma once


#include <string>


inline
void RemoveQuotes (std::string& input)
{
	size_t pos;
	while ((pos = input.find ('\"')) != std::string::npos)
	{
		input.erase (pos, 1);
	}
}

