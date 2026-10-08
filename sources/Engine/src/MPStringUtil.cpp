#include "stdafx.h"
#include "MPStringUtil.h"

//-----------------------------------------------------------------------------
void MPStringUtil::trim( String& str, bool left, bool right)
{
	static const String delims = " \t\r";
	if(right)
		str.erase(str.find_last_not_of(delims)+1); // strip from the right
	if(left)
		str.erase(0, str.find_first_not_of(delims)); // strip from the left
}
//-----------------------------------------------------------------------------
MPStringVector MPStringUtil::split( const std::string& str, const std::string& delims, unsigned int maxSplits)
{
	std::vector<std::string> ret;
	unsigned int numSplits = 0;

	// uses an STL facility
	size_t start, pos;
	start = 0;
	do 
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// does nothing
			start = pos + 1;
		}
		else if (pos == std::string::npos || (maxSplits && numSplits == maxSplits))
		{
			// copy the rest of the string
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			// copy up to the delimiter
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
		}
		// look up the next wanted string
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != std::string::npos);

	return ret;
}
//-----------------------------------------------------------------------------
void toLowerCase( String& str )
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}
//-----------------------------------------------------------------------------
void toUpperCase( String& str )
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}
