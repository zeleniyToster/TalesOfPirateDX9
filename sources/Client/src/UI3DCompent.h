//----------------------------------------------------------------------
// Name: 3D control
// Author: lh 2004-09-13
// Purpose: provides an external display interface in the GUI system
//		there are two kinds of interface:	one, external classes derive from C3DRenderObj and implement Render
//					two, register a callback to display: GUIRenderEvent, GUIRenderMsgEvent
// Last modified date: 2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "uicompent.h"

namespace GUI
{

class C3DCompent;
typedef void(*GUIRenderEvent)(C3DCompent *pSender, int x, int y);

class C3DCompent : public CCompent
{
public:
	C3DCompent(CForm& frmOwn);
	C3DCompent( const C3DCompent& rhs );
	C3DCompent& operator=(const C3DCompent& rhs);
	virtual ~C3DCompent();

	GUI_CLONE(C3DCompent)

	virtual void	Render();
	virtual bool	MouseRun( int x, int y, DWORD key );
	virtual bool	IsHandleMouse()		{ return true;			}		// whether it responds to the mouse
	virtual void	Refresh();

	void			SetRenderEvent( GUIRenderEvent p )			{ _pRenderEvent=p;			}
	GUIRenderEvent	GetRenderEvent()	{ return _pRenderEvent;	}

	int				GetCenterX()	{ return _nCenterX;		}
	int				GetCenterY()	{ return _nCenterY;		}

    void            SetPointer( void * p )  { _pPointer = p;    }
    void*           GetPointer()            { return _pPointer; }

public:
	GuiMouseEvent	evtMouseDown;		// mouse button pressed event
	GuiMouseEvent   evtMouseLDDown ;    //mouse double click event

protected:
	void			_Copy( const C3DCompent& rhs );

protected:
	GUIRenderEvent		_pRenderEvent;
	int					_nCenterX, _nCenterY;

    void*            _pPointer;

};

}
