//--------------------------------------------------------------
// Name: GUI interface
// Author: lh 2004-07-15
// Design: CGuiDat manages memory and provides the control interface
//		     CForm displays, handles events, switches control focus, and loads and stores resource files
// note: every instance may only be created on the heap, statically allocated when the program starts
//    releases the resources on exit
// Modified: 2004-10-26 by lh
// Change: the script functionality of CItemObj was split out into CUIScript; CItemRow supports polymorphism
//--------------------------------------------------------------

#pragma once
#include <assert.h>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include "UICursor.h"
#include "UIFont.h"
#include "UIPicture.h"

// the following macros are used to support GUI cloning
#define GUI_CLONE(TName)	\
public:							\
	virtual CGuiData* Clone() { return new (##TName)(*this); }	\
	virtual bool Clone( CGuiData* rhs ) {						\
		##TName *p = dynamic_cast<##TName*>(rhs);				\
		if( !p ) return false;									\
		*this = *p;												\
		return true;											\
	}

namespace GUI
{
// the set of mouse behaviours, used in the key parameter of the MouseRun function
enum eMouseState
{
	Mouse_LDown		= 0x0001,
	Mouse_MDown		= 0x0002,
	Mouse_RDown		= 0x0004,
	Mouse_Down		= 0x0008,	// a button is pressed
	Mouse_LUp		= 0x0010,
	Mouse_MUp		= 0x0020,
	Mouse_RUp		= 0x0040,
	Mouse_Move		= 0x0080,
	Mouse_LDB		= 0x0100,
	Mouse_MDB		= 0x0200,
	Mouse_RDB		= 0x0400,
	Mouse_LClick	= 0x0800,	// left mouse click
	Mouse_MClick	= 0x1000,	
	Mouse_RClick	= 0x2000,	
};

enum eCompentAlign
{
	caNone,				// does not change position
	caLeft,				// fills the left side
	caLeftUp,			// placed at the top left corner
	caUp,
	caRightUp,
	caRight,
	caRightBottom,
	caBottom,
	caLeftBottom,	
	caClient,			// fills everything
    caCenter,           // completely centred
    caWidthCenter,      // horizontally centred
    caHeightCenter,     // vertically centred
	caEnd,				// terminator, meaningless
};

const BYTE DROP_ALPHA = 0xA0;

class CCompent;
class CGuiData;
class CForm;

typedef void (*GuiMouseEvent) (CGuiData *pSender, int x, int y, DWORD key);
typedef bool (*GuiKeyDownEvent)(CGuiData *pSender, int& key );
typedef bool (*GuiKeyCharEvent)(CGuiData *pSender, char& key );
typedef void (*GuiEvent)(CGuiData *pSender);
typedef void (*GuiOnTimerEvent)(CGuiData *pSender,DWORD nEventID,PVOID lpData);
typedef void (*CompentFun)(CCompent* pSender, unsigned int index );
typedef bool (*GuiHotKeyEvent)(CForm *pSender, char key );
typedef void (*GuiItemClickEvent)(std::string strItem);

// GUI object dragging
class CDrag
{
public:
	enum eState
	{
		stNone=0,
		stPress,
		stDrag,
		stEnd,
	};

public:
	CDrag();

	void			SetYare( unsigned int n )	{ if(n<30) _nYareLen = n; }

	bool			MouseRun( int x, int y, DWORD key );
	int				BeginMouseRun( CGuiData* gui, bool InRect, int x, int y, DWORD key );	// decides the mouse state; returns 0 for no action

	static CDrag*		GetDrag()		{ return _pDrag;	}
	static CGuiData*	GetParent()		{ return _pParent;	}

	static bool		IsDraging(CGuiData *p)	{ return _pParent==p;	}
	static void		SetSnapToGrid( DWORD dwWidth, DWORD dwHeight );

	static void		SetDragX( int x )	{ _nDragX = x;		}
	static void		SetDragY( int y )	{ _nDragY = y;		}

	static int		GetDragX()			{ return _nDragX;	}
	static int		GetDragY()			{ return _nDragY;	}

	static int		GetStartX()			{ return _nStartX;	}
	static int		GetStartY()			{ return _nStartY;	}

	static int		GetX()				{ return _nStartX + _nDragX;					}
	static int		GetY()				{ return _nStartY + _nDragY;					}

	void			Reset();

	void			SetIsMove( bool v ) { _IsMove = v;		}
	bool			GetIsMove()			{ return _IsMove;	}
	
	void			SetIsUseGrid(bool v){ _IsUseGrid = v;	}
	bool			GetIsUseGrid()		{ return _IsUseGrid;}

	int				GetState()			{ return _eState;	}

	void			SetDragInCursor( CCursor::eState v )	{ _crDragIn = v;	}
	CCursor::eState	GetDragInCursor()						{ return _crDragIn;	}
	void			SetDragCursor( CCursor::eState v )		{ _crDrag = v;		}
	CCursor::eState	GetDragCursor()							{ return _crDrag;	}

public:
	GuiMouseEvent	evtMouseDragBegin;			// dragging starts
	GuiMouseEvent	evtMouseDragMove;			// dragging event
	GuiMouseEvent	evtMouseDragEnd;			// dragging ends

private:
	static CDrag*		_pDrag;					// the object currently being dragged
	static CGuiData*	_pParent;				// the gui currently being dragged

	static int			_nDragX, _nDragY;
	static int			_nStartX, _nStartY;

	int				_nYareLen;
	eState			_eState;

	bool			_IsMove;					// once the movement ends, the control is moved directly
	bool			_IsUseGrid;					// whether the alignment grid is used

	CCursor::eState	_crDragIn;					// the cursor inside the drag area
	CCursor::eState	_crDrag;					// the cursor while dragging
    static 	CCursor::eState	_crNormal;			// the cursor in normal situations

	static DWORD    _dwGridWidth, _dwGridHeight;// the width and height of the alignment grid
	static DWORD	_dwMouseUpTime;				// after dragging has started, if there is no more MouseUP message but the mouse has been released for a while, the drag is cancelled

};

// the following macros are used to support GUI cloning
#define ITEM_CLONE(TName)	\
public:							\
	virtual TName* Clone() { return new (##TName)(*this); }	\
	virtual bool Clone( CItemObj* rhs ) {					\
	##TName *p = dynamic_cast<##TName*>(rhs);				\
	if( !p ) return false;									\
	*this = *p;												\
	return true;											\
	}


// a small object displayed inside a GUI object
class CItemObj		
{
public:
	CItemObj()							{}
	virtual ~CItemObj();
	ITEM_CLONE(CItemObj)

	virtual void	Render( int x, int y ){}

	virtual int		GetWidth()			{ return 0;						}
	virtual int		GetHeight()			{ return 0;						}

    virtual void    ReadyForHint( int x, int y, CCompent* pCompent ){}
    virtual void    RenderHint( int x, int y ){}

    virtual bool    HasHint()           { return false;                 }

	virtual const char*	GetString()		{ return "";					}
	virtual void		SetString( const char* str ){}

	virtual void        SetWidth(int v) {}

	virtual DWORD		GetColor()		{ return 0;						}
	virtual void		SetColor( DWORD c ){}

};

// GUI ancestor class
class CForm;
class CGuiData
{

public:
	CGuiData();
	CGuiData( const CGuiData& rhs );
	CGuiData& operator=( const CGuiData& rhs );
	virtual ~CGuiData();
	GUI_CLONE(CGuiData)

	virtual void		Init() {}			// used for external initialisation, such as loading images

	virtual void		Render(){}			// display
	virtual void		Refresh();			// absolute coordinates used to refresh the display

	// keyboard and mouse handling; returns true when already handled
	virtual bool		MouseRun( int x, int y, DWORD key );
	virtual bool		MouseScroll( int nScroll )		{ return _IsMouseIn;}

	virtual void		SetParent( CGuiData* p )		{ _pParent = p;		}
	virtual void		SetAlign( eCompentAlign v ) {}
	virtual void		SetAlpha( BYTE alpha ) {}
	virtual	void		SetIsEnabled( bool v )			{ _bEnabled = v;	 }
	virtual void		SetMargin( int left, int top, int right, int bottom ) {}

	virtual CGuiPic*	GetImage()						{ return NULL;		}

	virtual CForm*		GetForm()						{ return NULL;		}
	virtual void		SetCaption( const char * str) {}	// sets the displayed text
	virtual void		SetTextColor( DWORD color ) {}
	virtual void		SetIsDrag( bool v );

	virtual CCompent*	Find( const char* str ) { return NULL; }

	virtual void		DragRender(){}
    virtual void		SetIsShow( bool v )			    { _bShow = v;		}

    virtual void        RenderHint( int x, int y );

    virtual bool        IsAllowActive();            // decides whether it can be activated

	virtual void		Reset()					{};	// used by external calls, for things like zeroing the scroll bar

public:
	static HWND		GetHWND();						// gets the windows handle
	static CGuiData* GetGui( DWORD dwID ) {			// gets the control object by index
        if( dwID==0 )
        {
            LG( "gui", "NULL GUI\n" );
            return NULL;
        }
		if( dwID > _AllGui.size() ) 
        {
            LG( "gui", "BIG[%d] GUI\n", dwID );
            return NULL;
        }
		return _AllGui[dwID-1]; 
	}
	static void		ClearAllGui();					// clears every control
	static void		InitMemory();					// called after every control has been created

    static bool         SetHintItem( CItemObj* pObj );
    static CItemObj*    GetHintItem()                   { return _pHintItem;    }

public:
	DWORD		GetID()						{ return _dwID;					}

	const char* GetName()					{ return _strName.c_str();		}
	void		SetName( const char * name ){ _strName = name;				}

    void		SetHint( const char * str )	{ _strHint = str;			    }
    std::string		GetHint()	{ return _strHint;			    }

	void		SetPos(int nLeft, int nTop) { _nLeft = nLeft; _nTop = nTop; }
	int			GetLeft()					{ return _nLeft;				}
	int			GetTop()					{ return _nTop;					}
	int         GetRight()                  { return _nLeft + _nWidth ;     }
	int         GetBottom()                 { return  _nTop + _nHeight;     }

	int			GetX()						{ return _nX1;					}
	int			GetY()						{ return _nY1;					}
	int			GetX2()						{ return _nX2;					}
	int			GetY2()						{ return _nY2;					}

	void		SetSize( int w, int h )		{ _nWidth = w, _nHeight = h;	}
	int			GetWidth()					{ return _nWidth;				}
	int			GetHeight()					{ return _nHeight;				}

	bool		GetIsShow()					{ return _bShow;				}

	bool		GetIsEnabled()				{ return _bEnabled;				}

	bool		IsNormal()					{ return _bShow && _bEnabled;	}	

	CGuiData*	GetParent()					{ return _pParent;				}

	bool		GetIsDrag()					{ return _pDrag != NULL;		}
	CDrag*		GetDrag()					{ return _pDrag;				}
	
	void		SetPointer( void* v )		{ _pVoid = v;					}
	void*		GetPointer()				{ return _pVoid;				}

	void		ScriptSetShow( bool v )		{ _bShow = v;					}	// used by external scripts

	int			nTag;

public:
	
	bool		InRect(int x, int y)		{ return _IsMouseIn = ( x >= _nX1 && x<= _nX2 && y >=_nY1 && y <=_nY2 );	}
	bool        IsNoDrag(int x, int y, DWORD key);		// if dragging has not started and the click is inside the area, returns true

	static void SetScreen( float dx, float dy, float sx, float sy ) {
		_fDrawX = dx, _fDrawY = dy,  _fScreenX = sx,  _fScreenY = sy;
	}
	void		LineFrame();					// draws a border
	void        SelectChaLineFrame() ;          //draws a dead frame

	void		LineThinFrame();				// draws a thin frame
	void		FillFrame();					// fill

	static void	SetMousePos( int x, int y )	{ _nMouseX=x; _nMouseY=y;		}
	static int  GetMouseX()					{ return _nMouseX;				}
	static int  GetMouseY()					{ return _nMouseY;				}
	
	static void SetCursor( CCursor::eState v )	{ _eCursor = v;				}		// used only inside MouseRun
	static CCursor::eState GetCursor()		{ return _eCursor;				}

protected:
	virtual void _AddForm() {}			// adds itself to the Form it belongs to

protected:
	typedef std::vector<CGuiData*> vcs;
	static vcs	_AllGui;		// holds every control resource

	DWORD		_dwID;			// the registered control ID, that is the index; not to be changed from outside

	CGuiData*	_pParent;

	std::string		_strName;				// identifier
	int			_nWidth, _nHeight;		// width and height
	int			_nLeft, _nTop;			// relative coordinates

	int			_nX1,_nX2, _nY1, _nY2;	// absolute coordinates used for display

	bool		_bShow;			// whether it is shown
	bool		_bEnabled;		// whether it is available
	CDrag*		_pDrag;

	bool		_IsMouseIn;		// whether the mouse is inside the area
    std::string		_strHint;		// control tooltip

	void*		_pVoid;

protected:
	int			_ScreenX( int x ) { return (int)(((float)(x)) / _fScreenX); }
	int			_ScreenY( int y ) { return (int)(((float)(y)) / _fScreenY); }
    void        _RenderHint( const char* str, int x, int y );

private:
	static		float		_fDrawX, _fDrawY,  _fScreenX,  _fScreenY;
	static		CGuiPic		_imgLevel,	_imgVertical;	// the horizontal and vertical lines used to draw the border
    static      CItemObj*   _pHintItem;

	static		int			_nMouseX,	_nMouseY;		// the current mouse position
	static		CCursor::eState		_eCursor;			// the cursor shown on the gui
};

class CGuiTime;
typedef void (*GuiTimerEvent)(CGuiTime *pSender);

class CGuiTime
{
	friend class CFormMgr;
public:
	static  CGuiTime*	Create( DWORD dwInterval=0, GuiTimerEvent evt=NULL );
	bool	Release();					// external object deletion

	static void			FrameMove(DWORD dwTime);
	static  CGuiTime*	Find( DWORD id ); 

	void	SetInterval( DWORD v )	{ _dwInterval = v;		}
	DWORD	GetInterval()			{ return _dwInterval;	}

	DWORD	GetID()					{ return _dwIndex;		}

	void	SetUserID( DWORD v )	{ _nEventID=v;			}
	DWORD	GetUserID()				{ return _nEventID;		}
	
	void	SetUserPoint( PVOID v )	{ _lpData = v;			}
	PVOID	GetUserPoint()			{ return _lpData;		}
	
	bool	IsTime()				{	// whether the time point is reached; must not block
		if( _IsTime ) {
			_IsTime = false;
			return true;
		}
	}

public: // event
	GuiTimerEvent	evtTime;

private:
	DWORD	_dwIndex;			// unique ID
	DWORD 	_dwInterval;		// 等于零,不激活;

	bool	_IsTime;
	DWORD	_dwLastTime;

	// user data
	DWORD	_nEventID;
	PVOID	_lpData;

	bool	_IsRelease;

private:
	CGuiTime( DWORD dwInterval=0, GuiTimerEvent evt=NULL );
	~CGuiTime();
	void	OnTime( DWORD dwTime );

	typedef std::vector<CGuiTime*>	times;
	static times				_times;

};

// inline function
inline bool	CGuiData::IsNoDrag(int x, int y, DWORD key)
{ 
	InRect( x, y );
    if( _pDrag && _pDrag->BeginMouseRun(this, _IsMouseIn, x, y, key )==CDrag::stDrag ) return false;

	return _IsMouseIn; 
}

inline void CGuiData::_RenderHint( const char* str, int x, int y )
{
    CGuiFont::s_Font.FrameRender( str, x + 40, y );
}

inline void	CGuiTime::OnTime( DWORD dwTime )
{
	if( !_IsRelease && _dwInterval>0 && dwTime>_dwLastTime && (dwTime-_dwLastTime>_dwInterval) )
	{
		if( evtTime )
		{
			evtTime( this );
		}
		else
		{
			_IsTime = true;
		}

		_dwLastTime = dwTime;
	}
}
}

