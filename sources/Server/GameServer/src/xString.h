#ifndef XSTRING_H
#define XSTRING_H

#include "TChar.h"

// gets a string from the given position (long *in_from) of the given string (_TCHAR *in),
// ending when any character of the string list (_TCHAR *end_list) is met
long StringGet(_TCHAR *out, long out_max, _TCHAR *in, long *in_from, _TCHAR *end_list, long end_len);


// removes any character of the string list (_TCHAR *end_list) from the given position (long *in_from) of the given string (_TCHAR *in)
void StringSkipCompartment(_TCHAR *in, long *in_from, _TCHAR *skip_list, long skip_len);





















#endif
