//----------------------------------------------------------------------
// Name: manages the input to Edit, cursor display, selection display and the like
// Author: lh 2005-05-23
// Last modified date:
//----------------------------------------------------------------------
#pragma once

namespace GUI
{

class CEditParse;
class CEditObj;
class CEditKey
{
public:		
	CEditKey();
	~CEditKey();

	void	Init();
	bool	SetFont( DWORD dwFont );
	void	SetColor( DWORD dwColor )		{ _dwFontColor=dwColor;		}

public:		// external interface
	bool	OnChar( char c );
	bool	OnKeyDown( int key );

	void	Render();	

private:
	void	AddChar( CEditObj* pObj );

private:
	CEditParse*		_pParse;

	char			_szEnter[16];				// the buf used to compute the input
	int				_nEnterPos;

	bool			_IsReadyOnly;				// whether it is read only

	DWORD			_dwFontColor;				// current font colour
	int				_dwFontIndex;				// current font

private:
	DWORD			_dwCurosrIndex;				// current cursor position

	DWORD			_dwCursorHeight;			// cursor height
	DWORD			_dwCursorColor;				// cursor colour
	int				_nCursorX, _nCursorY;		// cursor rendering position
	bool			_IsShowCursor;
	DWORD			_dwCursorTime;
	DWORD			_dwCursorSpace;

};

}
