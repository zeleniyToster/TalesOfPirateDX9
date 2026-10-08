//----------------------------------------------------------------------
// Name: button
// Author: lh 2004-07-19
// Purpose: can display text
// Last modified date: 2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "uicompent.h"
#include "UIForm.h"

namespace GUI
{

class CTextButton : public CCompent
{
public:
	enum eButtonState	// button states: normal, mouse hovering over it, pressed, disabled
	{
		csNormal = 0,
		csHover,
		csDown,
		csDisable,
		csEnd,
	};

public:
	CTextButton(CForm& frmOwn);
	CTextButton(const CTextButton& rhs);
	CTextButton& operator=(const CTextButton& rhs);
	~CTextButton(void);
	GUI_CLONE(CTextButton)

	virtual void		Render();
	virtual void		Refresh();
	virtual void		SetAlpha( BYTE alpha )  { _pImage->SetAlpha(alpha); }

	virtual bool		IsHandleMouse()         { return true;              }
	virtual bool		MouseRun( int x, int y, DWORD key );
    virtual void		FrameMove( DWORD dwTime );
	virtual bool		IsFrameMove()			{ return true;				}

	virtual CGuiPic*	GetImage()              { return _pImage;           }

	const char*			GetCaption()            {return _strCaption.c_str();}
	void				SetCaption( const char * str ) { _strCaption = str; }

	virtual	void		SetIsEnabled( bool v );

	// storage format in the file: in the order csNormal, csHover, csDown, csDisable; w,h are the cell width and height
	bool				LoadImage( const char* file, int w=32, int h=32, int tx=0, int ty=0, bool isHorizontal=true );

	DWORD				GetTextColor()						{ return _textColor;		}
	void				SetTextColor(DWORD color )			{ _textColor=color;			}
	bool                GetState()                          { return _isDown;           }

    void				SetFormModal( CForm::eModalResult v );
	CForm::eModalResult GetFormModal()						{ return _eFormModal;		}

	void				DoClick(eMouseState state = Mouse_LUp);		// external simulated click event

	// sets the blink period (milliseconds); 0 means no blinking
	void				SetFlashCycle( DWORD dwCycle = 1000 )	{ _dwFlashCycle = dwCycle;  }
	DWORD				GetFlashCycle()			{ return _dwFlashCycle;		}

public:	// events
	GuiMouseEvent		evtMouseClick;			// left mouse click event
	GuiMouseEvent		evtMouseRClick;			// right mouse click event  add by Philip.Wu 2006/01/23
	GuiMouseEvent		evtMouseDBClick;		// left mouse double click event  add by Philip.Wu 2006/01/23
	GuiEvent			evtMouseDownContinue;	// the mouse is held down on the button

protected:
	void				_SetState( eButtonState v );
	void				_ClearOldState();

protected:
	std::string				_strCaption;	// the name shown on the surface
	DWORD				_textColor;		// font colour

	CGuiPic*			_pImage;		// background image, four frames, corresponding to bsNormal, bsHover, bsDown, bsDisable
	bool				_isDown;		// already pressed on the button before, waits for the release to run the evtMouseClick event

    CForm::eModalResult _eFormModal;    // the value returned to the form when its owning form is shown modally

	DWORD				_dwFlashCycle;	// blink period; =0 means no blinking
	DWORD				_dwLastClick;	// the time of the previous click

	static CTextButton*	m_pCurButton;
};

inline void CTextButton::SetFormModal( CForm::eModalResult v )              
{ 
    if( v>=CForm::mrNone && v<CForm::mrEnd )
        _eFormModal=v;            
}

}
