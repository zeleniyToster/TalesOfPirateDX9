//----------------------------------------------------------------------
// Name: UI script class
// Author: lh 2004-10-26
// Purpose: a helper class so other objects can be used by external scripts
//   note: cannot currently be deleted dynamically at runtime
// Last modified date:
//----------------------------------------------------------------------
#pragma once
#include "uiguidata.h"

namespace GUI
{

template <class T>
class UIScript
{
public:
	T*		GetObj( unsigned int nIndex )	{ if(nIndex>=list.size()) return NULL;	return list[nIndex];	}
	int		AddObj( T* p )					{ list.push_back(p); return (int)(list.size()-1);				}

private:
	typedef std::vector<T*> vt;
	vt		list;

};

}
