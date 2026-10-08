//----------------------------------------------------------------------
// Description: the parsing algorithm of the Edit data structure
// Author: lh 2004-07-12
// Last modified date:
//----------------------------------------------------------------------
#pragma once

namespace GUI
{

class CEditArticle;
class CEditChar;
class CEditRow;
class CEditSentence;
class CEditObj;

// a line, merely a container
class CEditRow
{
public:
	CEditRow();

	void	SetPos( int x, int y );
	void	PushUnit( CEditObj* pObj, CEditObj* pAtom );
	void	Clear();
	void	Render();
	DWORD	GetWordCount()					{ return _dwWordCount;				}

	CEditObj*	GetObj( DWORD dwIndex )		{ if( dwIndex<_units.size() ) return _units[dwIndex]; return NULL;	}
	DWORD		GetObjNum()					{ return (DWORD)_units.size();		}

	DWORD		GetHeight()					{ return _dwHeight;					}
	DWORD		GetWidth()					{ return _dwWidth;					}

private:
	typedef std::vector<CEditObj*> units;
	units			_units;				// the formatted data
	DWORD			_dwWordCount;
	units			_atoms;				// the original data pointer; it does not manage memory and is used to compute the cursor

	DWORD			_dwHeight;
	DWORD			_dwWidth;

};

class CEditTextObj;
class CEditControl;
class CEditStrategy;

class CEditParse
{
public:
	CEditParse()	{	_items.push_back( new CEditRow );	}

	// inserts an object at the position; on success returns the index after the insertion, on failure -1
	int		Insert( DWORD nIndex, CEditObj* pObj )			{ return -1;			}

	// deletes the block from start to end
	bool	Del( DWORD nStart, DWORD nEnd );

	// gets the cursor information at the given row and column; internally it returns the font and colour of the last text
	bool	GetPos( DWORD nIndex, int& x, int& y, DWORD& dwFont, DWORD& dwColor );

	// gets the region after the selection
	bool	GetSelectRect( DWORD dwStart, DWORD dwEnd, std::vector<POINT>& pt );

public:	
	// inserts a unit
	bool	InsertText( CEditTextObj* pText );
	bool	InsertControl( CEditControl* pControl );
	bool	InsertObj( CEditObj* pObj );

	// computes the cursor
	int		CursorText( CEditTextObj* pText );
	int		CursorControl( CEditControl* pControl );
	int		CursorObj( CEditControl* pControl );

private:
	typedef std::vector<CEditRow*> items;
	items		_items;					// logical memory, used for display and for computing logic

	DWORD		_dwRowLimit;			// line count limit
	DWORD		_dwMaxWordLimit;		// maximum character count limit

};

// the algorithm that builds a displayable article from the original memory
class CEditStrategy
{
public:
	CEditStrategy( CEditArticle* pActicle );
	~CEditStrategy();

	void	Init();
	bool	Append( CEditObj *pChar );
	void	Clear();
	void	Render();
	void	RefreshPos( int x, int y );

public:		// visitor pattern, for parsing the three kinds of element: text, control and graphic
	void	ParseText( CEditTextObj* pText );
	void	ParseControl( CEditControl* pControl );
	void	ParseObj( CEditObj* pObj );

private:
	CEditRow*	_AppendToBackRow( CEditObj *pChar );

private:
	CEditArticle*	_pActicle;	

	typedef std::vector<CEditRow*> items;
	items			_items;			// logical memory, used for display and for computing logic

};

inline void CEditStrategy::ParseObj( CEditObj* pObj )
{
	// a graphic element that cannot be merged
	CEditRow* pRow = _AppendToBackRow( pObj );
	pRow->PushUnit( pObj, pObj );
}

inline void CEditStrategy::ParseControl( CEditControl* pControl )
{
	_items.push_back( new CEditRow );	
}

}
