#ifndef MPStringUtil_H
#define MPStringUtil_H

#include "MPEffPrerequisites.h"
#include "MPStringVector.h"

class MINDPOWER_API MPStringUtil
{
public:
	typedef std::stringstream StrStreamType;

public:
	/** Removes any whitespace characters; the standard set is space, tab and so on.
	@remarks
	Whether the leading or trailing whitespace is removed can be specified; by default both are.
	*/
	static void trim( String& str, bool left = true, bool right = true );

	/** Returns an MPStringVector holding every substring between the delimiters.
	@param
	delims list of delimiters
	@param
	maxSplits maximum number of splits. 0 splits indefinitely; when the count is greater than 0
	the split stops once that many substrings have been produced.
	*/
	static MPStringVector split( const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0);

	/** Converts every character of the string to lower case.
	*/
	static void toLowerCase( String& str );

	/** Converts every character of the string to upper case.
	*/
	static void toUpperCase( String& str );

};
#endif