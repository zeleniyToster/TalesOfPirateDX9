//----------------------------------------------------------------------
// Name: manages the memory of identical forms
// Author: lh 2005-03-15
// Last modified date:
//----------------------------------------------------------------------
#pragma once

namespace GUI
{
class CForm;

// manages the forms cloned from the same source; on deletion they are not really deleted but cached, so the next clone can use them directly
class CCloneForm
{
public:
	CCloneForm();
	~CCloneForm();

	void	SetSample( CForm* frm )	{ _pSample = frm;	}

	CForm*	Clone();
	bool	Release( CForm* p );

private:
	CForm*	_pSample;		// sample form

	typedef std::vector<CForm*>	vfrm;
	vfrm	_vfrm;

    int     _nCount;		// number of valid forms

};

// gets a hidden form among the forms of the same class automatically; if there is no hidden form, one is cloned
class CHideForm
{
public:
	void	Init( CForm* frm )		{ _vfrm.push_back( frm );	}

	CForm*	GetHide();
	void	CloseAll();		// closes all forms

	CForm*  GetForm( int n )		{ return _vfrm[n];			}
	int		GetCount()				{ return (int)_vfrm.size();	}

	CForm*  operator[]( int n )		{ return _vfrm[n];			}

private:
	typedef std::vector<CForm*>	vfrm;
	vfrm	_vfrm;

};

}

