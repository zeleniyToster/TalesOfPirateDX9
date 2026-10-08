//--------------------------------------------------------------
// Name: control interface class
// Author: lh 2004-07-08
// Purpose: determines the owning form, the currently active control, keyboard and mouse support, and focus events
// Last modified date: 2004-10-09
//--------------------------------------------------------------
#pragma once
#include "uiguidata.h"
#include "uiform.h"
#include "uicursor.h"

namespace GUI
{
class CCommandObj;
typedef void (*UseComandEvent)(CCommandObj* pItem,bool& isUse);

enum eAccept
{
	enumAccept,		// accept
	enumRefuse,		// reject
	enumFast,		// only accept shortcuts
};

enum eMouseAction
{
	enumMA_None,		// not clicked on the control

	enumMA_Gui,			// an ordinary click on the GUI
	enumMA_Skill,		// the skill can be used but the click does not pass through
	enumMA_Drill,		// the control can be clicked through
};

class CCommandObj;
class CContainer;
class CCompent : public CGuiData
{
public:
	friend class CFormMgr;
	friend class CForm;
	friend class CContainer;
public:
	CCompent(CForm& frmOwn);
	CCompent(const CCompent& rhs);
	CCompent& operator=( const CCompent& rhs );
	virtual ~CCompent(void);
	GUI_CLONE(CCompent)

	virtual bool	MouseRun( int x, int y, DWORD key );
    virtual void    Refresh();
    virtual void    FrameMove( DWORD dwTime ) {}
	virtual bool	IsFrameMove()				{ return false;		}

	eCompentAlign	GetAlign()	{ return _align; }
	virtual void	SetAlign( eCompentAlign v );
	virtual void	SetIsDrag( bool v );
    virtual void	SetIsShow( bool v );

	virtual void	SetParent( CGuiData* p );

	void			SetForm( CForm* f );
	virtual			CForm* GetForm()	        { return _frmOwn;	}

	virtual CCompent* Find( const char* str )   { return _frmOwn->Find( str ); }

    virtual CCompent* GetHintCompent( int x, int y );                       // returns the control that has a tooltip

    virtual CCompent*       GetHitCommand(int x, int y)     { return NULL;  }   // to stay compatible when it is a container, returns the bottom-most Compent
    virtual UseComandEvent  GetUseCommantEvent()            { return NULL;  }   // this event is called back from the item while the item is executed
    virtual eAccept	        SetCommand( CCommandObj* p, int x, int y )	{ return enumRefuse;    }   // whether a dragged command can be accepted

public:
	virtual void	OnActive();		// when it becomes active
	virtual void	OnLost() { if ( evtLost ) evtLost(this );	}		// when it loses focus

	virtual bool	OnKeyDown( int key )	{ return false;		}
	virtual bool	OnChar( char c )		{ return false;		}

	virtual bool	IsHandleMouse()			{ return false;		}		// whether it responds to the mouse

	static void		SetActive(CCompent* v);
	static CCompent* GetActive()			{ return _pActive;	}

	void	SetIsFocus( bool v )			{ _IsFocus = v;		}
	bool	GetIsFocus()					{ return _IsFocus;	}

    void            AddForm();

	eMouseAction	GetMouseAction()						{ return _eMouseAction;			}
	void			SetMouseAction( eMouseAction v )		{ _eMouseAction = v;			}

	static CCompent* GetLastMouseCompent()					{ return _pLastMouseCompent;	}

public:
	GuiEvent		evtLost;		// the event of losing focus
	GuiEvent		evtActive;		// the event of gaining focus

protected:
	virtual void	_AddForm();		// adds itself to the Form it belongs to
	void			_SetActive();

private:
	void			_Copy( const CCompent& rhs );

protected:
	static CCompent*	_pActive;	// among all controls only one may be active

protected:
	CForm			*_frmOwn;		// the form it belongs to; that form is responsible for displaying it and handling its events

	bool			_IsFocus;		// whether it responds to keyboard focus

	eCompentAlign	_align;
	bool			_isChild;		// whether the current control is a child control
	eMouseAction	_eMouseAction;	// mouse behaviour

	static CCompent*	_pLastMouseCompent;

};

// container class, used to display the many CCompent on it together
class CContainer : public CCompent
{
public:
	CContainer(CForm& frmOwn);
	CContainer( const CContainer& rhs );
	CContainer& operator=(const CContainer& rhs);
	virtual ~CContainer(void);
	GUI_CLONE(CContainer)

	virtual void	Init();
	virtual void	Render();
	virtual void	Refresh();
	virtual bool	MouseRun( int x, int y, DWORD key );
	virtual void	SetAlpha( BYTE alpha );
	virtual CCompent* Find( const char* str );
    virtual CCompent* GetHitCommand(int x, int y);
    virtual CCompent* GetHintCompent( int x, int y );
    virtual void    FrameMove( DWORD dwTime );

	bool			AddCompent( CCompent* p );
	CCompent*		GetIndex( unsigned int index );

	int				GetIndex( CCompent* p );		// given p, gets its index in the container; returns -1 on failure
	int				GetSize() { return (int)_items.size(); }

	void			ForEach( CompentFun pFun );
	
	virtual eAccept	SetCommand( CCommandObj* p, int x, int y )	{ return enumRefuse;			}

private:
	void			_SetSelf( const CContainer& rhs );

protected:
	typedef std::vector<CCompent*>	items;
	items			_items;
	items			_mouse;				// handles the control's mouse

};

// inline function in the class
inline void CCompent::_SetActive() 
{
    if( !_isChild && _IsFocus && GetActive()!=this )
		SetActive( this );
}

inline void CCompent::SetForm( CForm* f ) 
{ 
	_frmOwn = f;
	_pParent = f;
	_isChild = false;
}

inline CCompent* CContainer::GetIndex( unsigned int index )
{
	if( index < _items.size() ) return _items[index];
	return NULL;
}

}
