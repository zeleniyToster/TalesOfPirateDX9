//----------------------------------------------------------------------
// Name: fixed image style list
// Author: lh 2004-10-09
// Last modified date: 2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "uicompent.h"
#include "Tools.h"

namespace GUI
{

// a List of fixed size; in fact it is only a background plus a selection box, and selecting moves the selection box up and down
class CFixList : public CCompent
{
public:
	CFixList(CForm& frmOwn);
	CFixList( const CFixList& rhs );
	CFixList& operator=(const CFixList& rhs);
	~CFixList();
	GUI_CLONE(CFixList)

	virtual void		Render();
	virtual void		Refresh();
	virtual bool		MouseRun( int x, int y, DWORD key );
	virtual bool		MouseScroll( int nScroll );
	virtual bool		SetMaxNum( int n );
	virtual bool		IsHandleMouse()				{ return true;					}

	void				SetMargin( int left, int top, int right, int bottom );
	void				SetString( int n, const char* str );
	const char*			GetString( int n );
	int					GetSelIndex()				{ return _lmtSel.GetCur();		}

	CGuiPic*			GetImage()					{ return _pImage;				}
	CGuiPic*			GetSelectImage()			{ return _pSelect;				}

	void				SetRowHeight( int v )		{ if( v>0 ) _nRowHeight = v;	}
	void				SetRowSpace( int v )		{ if( v>=0 ) _nRowSpace = v;	}

public:	// events
	GuiEvent			evtSelectChange;				// the selection changed
	GuiMouseEvent		evtListMouseDown;			// the mouse is pressed in the text area of the list

private:
	void				_SetSelf( const CFixList& rhs );
	void				_RefreshSelY();

protected:
	int					_nLeftMargin;	// distance to the surrounding border
	int					_nTopMargin;
	int					_nRightMargin;
	int					_nBottomMargin;

	int					_nRowHeight;	// row height
	int					_nRowSpace;		// row spacing

	typedef std::vector<std::string> vstring;
	vstring				_vstString;

	CGuiPic*			_pImage;		// background image
	CGuiPic*			_pSelect;		// selection box
	int					_nSelX, _nSelY;
	CLimit<int>			_lmtSel;		// the current selection

	int					_nHitX1, _nHitY1, _nHitX2, _nHitY2;

};

// Purpose: marks on each column whether it is already selected
class CCheckFixList : public CFixList
{
public:
	CCheckFixList(CForm& frmOwn);
	CCheckFixList( const CCheckFixList& rhs );
	CCheckFixList& operator=(const CCheckFixList& rhs);
	~CCheckFixList();
	GUI_CLONE(CCheckFixList)

	virtual bool			SetMaxNum( int n );
	virtual void			Render();

	void					SetCheckMargin( int left, int top )	{ _nCheckLeftMargin=left; _nCheckTopMargin=top; }
	CGuiPic*				GetCheckImage()						{ return _pImgCheck;							}
	CGuiPic*				GetUnCheckImage()					{ return _pImgUnCheck;							}
	void					SetIsCheck( int index, bool v );
	bool					GetIsCheck( int index );

private:
	void					_SetSelf( const CCheckFixList& rhs );

	static void				_OnCheckDown(CGuiData *pSender, int x, int y, DWORD key) {
		((CCheckFixList*)(pSender))->_OnCheckDown();
	}
	void					_OnCheckDown()				{_checks[_lmtSel.GetCur()] = !_checks[_lmtSel.GetCur()];}

protected:
	typedef std::vector<bool>	vbool;
	vbool					_checks;

	int						_nCheckLeftMargin, _nCheckTopMargin;	// left and top margins of the displayed Check

	CGuiPic*				_pImgCheck;			// selected
	CGuiPic*				_pImgUnCheck;		// not selected

};

// inline function
inline 	void CFixList::SetMargin( int left, int top, int right, int bottom ) 
{ 
	_nLeftMargin = left;
	_nTopMargin = top;
	_nRightMargin= right;
	_nBottomMargin = bottom; 
}

inline 	void CFixList::SetString( int n, const char* str )
{ 
	if( n>0 && n<=_lmtSel.GetMax() )
	{
		_vstString[n] = str; 
	}
}

inline void	CFixList::_RefreshSelY() 
{ 
	_nSelY = GetSelIndex() * (_nRowHeight+_nRowSpace) + _nHitY1; 
}

inline void CCheckFixList::SetIsCheck( int index, bool v )
{ 
	if(index>=0 && index<=_lmtSel.GetMax()) 
		_checks[index]=v; 
}

inline bool CCheckFixList::GetIsCheck( int index )
{ 
	if(index>=0 && index<=_lmtSel.GetMax()) 
		return _checks[index]; 
	return false;
}

}
